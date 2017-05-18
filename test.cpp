#define DEBUG 1

#include "tensor.h"
#include "ring_buffer.h"
#include <iostream>
#include "hmm.h"
#include "hmm2s.h"
#include "scfg_gnf.h"

std::ostream& printTensorSlice(std::ostream& out, Tensor<double>& t, unsigned pos, unsigned level) {
  unsigned cur_dim_idx = t.getNumOfDimensions() - level - 1;
  unsigned block_sz = t.getSize();

  for (unsigned i = 0; i<=cur_dim_idx; i++) {
    block_sz /= t.getDimension(i);
  }

  if (level == 0) {
    for (unsigned i=0; i < t.getDimension(cur_dim_idx); i++) {
      out << ((double*)(t.mem.bytes))[pos+i] << " ";
    }
    return out;
  }
  else {
    //out << "level# " << level << ", pos# " << pos << "\n";
    //out << "cur_dim_idx " << cur_dim_idx << ", block_sz " << block_sz << "\n";
    for (unsigned i=0; i < t.getDimension(cur_dim_idx); i++) {
      printTensorSlice(out, t, pos + i*block_sz, level-1);
      out << "\n";
    }
  }
}

std::ostream& operator<<(std::ostream& out, Tensor<double>& t) {
  out << "Tensor<double>, dims={";
  for (unsigned i=0; i<t.getNumOfDimensions(); i++)
    out << t.getDimension(i) << ", ";
  out << "}\n";
  printTensorSlice(out, t, 0, t.getNumOfDimensions()-1);
  out << "\n";
  return out;
}

void test1() {
  RingBuffer<unsigned, 2> buf;
  buf.writeTop(5);
  std::cout << buf[0] << " " << buf[1] << "\n";
  buf.writeTop(4);
  std::cout << buf[0] << " " << buf[1];

}

void test2() {
  StochasticGrammarInGNF gr("ex.gnf");
  HMM2S hmm(gr);

  std::cout << hmm.startPr << hmm.transitionPr << hmm.observationPr;
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
  //test1();
  test2();

  return 0;
}
