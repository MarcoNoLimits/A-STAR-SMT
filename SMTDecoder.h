#ifndef SMT_DECODER_H
#define SMT_DECODER_H

#include <algorithm>
#include <bitset>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

struct State {
  vector<int> sequence;
  bitset<32> covered_mask;
  double g_cost;
  double h_cost;
  double f_cost;

  State() : g_cost(0.0), h_cost(0.0), f_cost(0.0) { covered_mask.reset(); }
};

class LanguageModel {
public:
  bool load(const string &filepath);
  double get_transition_cost(const string &context,
                             const string &word) const;
  double get_min_incoming_cost(const string &word) const;

private:
  unordered_map<string, double> bigram_costs;
  unordered_map<string, double> min_incoming_costs;
  double default_cost = 10.0;
};

class Dictionary {
public:
  bool load(const string &filepath);
  string translate(const string &en_word) const;

private:
  unordered_map<string, string> translations;
};

vector<string>
solve_permutation(const vector<string> &source_words,
                  const LanguageModel &lm);

#endif // SMT_DECODER_H
