# 🎯 Number Guessing Arena (C++17)

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg?style=flat-square)](#-compilation--building)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg?style=flat-square)](#-compilation--building)

```
  ╔═════════════════════════════════════════════════════════════════════════╗
  ║   _  _ _  _ _  _ ___  ____ ____    ____ _  _ ____ ____ ____ ____ ____   ║
  ║   |\ | |  | |\/| |__] |___ |__/    | __ |  | |___ [__  [__  |___ |__/   ║
  ║   | \| |__| |  | |__] |___ |  \    |__] |__| |___ ___] ___] |___ |  \   ║
  ╚═════════════════════════════════════════════════════════════════════════╝
              ★  ULTIMATE C++17 NUMBER GUESSING ARENA  ★
```

**Number Guessing Arena** is a feature-packed, production-grade terminal game built in modern **C++17**. Designed with clean object-oriented architecture, mathematical gameplay systems, intelligent AI opponents with real-time **Anti-Cheat Contradiction Detection**, persistent statistics, unlockable achievement vaults, dynamic 256-color ANSI themes, and comprehensive automated test suites.

---

## 🌟 Key Features

### 🕹️ 6 Unique Game Modes
1. **Classic Mode**: Standard range guessing ($1$ to $N$) across 4 preset difficulties or custom user ranges, equipped with real-time **Thermal Proximity Radars** (Boiling, Burning, Hot, Warm, Cold, Freezing).
2. **Survival / Hardcore Mode**: Win within theoretical mathematical limits ($\lceil\log_2 N\rceil$ attempts) before your shield lives deplete.
3. **Time Attack Mode**: High-octane speedrun against a live countdown clock ($30$s, $60$s, $90$s).
4. **Reverse AI Guesser**: Think of a number and let the AI deduce it using **Binary Search**, **Heuristic**, or **Novice Random** algorithms. Features an **Anti-Cheat Engine** that detects mathematical contradictions in player feedback in real time!
5. **Duel Arena**:
   - **Player vs AI**: Alternating turn-based race to find the secret number first.
   - **Pass & Play (2 Players)**: Local 2-player competitive showdown.
6. **Bulls & Cows (Mastermind)**: Crack a secret 4-digit code of distinct digits with exact match (*Bulls*) and value mismatch (*Cows*) feedback.

---

### 🧠 Mathematical Hint Store
Spend earned score points during active games to unlock strategic mathematical clues:
- **Parity Check**: Even or Odd.
- **Primality Test**: Prime vs Composite verification.
- **Divisibility Clue**: Factor discovery ($3, 5, 7, 10, \dots$).
- **Digit Sum & Count**: Sum of digits and total digit count.
- **Radar Shrink**: Narrows active search bounds by $40\%$.

---

### 🏆 Profiles, Ranks & Achievements
- **Dynamic Scoring**: Evaluates theoretical efficiency ($\text{Optimal Attempts} / \text{Used Attempts}$), time bonus, streak multipliers, and hint penalties.
- **Mastery Ranks**:
  - `Novice Guesser` ($< 500$ pts)
  - `Apprentice` ($500 - 1,999$ pts)
  - `Adept Guesser` ($2,000 - 4,999$ pts)
  - `Binary Seeker (★)` ($5,000 - 9,999$ pts)
  - `Master Mind Reader (★★)` ($10,000 - 14,999$ pts)
  - `Grandmaster Oracle (★★★)` ($\ge 15,000$ pts)
- **12 Unlockable Achievement Badges**:

| Badge | Title | Requirement |
| :--- | :--- | :--- |
| `[*]` | **First Steps** | Win your first number guessing game |
| `[7]` | **Lucky 777** | Guess the secret number on the very 1st try |
| `[B]` | **Binary Prodigy** | Win within theoretical $\lceil\log_2 N\rceil$ optimal attempts |
| `[S]` | **Speed Demon** | Win a game in under 10 seconds |
| `[W]` | **Streak Titan** | Reach a 5-win streak without losing |
| `[V]` | **Survival Veteran** | Beat Survival mode on Extreme difficulty |
| `[!]` | **Anti-Cheat Sleuth** | Trigger contradiction detector in Reverse AI mode |
| `[M]` | **Mastermind Ace** | Solve Bulls & Cows in $\le 6$ attempts |
| `[+]` | **Math Scholar** | Use mathematical clues and achieve victory |
| `[C]` | **Century Club** | Play 100 total rounds |
| `[D]` | **Duel Grand Champion** | Win a duel match against AI or human player |
| `[G]` | **Oracle Grandmaster** | Reach an overall career score of $10,000+$ points |

---

### 🎨 Visual Themes & UI
Customizable 256-color ANSI themes with retro box-drawing aesthetics and toggleable audio/visual feedback:
- **Neon Cyberpunk** (Cyan / Neon Magenta / Yellow)
- **Retro Emerald** (Matrix Green / Forest / Lime Gold)
- **Sunset Amber** (Warm Orange / Crimson / Amber)
- **Deep Ocean** (Sky Blue / Azure / Turquoise)
- **Monochrome Minimalist** (Clean White / Silver)

---

## 📐 Mathematical Theory: The $\lceil\log_2 N\rceil$ Bound

In an interval $[1, N]$, binary search halves the remaining search space with each query:

$$\text{Remaining Candidates after } k \text{ queries} = \frac{N}{2^k}$$

To guarantee finding the secret number:

$$\frac{N}{2^k} \le 1 \implies 2^k \ge N \implies k = \lceil\log_2 N\rceil$$

| Difficulty | Range | Maximum Candidates ($N$) | Optimal Guaranteed Attempts ($\lceil\log_2 N\rceil$) |
| :--- | :--- | :--- | :--- |
| **Easy** | $1 - 50$ | $50$ | **6** |
| **Medium** | $1 - 100$ | $100$ | **7** |
| **Hard** | $1 - 500$ | $500$ | **9** |
| **Extreme** | $1 - 1000$ | $1000$ | **10** |

---

## 📁 Project Architecture

```
Number Guessing Game/
├── .github/
│   └── workflows/
│       └── ci.yml               # Multi-OS GitHub Actions CI matrix
├── src/
│   ├── Types.hpp                # Enums, configs, structures, achievements
│   ├── Terminal.hpp             # ANSI escape codes, box drawing, colors, sounds
│   ├── Terminal.cpp
│   ├── InputValidator.hpp       # Stream sanitization, boundary checks, input recovery
│   ├── InputValidator.cpp
│   ├── RandomGenerator.hpp      # Modern C++ <random> std::mt19937 engine
│   ├── RandomGenerator.cpp
│   ├── HintSystem.hpp           # Primes, factors, digit sums, proximity radar
│   ├── HintSystem.cpp
│   ├── StatsManager.hpp         # Serialization, scoring, high scores, ranks
│   ├── StatsManager.cpp
│   ├── AiGuesser.hpp            # AI search strategies & Anti-Cheat engine
│   ├── AiGuesser.cpp
│   ├── BullsAndCows.hpp         # 4-Digit Mastermind codebreaker logic
│   ├── BullsAndCows.cpp
│   ├── GameEngine.hpp           # Mode orchestration, round lifecycles, timers
│   ├── GameEngine.cpp
│   └── main.cpp                 # Main entry point & interactive menu
├── tests/
│   └── test_runner.cpp          # Automated unit test suite (12 test suites)
├── CMakeLists.txt               # Cross-platform build script
├── build.bat                    # Fast Windows MSVC build script
├── build.sh                     # Unix GCC/Clang build script
├── .gitignore
├── LICENSE                      # MIT License
└── README.md                    # Documentation
```

---

## 🛠️ Compilation & Building

### Windows (MSVC)
Run the automated build script:
```cmd
build.bat
```
Run the game:
```cmd
bin\number_guesser.exe
```

### Linux / macOS (GCC / Clang)
```bash
chmod +x build.sh
./build.sh
./bin/number_guesser
```

### CMake (Cross-Platform)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

---

## 🧪 Running Unit Tests

To run the full suite of automated unit tests:

```cmd
bin\number_tests.exe
```

### Test Coverage Highlights:
- ✅ Uniform RNG bounds and distinct 4-digit code generation
- ✅ Information-theoretic optimal attempt calculations ($\lceil\log_2 N\rceil$)
- ✅ Primality tests, divisor factorization, and digit sum evaluations
- ✅ Proximity radar scaling across dynamic ranges
- ✅ Binary search convergence and AI strategy simulations
- ✅ **Anti-Cheat Contradiction Detection** interval validation
- ✅ Bulls & Cows exact (Bulls) and position-mismatch (Cows) evaluations
- ✅ Profile and High Scores file serialization / deserialization roundtrips

---

## 📜 License

This project is licensed under the **MIT License**. See [LICENSE](LICENSE) for details.
