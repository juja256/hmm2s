#include "scfg_gnf.h"
#include <sstream>
#include <string>
#include <stack>
#include <algorithm>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <ctime>
#include "settings.h"

unsigned sample_from_categorical(unsigned n, GNF_RULE* rules) {
  std::unique_ptr<double[]> a(new double[n+1]);
  a[0] = 0;
  #ifdef DEBUG
  std::cout << "Sampling from CDF: " << a[0] << " ";
  #endif
  for (unsigned i = 0; i < n; i++) {
    a[i+1] = a[i] + rules[i].probability;
    #ifdef DEBUG
    std::cout << a[i+1] << " ";
    #endif
  }
  #ifdef DEBUG
  std::cout << "\n";
  #endif

  double r = a[n] * ((double)rand() / RAND_MAX);
  #ifdef DEBUG
  std::cout << "Random: " << r << "\n";
  #endif
  if (r < a[n-1] * 0.5) {
    for (unsigned i=1; i<=n; i++) {
      if (r > a[i-1] && r < a[i]) {
        return i-1;
      }
    }
  }
  else {
    for (int i=n; i>=1; i--) {
      if (r > a[i-1] && r < a[i])
        return i-1;
    }
  }
}

StochasticGrammarInGNF::StochasticGrammarInGNF(const char* fn) {
  srand(time(NULL));
  std::ifstream file(fn);
  std::string line;
  unsigned n=2;
  std::getline(file, line);

  std::stringstream linestream(line);
  std::string gnf;
  if (!(linestream >> gnf) || gnf != "GNF") {
    throw StochasticGrammarException( GNF_READ_ERR | (1 << 8) );
  }
  unsigned m,l;
  if (!(linestream >> m >> l)) {
    throw StochasticGrammarException( GNF_READ_ERR | (1 << 8) );
  }
  else {
    this->M = m;
    this->L = l;
  }


  while(std::getline(file, line) && line[0] != '#') {
    std::stringstream linestream(line);
    std::string separator;
    GNF_RULE cur_rule;

    if (!(linestream >> cur_rule.probability)) {
      throw StochasticGrammarException(PROBABILITY_READ_ERR | (n >> 8));
    };
    if (!(linestream >> cur_rule.lhs_nt)) {
      throw StochasticGrammarException(LHS_NT_READ_ERR | (n >> 8));
    }
    if (!(linestream >> separator)) {
      throw StochasticGrammarException(SEPARATOR_READ_ERR | (n >> 8));
    }
    if (separator != "->") {
      throw StochasticGrammarException(SEPARATOR_INVALID | (n >> 8));
    }
    if (!(linestream >> cur_rule.rhs_t)) {
      throw StochasticGrammarException(RHS_T_READ_ERR | (n >> 8));
    }

    /* tail of the rule */
    if (!(linestream >> cur_rule.rhs_nt1)) {
      cur_rule.rhs_nt1 = EMPTY_SYMBOL;
    }
    if (!(linestream >> cur_rule.rhs_nt2)) {
      cur_rule.rhs_nt2 = EMPTY_SYMBOL;
    }

    if ( (cur_rule.lhs_nt > this->M) || (cur_rule.rhs_nt1 > this->M) || (cur_rule.rhs_nt2 > this->M) ) {
      throw StochasticGrammarException(NT_INVALID | (n >> 8));
    }
    if ( cur_rule.rhs_t > this-> L ) { /* Important: Terminal Alphabet starts from 1 !!! */
      throw StochasticGrammarException(T_INVALID | (n >> 8));
    }
    n++;
    rules.push_back(cur_rule);
  }
  /* sorting rules with respect to lhs nonterminals */
  std::sort(rules.begin(), rules.end(), [](const GNF_RULE & a, const GNF_RULE & b) -> bool { return a.lhs_nt < b.lhs_nt; });
  this->rules_cnt.push_back(0);

  /* iterators to each lhs nonterminal */
  for (unsigned i=1; i<rules.size(); i++) {
    if (rules[i-1].lhs_nt != rules[i].lhs_nt) {
      rules_cnt.push_back(i);
    }
  }

  this->rules_cnt.push_back(rules.size());

  #ifdef DEBUG
  std::cout << "Rules indices: ";
  for(unsigned i=0;i<rules_cnt.size();i++) {
    std::cout << rules_cnt[i] << " ";
  }
  std::cout << "\n";
  #endif
}

StochasticGrammarInGNF::~StochasticGrammarInGNF() { }

std::vector<unsigned> StochasticGrammarInGNF::infer(unsigned nonterminal) {
  std::stack<unsigned> state;
  std::vector<unsigned> sentance;
  state.push(nonterminal);
  while(!state.empty()) {
    unsigned cur_state = state.top();
    #ifdef DEBUG
      std::cout << "State before inference #" << cur_state << "\n";
    #endif
    state.pop();
    unsigned sampled = sample_from_categorical(this->rules_cnt[cur_state] - this->rules_cnt[cur_state-1],
    rules.data() + this->rules_cnt[cur_state-1]);
    #ifdef DEBUG
      std::cout << "Sampled raw: " << sampled << "\n";
    #endif
    unsigned cur_rule = this->rules_cnt[cur_state-1] + sampled;

    #ifdef DEBUG
      std::cout << "Rule #" << cur_rule << " is selected for inference\n" << std::flush;
    #endif
    if (this->rules[cur_rule].rhs_nt2) {
      state.push(this->rules[cur_rule].rhs_nt2);
    }
    if (this->rules[cur_rule].rhs_nt1) {
      state.push(this->rules[cur_rule].rhs_nt1);
    }
    sentance.push_back(this->rules[cur_rule].rhs_t);
  }
  return sentance;
}

unsigned StochasticGrammarInGNF::getGrammarSize() {
  return rules.size();
}

unsigned StochasticGrammarInGNF::getTerminalCount() {
  return L;
}

unsigned StochasticGrammarInGNF::getNonterminalCount() {
  return M;
}

std::vector<GNF_RULE>& StochasticGrammarInGNF::getRules () {
  return rules;
}
