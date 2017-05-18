#ifndef HMM2S_H
#define HMM2S_H

#include <stack>
#include "hmm.h"
#include "scfg_gnf.h"

#define HMM2S_HALT HMM_HALT

/* HMM with stack of order 2 by emissions */
class HMM2S: public HMM<2> {
private:
  std::stack<unsigned> stack;

public:
  HMM2S(unsigned M, unsigned L);
  HMM2S(StochasticGrammarInGNF& grammar);
  virtual unsigned start();
  virtual unsigned step();
};

#endif
