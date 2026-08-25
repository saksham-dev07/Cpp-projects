#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace MathCalc {

class EvaluationError : public std::runtime_error {
public:
    explicit EvaluationError(const std::string& msg) : std::runtime_error(msg) {}
};

class Calculator {
public:
    Calculator();

    // Evaluates mathematical expression and returns double result
    double evaluate(const std::string& expression);

    // Formats numbers cleanly (strips trailing zeros, uses scientific format for extremes)
    static std::string formatResult(double value, int precision = 8);

    static double factorial(double n);
    static double degToRad(double deg);
    static double radToDeg(double rad);

    static std::vector<std::pair<std::string, std::string>> getSupportedFunctions();
    static std::vector<std::pair<std::string, std::string>> getSupportedConstants();

private:
    enum class TokenType {
        Number,
        Identifier,
        Plus,
        Minus,
        Multiply,
        Divide,
        Modulo,
        Power,
        Factorial,
        OpenParen,
        CloseParen,
        Comma,
        End
    };

    struct Token {
        TokenType type;
        double value = 0.0;
        std::string text;
        size_t pos = 0;
    };

    std::vector<Token> tokenize(const std::string& expr);

    // Recursive-descent expression parser:
    // Expression -> Term ( ('+' | '-') Term )*
    // Term       -> Factor ( ('*' | '/' | '%') Factor )*
    // Factor     -> ('+' | '-') Factor | Power
    // Power      -> Postfix ( '^' Factor )?   [right-associative]
    // Postfix    -> Primary ('!')*
    // Primary    -> Number | Ident | Ident '(' Args ')' | '(' Expression ')'
    class Parser {
    public:
        Parser(std::vector<Token> tokens, const std::string& raw);
        double parse();

    private:
        std::vector<Token> tokens;
        size_t cursor = 0;
        std::string rawExpr;

        const Token& peek() const;
        const Token& previous() const;
        bool isAtEnd() const;
        Token advance();
        bool match(TokenType t);
        bool check(TokenType t) const;

        double expression();
        double term();
        double factor();
        double power();
        double postfix();
        double primary();
        double callFunction(const std::string& name);
    };

    std::map<std::string, double> constants;
};

} // namespace MathCalc
