#include "tensor.h"
#include "ring_buffer.h"
#include <iostream>
#include "hmm.h"
#include "hmm2s.h"
#include "scfg_gnf.h"
#include "scfl.h"
#include <map>

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
  StochasticGrammarInGNF gr("ex2.gnf");
  std::cout << "### HMM with stack test ###\n";
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
    StochasticGrammarInGNF gr("ex2.gnf");
    std::cout << "### SCFG in GNF test ###\n" <<
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

void test4() {
  StochasticGrammarInGNF first_grammar_provider("ex2.gnf");
  HMM2S second_grammar_provider(first_grammar_provider);
  std::initializer_list<std::string> vocabulary = {"a", "b", "c", "d"};
  StochasticLanguage lang1(&first_grammar_provider, vocabulary);
  StochasticLanguage lang2(&second_grammar_provider, vocabulary);
  std::cout << "SCFG in GNF provider generated language: \n";
  lang1.speakSomeText(100);

  std::cout << "HMM2S provider generated language: \n";
  lang2.speakSomeText(100);
}

void test5() {
  StochasticGrammarInGNF first_grammar_provider("ex2.gnf");
  HMM2S second_grammar_provider(first_grammar_provider);
  std::initializer_list<std::string> vocabulary = {"a", "b", "c", "d"};
  StochasticLanguage lang1(&first_grammar_provider, vocabulary);
  StochasticLanguage lang2(&second_grammar_provider, vocabulary);
  std::cout << "SCFG in GNF provider generated language: \n";
  std::map<std::string, unsigned> freq1 = lang1.countFrequencies(100);
  for (auto& x: freq1) {
    std::cout << x.first << ": " << x.second << "\n";
  }

  std::cout << "HMM2S provider generated language: \n";
  std::map<std::string, unsigned> freq2 = lang2.countFrequencies(100);
  for (auto& x: freq2) {
    std::cout << x.first << ": " << x.second << "\n";
  }
}

int main() {
  //test4();
  test5();
  return 0;
}
