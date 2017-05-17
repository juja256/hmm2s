#include "hmm2s.h"

HMM2S::HMM2S(unsigned M_, unsigned L_) : HMM<2>(M_, L_) {
  this->transitionPr = Tensor<double>({this->M, this->M, this->M});
  this->observationPr = Tensor<double>({this->M, this->M, this->M, this->L});
  this->startPr = Tensor<double>({this->M});
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
  unsigned pos = sample_from_categorical(M*M, this->transitionPr.getSlice({this->cur_state_buf[0]}));
  this->cur_state_buf.writeTop(pos / M);
  unsigned for_stack = pos % M;
  if (for_stack != EMPTY_STATE) {
    this->stack.push(for_stack);
  }
  this->takts++;

  if (this->cur_state_buf[0] != EMPTY_STATE) {
    this->cur_observation = sample_from_categorical(this->L,
      this->observationPr.getSliceForHMM2S(this->cur_state_buf, for_stack)
    );
  }
  else if (this->cur_state_buf[0] == EMPTY_STATE && this->stack.empty()) {
    this->cur_observation = HMM2S_HALT;
  }
  else if (this->cur_state_buf[0] == EMPTY_STATE && !(this->stack.empty())) {
    this->cur_observation = sample_from_categorical(this->L,
      this->observationPr.getSliceForHMM2S(this->cur_state_buf, for_stack)
    );
    this->cur_state_buf[0] = this->stack.top();
    stack.pop();
  }


  #ifdef DEBUG
  std::cout << "Stepping HMM2S" << "(" << this->M << ", " << this->L << "). Current state: "
  << this->cur_state_buf[0] << ". Stack top: " << this->stack.top() << "\n";
  #endif
  return this->cur_observation;
}
