# CLI Calculator & Unit Converter (C++17)

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square&logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg?style=flat-square)]()
[![Tests](https://img.shields.io/badge/Tests-73%20Passing-brightgreen.svg?style=flat-square)]()

A fast, modular, and lightweight CLI Calculator, Programmer Base Converter, Financial Engine, and Multi-Category Unit Converter built in modern **C++17**.

Supports both an **interactive console shell (REPL)** with ANSI terminal formatting and **direct non-interactive CLI one-shot evaluations**.

---

## Table of Contents
- [Features](#-features)
- [Quick Start](#-quick-start)
- [Usage Guide](#-usage-guide)
  - [1. Interactive Mode](#1-interactive-mode)
  - [2. CLI One-Shot Math Evaluation](#2-cli-one-shot-math-evaluation)
  - [3. CLI Unit Conversion](#3-cli-unit-conversion)
  - [4. CLI Radix Base Conversion](#4-cli-radix-base-conversion)
- [Supported Units & Functions](#-supported-units--functions)
- [Project Architecture](#-project-architecture)
- [Building & Testing](#-building--testing)
- [Uploading to GitHub](#-uploading-to-github)
- [License](#-license)

---

## 🌟 Features

- **Mathematical Expression Engine**:
  - Recursive-descent parser supporting standard arithmetic (`+`, `-`, `*`, `/`, `%`, `^`), unary negation, and nested parentheses.
  - Trigonometric, logarithmic, hyperbolic, rounding, and factorial functions.
  - Mathematical constants: `pi`, `e`, `tau`, `phi`.
- **Multi-Category Unit Converter (11 Categories, 70+ Units)**:
  - Length, Mass/Weight, Temperature (non-linear conversion), Digital Data Storage (Decimal & Binary prefixes), Time, Speed, Area, Volume, Pressure, Energy, Power.
- **Programmer / Radix & Bitwise Engine**:
  - Radix conversion across Hexadecimal, Decimal (Signed/Unsigned), Binary (formatted nibbles), Octal, and ASCII characters.
  - 64-bit Bitwise operations: `AND`, `OR`, `XOR`, `NOT`, `NAND`, `NOR`, `SHL (<<)`, `SHR (>>)`, `ROL`, `ROR`.
  - Word size masking (8, 16, 32, 64-bit), population count (`popCount`), and bit inspection grid.
- **Financial & Health Calculators**:
  - Loan / Mortgage Monthly EMI calculation with interest ratio breakdown.
  - Compound Interest with customizable compounding frequency and Simple Interest.
  - Percentage change, markdowns, and sales tax calculations.
  - Health metrics: BMI (Body Mass Index) classification and BMR (Basal Metabolic Rate via Mifflin-St Jeor equation).
- **Session History & Export**:
  - In-memory calculation logging with timestamps.
  - Export session history to formatted text files (`history.txt`).

---

## ⚡ Quick Start

### Windows (MSVC)
```cmd
# 1. Build project
.\build.bat

# 2. Run interactive app
.\bin\calculator.exe

# 3. Run test suite
.\bin\test_runner.exe
```

### Linux / macOS (GCC / Clang)
```bash
# 1. Make executable and build
chmod +x build.sh
./build.sh

# 2. Run interactive app
./bin/calculator

# 3. Run test suite
./bin/test_runner
```

### Using CMake (Cross-Platform)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

---

## 📖 Usage Guide

### 1. Interactive Mode
Run without arguments to enter the interactive shell:
```cmd
calculator
```

```text
┌────────────────────────────────────────────────────────────┐
│              CLI CALCULATOR & UNIT CONVERTER               │
│            Engineering, Math & Conversion Suite            │
└────────────────────────────────────────────────────────────┘

┌─ Main Menu ──────────────────────────────────────────────┐
│ [1] Scientific & Arithmetic Calculator (REPL)             │
│ [2] Multi-Category Unit Converter (11 Categories)         │
│ [3] Programmer / Radix Converter & Bitwise Engine         │
│ [4] Financial & Loan EMI Calculator                       │
│ [5] Health & Fitness (BMI / BMR) Calculator               │
│ [6] Session Calculation History (0 records)               │
│ [7] Quick Reference & Supported Functions                 │
│ [0] Exit                                                  │
└──────────────────────────────────────────────────────────┘
> Select Option (0-7): 
```

### 2. CLI One-Shot Math Evaluation
```bash
# Arithmetic & power
calculator "2 ^ 10 + sqrt(144) * 5"
# Output: 1084

# Functions & constants
calculator "fact(6) / (sin(pi / 2) * 10)"
# Output: 72
```

### 3. CLI Unit Conversion
```bash
# Length conversion
calculator convert 100 km mi
# Output: 62.137119

# Temperature conversion
calculator convert 32 F C
# Output: 0

# Data storage conversion
calculator convert 16 GB GiB
# Output: 14.901161
```

### 4. CLI Radix Base Conversion
```bash
# Decimal to Hex
calculator base 255 dec hex
# Output: 0x00000000000000FF

# Hex to Binary
calculator base 0xFF hex bin
# Output: 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 1111

# Binary to Decimal
calculator base 1010 bin dec
# Output: 10
```

---

## 📐 Supported Units & Functions

| Category | Supported Units |
| :--- | :--- |
| **Math Functions** | `sqrt`, `cbrt`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `log10`, `ln`, `log2`, `exp`, `abs`, `floor`, `ceil`, `round`, `fact` / `!`, `deg`, `rad` |
| **Constants** | `pi` (3.14159...), `e` (2.71828...), `tau` (6.28318...), `phi` (1.61803...) |
| **Length** | `m`, `km`, `cm`, `mm`, `um`, `nm`, `mi`, `yd`, `ft`, `in`, `nmi`, `ly`, `au` |
| **Mass** | `kg`, `g`, `mg`, `ug`, `t` (metric ton), `lb`, `oz`, `stone`, `ct`, `ton_us`, `ton_uk` |
| **Temperature** | `C` (Celsius), `F` (Fahrenheit), `K` (Kelvin), `R` (Rankine) |
| **Data Storage** | `bit`, `B`, `KB`, `MB`, `GB`, `TB`, `PB`, `KiB`, `MiB`, `GiB`, `TiB`, `PiB`, `kbit`, `mbit`, `gbit` |
| **Time** | `ns`, `us`, `ms`, `s`, `min`, `h`, `d`, `week`, `month`, `yr`, `decade`, `century` |
| **Speed** | `m/s`, `km/h`, `mph`, `knot`, `ft/s`, `mach`, `c_light` |
| **Area** | `m2`, `km2`, `cm2`, `mm2`, `ft2`, `in2`, `yd2`, `mi2`, `acre`, `ha` |
| **Volume** | `L`, `mL`, `m3`, `cm3`, `gal` (US), `gal_uk`, `qt`, `pt`, `cup`, `fl_oz`, `tbsp`, `tsp`, `ft3`, `in3` |
| **Pressure** | `Pa`, `kPa`, `MPa`, `bar`, `mbar`, `psi`, `atm`, `mmHg` (torr), `inHg` |
| **Energy** | `J`, `kJ`, `MJ`, `cal`, `kcal`, `Wh`, `kWh`, `BTU`, `eV` |
| **Power** | `W`, `kW`, `MW`, `hp`, `cal/s`, `BTU/h` |

---

## 🏗️ Project Architecture

```
Calculator_UnitConverter/
├── .github/
│   └── workflows/
│       └── ci.yml                 # Cross-platform GitHub Actions CI
├── .gitignore                     # Git ignore rules for build artifacts
├── CMakeLists.txt                 # CMake configuration
├── build.bat                      # 1-click Windows MSVC build script
├── build.sh                       # 1-click Linux/macOS build script
├── LICENSE                        # MIT License
├── README.md                      # Documentation
├── src/
│   ├── main.cpp                   # Application entry & CLI argument parsing
│   ├── UI.hpp / .cpp              # ANSI console styling & table formatting
│   ├── Calculator.hpp / .cpp      # Expression parser & mathematical evaluator
│   ├── UnitConverter.hpp / .cpp   # 11 Unit categories & lookup system
│   ├── ProgrammerCalc.hpp / .cpp  # Radix (Hex/Dec/Bin/Oct) & 64-bit bitwise engine
│   ├── FinanceHealthCalc.hpp/.cpp # Financial (EMI, Interest) & Health (BMI, BMR)
│   └── History.hpp / .cpp         # History logging & file export
└── tests/
    └── test_runner.cpp            # 73-point automated unit test suite
```

---

## 📤 Uploading to GitHub

To publish this project to your GitHub account:

```bash
# 1. Navigate to the project root
cd "c:\Users\agarw\OneDrive\Desktop\C++ projects\Calculator_UnitConverter"

# 2. Initialize git repository
git init

# 3. Stage all files (cleanly filtered by .gitignore)
git add .

# 4. Commit files
git commit -m "Initial commit: CLI Calculator & Multi-Unit Converter in C++17"

# 5. Link to your remote GitHub repo
git branch -M main
git remote add origin https://github.com/<YOUR_USERNAME>/<REPO_NAME>.git

# 6. Push to GitHub
git push -u origin main
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
