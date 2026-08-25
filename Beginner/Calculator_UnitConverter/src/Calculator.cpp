#include "Calculator.hpp"
#include <cmath>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace MathCalc {

Calculator::Calculator() {
    constants["pi"]  = 3.14159265358979323846;
    constants["e"]   = 2.71828182845904523536;
    constants["tau"] = 6.28318530717958647692;
    constants["phi"] = 1.61803398874989484820;
}

std::vector<std::pair<std::string, std::string>> Calculator::getSupportedFunctions() {
    return {
        {"sqrt(x)", "Square root (x >= 0)"},
        {"cbrt(x)", "Cube root"},
        {"sin(x), cos(x), tan(x)", "Trigonometric functions (in radians)"},
        {"asin(x), acos(x), atan(x)", "Inverse trigonometric functions"},
        {"sinh(x), cosh(x), tanh(x)", "Hyperbolic functions"},
        {"log(x) / log10(x)", "Base-10 logarithm"},
        {"ln(x)", "Natural logarithm (base e)"},
        {"log2(x)", "Base-2 logarithm"},
        {"exp(x)", "e^x exponential"},
        {"abs(x)", "Absolute value"},
        {"floor(x), ceil(x), round(x)", "Rounding functions"},
        {"fact(x) / x!", "Factorial (integer x >= 0)"},
        {"deg(x), rad(x)", "Angle conversion (rad <-> deg)"}
    };
}

std::vector<std::pair<std::string, std::string>> Calculator::getSupportedConstants() {
    return {
        {"pi",  "3.1415926535... (Circle circumference ratio)"},
        {"e",   "2.7182818284... (Euler's number)"},
        {"tau", "6.2831853071... (2 * pi)"},
        {"phi", "1.6180339887... (Golden ratio)"}
    };
}

double Calculator::degToRad(double deg) {
    return deg * (3.14159265358979323846 / 180.0);
}

double Calculator::radToDeg(double rad) {
    return rad * (180.0 / 3.14159265358979323846);
}

double Calculator::factorial(double n) {
    if (n < 0) {
        throw EvaluationError("Factorial is undefined for negative values.");
    }
    double intPart;
    if (std::modf(n, &intPart) != 0.0) {
        return std::tgamma(n + 1.0);
    }
    if (n > 170.0) {
        throw EvaluationError("Factorial overflow (max supported is 170!).");
    }
    long long k = static_cast<long long>(n);
    double ans = 1.0;
    for (long long i = 2; i <= k; ++i) {
        ans *= static_cast<double>(i);
    }
    return ans;
}

std::string Calculator::formatResult(double value, int precision) {
    if (std::isnan(value)) return "NaN";
    if (std::isinf(value)) return (value > 0) ? "Infinity" : "-Infinity";

    double absVal = std::abs(value);
    if (absVal != 0.0 && (absVal >= 1e15 || absVal < 1e-6)) {
        std::ostringstream ss;
        ss << std::scientific << std::setprecision(precision) << value;
        return ss.str();
    }

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string str = ss.str();

    // Strip trailing decimals
    if (str.find('.') != std::string::npos) {
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back();
    }
    if (str == "-0") str = "0";
    return str;
}

std::vector<Calculator::Token> Calculator::tokenize(const std::string& expr) {
    std::vector<Token> list;
    size_t i = 0;
    size_t len = expr.length();

    while (i < len) {
        char ch = expr[i];

        if (std::isspace(static_cast<unsigned char>(ch))) {
            i++;
            continue;
        }

        // Numbers: digits and floating point
        if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
            size_t start = i;
            bool dotFound = (ch == '.');
            i++;
            while (i < len && (std::isdigit(static_cast<unsigned char>(expr[i])) || (!dotFound && expr[i] == '.'))) {
                if (expr[i] == '.') dotFound = true;
                i++;
            }
            // Scientific notation: 1e5 or 2.5e-3
            if (i < len && (expr[i] == 'e' || expr[i] == 'E')) {
                size_t next = i + 1;
                if (next < len && (expr[next] == '+' || expr[next] == '-')) {
                    next++;
                }
                if (next < len && std::isdigit(static_cast<unsigned char>(expr[next]))) {
                    i = next;
                    while (i < len && std::isdigit(static_cast<unsigned char>(expr[i]))) {
                        i++;
                    }
                }
            }

            std::string numStr = expr.substr(start, i - start);
            if (numStr == ".") {
                throw EvaluationError("Stray decimal point at column " + std::to_string(start + 1));
            }
            list.push_back({TokenType::Number, std::stod(numStr), numStr, start});
            continue;
        }

        // Identifiers: functions, variables, constants
        if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
            size_t start = i;
            while (i < len && (std::isalnum(static_cast<unsigned char>(expr[i])) || expr[i] == '_')) {
                i++;
            }
            std::string ident = expr.substr(start, i - start);
            std::string lower = ident;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            list.push_back({TokenType::Identifier, 0.0, lower, start});
            continue;
        }

        // Operators & delimiters
        switch (ch) {
            case '+': list.push_back({TokenType::Plus, 0.0, "+", i++}); break;
            case '-': list.push_back({TokenType::Minus, 0.0, "-", i++}); break;
            case '*': list.push_back({TokenType::Multiply, 0.0, "*", i++}); break;
            case '/': list.push_back({TokenType::Divide, 0.0, "/", i++}); break;
            case '%': list.push_back({TokenType::Modulo, 0.0, "%", i++}); break;
            case '^': list.push_back({TokenType::Power, 0.0, "^", i++}); break;
            case '!': list.push_back({TokenType::Factorial, 0.0, "!", i++}); break;
            case '(': list.push_back({TokenType::OpenParen, 0.0, "(", i++}); break;
            case ')': list.push_back({TokenType::CloseParen, 0.0, ")", i++}); break;
            case ',': list.push_back({TokenType::Comma, 0.0, ",", i++}); break;
            default:
                throw EvaluationError(std::string("Unexpected character '") + ch + "' at column " + std::to_string(i + 1));
        }
    }

    list.push_back({TokenType::End, 0.0, "", len});
    return list;
}

double Calculator::evaluate(const std::string& expression) {
    if (expression.empty() || expression.find_first_not_of(" \t\r\n") == std::string::npos) {
        throw EvaluationError("Expression is empty.");
    }
    auto tokens = tokenize(expression);
    Parser parser(std::move(tokens), expression);
    return parser.parse();
}

Calculator::Parser::Parser(std::vector<Token> t, const std::string& raw)
    : tokens(std::move(t)), cursor(0), rawExpr(raw) {}

const Calculator::Token& Calculator::Parser::peek() const {
    return tokens[cursor];
}

const Calculator::Token& Calculator::Parser::previous() const {
    return tokens[cursor - 1];
}

bool Calculator::Parser::isAtEnd() const {
    return peek().type == TokenType::End;
}

Calculator::Token Calculator::Parser::advance() {
    if (!isAtEnd()) cursor++;
    return previous();
}

bool Calculator::Parser::check(TokenType t) const {
    if (isAtEnd()) return false;
    return peek().type == t;
}

bool Calculator::Parser::match(TokenType t) {
    if (check(t)) {
        advance();
        return true;
    }
    return false;
}

double Calculator::Parser::parse() {
    double res = expression();
    if (!isAtEnd()) {
        throw EvaluationError("Unexpected token '" + peek().text + "' at column " + std::to_string(peek().pos + 1));
    }
    return res;
}

double Calculator::Parser::expression() {
    double lhs = term();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        TokenType op = previous().type;
        double rhs = term();
        if (op == TokenType::Plus) {
            lhs += rhs;
        } else {
            lhs -= rhs;
        }
    }

    return lhs;
}

double Calculator::Parser::term() {
    double lhs = factor();

    while (match(TokenType::Multiply) || match(TokenType::Divide) || match(TokenType::Modulo)) {
        TokenType op = previous().type;
        size_t opPos = previous().pos;
        double rhs = factor();
        if (op == TokenType::Multiply) {
            lhs *= rhs;
        } else if (op == TokenType::Divide) {
            if (rhs == 0.0) {
                throw EvaluationError("Division by zero at column " + std::to_string(opPos + 1));
            }
            lhs /= rhs;
        } else if (op == TokenType::Modulo) {
            if (rhs == 0.0) {
                throw EvaluationError("Modulo by zero at column " + std::to_string(opPos + 1));
            }
            lhs = std::fmod(lhs, rhs);
        }
    }

    return lhs;
}

double Calculator::Parser::factor() {
    if (match(TokenType::Minus)) {
        return -factor();
    }
    if (match(TokenType::Plus)) {
        return factor();
    }
    return power();
}

double Calculator::Parser::power() {
    double lhs = postfix();

    // Exponentiation is right-associative: a ^ b ^ c = a ^ (b ^ c)
    if (match(TokenType::Power)) {
        double rhs = factor();
        if (lhs == 0.0 && rhs < 0.0) {
            throw EvaluationError("Cannot raise zero to a negative exponent.");
        }
        if (lhs < 0.0 && std::floor(rhs) != rhs) {
            throw EvaluationError("Negative base with non-integer exponent produces a complex number.");
        }
        return std::pow(lhs, rhs);
    }

    return lhs;
}

double Calculator::Parser::postfix() {
    double val = primary();

    while (match(TokenType::Factorial)) {
        val = Calculator::factorial(val);
    }

    return val;
}

double Calculator::Parser::primary() {
    if (match(TokenType::Number)) {
        return previous().value;
    }

    if (match(TokenType::Identifier)) {
        std::string name = previous().text;
        size_t pos = previous().pos;

        // Function call: ident followed by '('
        if (check(TokenType::OpenParen)) {
            return callFunction(name);
        }

        // Known mathematical constants
        if (name == "pi")  return 3.14159265358979323846;
        if (name == "e")   return 2.71828182845904523536;
        if (name == "tau") return 6.28318530717958647692;
        if (name == "phi") return 1.61803398874989484820;

        throw EvaluationError("Unknown identifier '" + name + "' at column " + std::to_string(pos + 1));
    }

    if (match(TokenType::OpenParen)) {
        size_t openPos = previous().pos;
        double sub = expression();
        if (!match(TokenType::CloseParen)) {
            throw EvaluationError("Unclosed parenthesis starting at column " + std::to_string(openPos + 1));
        }
        return sub;
    }

    if (isAtEnd()) {
        throw EvaluationError("Unexpected end of expression.");
    }

    throw EvaluationError("Syntax error near '" + peek().text + "' at column " + std::to_string(peek().pos + 1));
}

double Calculator::Parser::callFunction(const std::string& name) {
    advance(); // Consume '('
    size_t parenPos = previous().pos;

    std::vector<double> args;
    if (!check(TokenType::CloseParen)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::Comma));
    }

    if (!match(TokenType::CloseParen)) {
        throw EvaluationError("Expected ')' to close function '" + name + "' at column " + std::to_string(parenPos + 1));
    }

    if (name == "sqrt") {
        if (args.size() != 1) throw EvaluationError("sqrt() takes 1 argument");
        if (args[0] < 0.0) throw EvaluationError("sqrt() domain error: argument must be >= 0");
        return std::sqrt(args[0]);
    }
    if (name == "cbrt") {
        if (args.size() != 1) throw EvaluationError("cbrt() takes 1 argument");
        return std::cbrt(args[0]);
    }
    if (name == "sin") {
        if (args.size() != 1) throw EvaluationError("sin() takes 1 argument");
        return std::sin(args[0]);
    }
    if (name == "cos") {
        if (args.size() != 1) throw EvaluationError("cos() takes 1 argument");
        return std::cos(args[0]);
    }
    if (name == "tan") {
        if (args.size() != 1) throw EvaluationError("tan() takes 1 argument");
        return std::tan(args[0]);
    }
    if (name == "asin") {
        if (args.size() != 1) throw EvaluationError("asin() takes 1 argument");
        if (args[0] < -1.0 || args[0] > 1.0) throw EvaluationError("asin() value must be in [-1, 1]");
        return std::asin(args[0]);
    }
    if (name == "acos") {
        if (args.size() != 1) throw EvaluationError("acos() takes 1 argument");
        if (args[0] < -1.0 || args[0] > 1.0) throw EvaluationError("acos() value must be in [-1, 1]");
        return std::acos(args[0]);
    }
    if (name == "atan") {
        if (args.size() == 1) return std::atan(args[0]);
        if (args.size() == 2) return std::atan2(args[0], args[1]);
        throw EvaluationError("atan() takes 1 or 2 arguments");
    }
    if (name == "sinh") {
        if (args.size() != 1) throw EvaluationError("sinh() takes 1 argument");
        return std::sinh(args[0]);
    }
    if (name == "cosh") {
        if (args.size() != 1) throw EvaluationError("cosh() takes 1 argument");
        return std::cosh(args[0]);
    }
    if (name == "tanh") {
        if (args.size() != 1) throw EvaluationError("tanh() takes 1 argument");
        return std::tanh(args[0]);
    }
    if (name == "log" || name == "log10") {
        if (args.size() != 1) throw EvaluationError("log() takes 1 argument");
        if (args[0] <= 0.0) throw EvaluationError("log() domain error: argument must be > 0");
        return std::log10(args[0]);
    }
    if (name == "ln") {
        if (args.size() != 1) throw EvaluationError("ln() takes 1 argument");
        if (args[0] <= 0.0) throw EvaluationError("ln() domain error: argument must be > 0");
        return std::log(args[0]);
    }
    if (name == "log2") {
        if (args.size() != 1) throw EvaluationError("log2() takes 1 argument");
        if (args[0] <= 0.0) throw EvaluationError("log2() domain error: argument must be > 0");
        return std::log2(args[0]);
    }
    if (name == "exp") {
        if (args.size() != 1) throw EvaluationError("exp() takes 1 argument");
        return std::exp(args[0]);
    }
    if (name == "abs") {
        if (args.size() != 1) throw EvaluationError("abs() takes 1 argument");
        return std::abs(args[0]);
    }
    if (name == "floor") {
        if (args.size() != 1) throw EvaluationError("floor() takes 1 argument");
        return std::floor(args[0]);
    }
    if (name == "ceil") {
        if (args.size() != 1) throw EvaluationError("ceil() takes 1 argument");
        return std::ceil(args[0]);
    }
    if (name == "round") {
        if (args.size() != 1) throw EvaluationError("round() takes 1 argument");
        return std::round(args[0]);
    }
    if (name == "pow") {
        if (args.size() != 2) throw EvaluationError("pow(base, exp) takes 2 arguments");
        return std::pow(args[0], args[1]);
    }
    if (name == "fact" || name == "factorial") {
        if (args.size() != 1) throw EvaluationError("fact() takes 1 argument");
        return Calculator::factorial(args[0]);
    }
    if (name == "deg") {
        if (args.size() != 1) throw EvaluationError("deg() takes 1 argument");
        return Calculator::radToDeg(args[0]);
    }
    if (name == "rad") {
        if (args.size() != 1) throw EvaluationError("rad() takes 1 argument");
        return Calculator::degToRad(args[0]);
    }

    throw EvaluationError("Unknown function '" + name + "'");
}

} // namespace MathCalc
