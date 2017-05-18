#include "hmm2s.h"
#include "settings.h"
#include "scfg_gnf.h"
#include <vector>

HMM2S::HMM2S(unsigned M_, unsigned L_) : HMM<2>(M_, L_) {
  this->transitionPr = Tensor<double>({this->M, this->M, this->M});
  this->observationPr = Tensor<double>({this->M, this->M, this->M, this->L});
  this->startPr = Tensor<double>({this->M});
}

/*
  SCFG in GNF to HMM2 with stack converter
  Scenaries:
    A -> A1, Stack.Push A2
    A -> A1
    A -> Stack.Top
*/
HMM2S::HMM2S(StochasticGrammarInGNF& grammar): HMM2S(grammar.getNonterminalCount()+1, grammar.getTerminalCount()+1) {
  this->startPr.setElement({START_SYMBOL}, 1.0); // Start state is 1 !
  std::vector<GNF_RULE>& rules = grammar.getRules();
  unsigned size = grammar.getGrammarSize();
  for (unsigned i=0; i<size; i++) {
    /* marginalization: Pr(A -> A1, push A2) = Pr(A -> a A1 A2) + Pr(A -> b A1 A2) */
    this->transitionPr.setElement({rules[i].lhs_nt, rules[i].rhs_nt1, rules[i].rhs_nt2},
      this->transitionPr.getElement({rules[i].lhs_nt, rules[i].rhs_nt1, rules[i].rhs_nt2}) + rules[i].probability);
    /* initialization of emission weights from raw rules's probabilities of grammar */
    this->observationPr.setElement({rules[i].lhs_nt, rules[i].rhs_nt1, rules[i].rhs_nt2, rules[i].rhs_t}, rules[i].probability);

  }
}

unsigned HMM2S::start() {
  this->cur_state_buf.writeTop(
    sample_from_categorical(M, startPr.getSlice({}) )
  );
  this->takts++;

  this->cur_observation = NO_OBSERVATION;

  #ifdef DEBUG
  std::cout << "Starting HMM2S" << "(" << this->M << ", " << this->L << "). Start state: "
  << this->cur_state_buf[0] << "\n";
  #endif
  return this->cur_observation;
}

unsigned HMM2S::step() {
  if ((this->cur_state_buf[0] == EMPTY_STATE) && stack.empty()) {
    //#ifdef DEBUG
      std::cout << "Halting HMM2S!\n";
    //#endif
    return HMM2S_HALT;
  }
  unsigned pos = sample_from_categorical(M*M, this->transitionPr.getSlice({this->cur_state_buf[0]}));
  this->cur_state_buf.writeTop(pos / M);
  unsigned for_stack = pos % M;
  if (for_stack != EMPTY_STATE) {
    this->stack.push(for_stack);
  }
  #ifdef DEBUG
    std::cout << "HMM2S is jumping to state #" << this->cur_state_buf[0] << " pushing #" << for_stack << " to stack\n";
  #endif
  this->takts++;

  this->cur_observation = sample_from_categorical(this->L,
    this->observationPr.getSliceForHMM2S(this->cur_state_buf, for_stack)
  );

  if (this->cur_state_buf[0] == EMPTY_STATE && !(this->stack.empty())) {
    this->cur_state_buf[0] = this->stack.top();
    stack.pop();
  }


  #ifdef DEBUG

  std::cout << "Stepping HMM2S" << "(" << this->M << ", " << this->L << "). Current state: "
    << this->cur_state_buf[0] << ". \n";
  if (!stack.empty())
    std::cout << "Stack top: " << this->stack.top() << "\n";
  #endif
  return this->cur_observation;
}
