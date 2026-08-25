#pragma once

#include "Types.hpp"
#include <vector>
#include <array>
#include <map>
#include <string>
#include <memory>

namespace rps {

class AiPredictor {
public:
    virtual ~AiPredictor() = default;

    virtual void reset() = 0;
    virtual void recordRound(Move playerMove, Move aiMove, Outcome outcome) = 0;
    virtual Move selectMove(RuleSet ruleSet) = 0;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual AiPersonality getPersonality() const = 0;

    static std::unique_ptr<AiPredictor> create(AiPersonality personality);
};

// Level 1: Uniform Random
class RandomAi : public AiPredictor {
public:
    void reset() override {}
    void recordRound(Move, Move, Outcome) override {}
    Move selectMove(RuleSet ruleSet) override;
    std::string getName() const override { return "Chaos Randomizer"; }
    std::string getDescription() const override { return "Plays purely random moves uniformly across options."; }
    AiPersonality getPersonality() const override { return AiPersonality::Random; }
};

// Level 2: Rock-Biased Brute
class BruteAi : public AiPredictor {
public:
    void reset() override {}
    void recordRound(Move, Move, Outcome) override {}
    Move selectMove(RuleSet ruleSet) override;
    std::string getName() const override { return "Rocky Brute"; }
    std::string getDescription() const override { return "Prefers heavy moves like Rock and Spock."; }
    AiPersonality getPersonality() const override { return AiPersonality::Brute; }
};

// Level 3: Mirror Mimic
class MimicAi : public AiPredictor {
public:
    void reset() override;
    void recordRound(Move playerMove, Move aiMove, Outcome outcome) override;
    Move selectMove(RuleSet ruleSet) override;
    std::string getName() const override { return "Mirror Mimic"; }
    std::string getDescription() const override { return "Copies your last move or anticipates your counter."; }
    AiPersonality getPersonality() const override { return AiPersonality::Mimic; }

private:
    Move m_lastPlayerMove{Move::None};
};

// Level 4: Win-Stay, Lose-Shift
class WslsAi : public AiPredictor {
public:
    void reset() override;
    void recordRound(Move playerMove, Move aiMove, Outcome outcome) override;
    Move selectMove(RuleSet ruleSet) override;
    std::string getName() const override { return "WSLS Tactician"; }
    std::string getDescription() const override { return "Exploits human Win-Stay / Lose-Shift tendencies."; }
    AiPersonality getPersonality() const override { return AiPersonality::TacticianWSLS; }

private:
    Move m_lastPlayerMove{Move::None};
    Move m_lastAiMove{Move::None};
    Outcome m_lastOutcome{Outcome::Tie};
};

// Level 5: 2nd-order Markov Chain
class MarkovOracleAi : public AiPredictor {
public:
    MarkovOracleAi();
    void reset() override;
    void recordRound(Move playerMove, Move aiMove, Outcome outcome) override;
    Move selectMove(RuleSet ruleSet) override;
    std::string getName() const override { return "Markov Oracle"; }
    std::string getDescription() const override { return "Learns player move sequences using a decay-weighted Markov model."; }
    AiPersonality getPersonality() const override { return AiPersonality::MarkovOracle; }

    double getTransitionProbability(Move prevMove, Move nextMove, RuleSet ruleSet) const;
    Move predictMostLikelyPlayerMove(RuleSet ruleSet) const;

private:
    std::array<std::array<double, 6>, 6> m_firstOrderMatrix{};
    std::map<uint32_t, std::array<double, 6>> m_secondOrderMatrix;

    std::vector<Move> m_playerHistory;
    std::vector<Move> m_aiHistory;
    double m_decay{0.96};
    double m_epsilon{0.08};
};

} // namespace rps
