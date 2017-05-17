#ifndef SCFG_GNF_H
#define SCFG_GNF_H

#define EMPTY_SYMBOL 0
#define START_SYMBOL 1

/* Exceptions */
#define PROBABILITY_READ_ERR 1
#define LHS_NT_READ_ERR 2
#define SEPARATOR_READ_ERR 3
#define SEPARATOR_INVALID 4
#define RHS_T_READ_ERR 5
#define GNF_READ_ERR 6
#define NT_INVALID 7
#define T_INVALID 8

#include <vector>

typedef struct {
  double probability;
  unsigned lhs_nt;
  unsigned rhs_t;
  unsigned rhs_nt1;
  unsigned rhs_nt2;
} GNF_RULE; // 24 bytes

unsigned sample_from_categorical(unsigned n, GNF_RULE* pr);

class StochasticGrammarException {
  unsigned e;
public:
  StochasticGrammarException(unsigned err) : e(err) {}
  int getErrorCode() { return e; }
};

class StochasticGrammar {
public:
  virtual std::vector<unsigned> infer(unsigned nonterminal=START_SYMBOL)=0;
  virtual ~StochasticGrammar() {}
};

class StochasticGrammarInGNF : public StochasticGrammar {
  unsigned M, L;
  std::vector<GNF_RULE> rules;
  std::vector<unsigned> rules_cnt;
public:
  StochasticGrammarInGNF(const char* fn);
  ~StochasticGrammarInGNF();
  virtual std::vector<unsigned> infer(unsigned nonterminal=START_SYMBOL);
  unsigned getGrammarSize();
  unsigned getTerminalCount();
  unsigned getNonterminalCount();
  std::vector<GNF_RULE> getRules();
};

#endif
