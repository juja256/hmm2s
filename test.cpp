#define DEBUG 1

#include "tensor.h"
#include "ring_buffer.h"
#include <iostream>
#include "hmm.h"
#include "hmm2s.h"
#include "scfg_gnf.h"



void test1() {
  unsigned d = 7;
  Tensor<float> a({2,2,d});

  for (int i=0; i<2; i++)
    for (int j=0; j<2; j++)
      for (int k=0; k<d; k++)
        std::cout<<a.getElement({i,j,k}) << " ";
  RingBuffer<unsigned, 4> b;
  std::cout << "\n";
}

void test2() {
  StochasticGrammarInGNF gr("ex.gnf");
  HMM2S hmm(gr);
  std::vector<unsigned> s = hmm.run();
  std::cout << "Infered sentance of length " << s.size() << "\n";
  for (unsigned i=0; i<s.size(); i++) {
    std::cout << s[i] << " ";
  }
  std::cout << "\n";
}

void test3() {
  try{
    StochasticGrammarInGNF gr("ex.gnf");
    std::cout << "SCFG in GNF test\n" <<
                 "size = " << gr.getGrammarSize() << "\n" <<
                 "M = " << gr.getNonterminalCount() << "\n" <<
                 "L = " << gr.getTerminalCount() << "\n";
    std::vector<unsigned> s = gr.infer();
    std::cout << "Infered sentance of length " << s.size() << "\n";
    for (unsigned i=0; i<s.size(); i++) {
      std::cout << s[i] << " ";
    }
    std::cout << "\n";
  }
  catch(StochasticGrammarException& e) {
    std::cout << "Error occured #" << std::hex << e.getErrorCode() << "\n";
  }

}

int main() {
  test2();

  return 0;
}
