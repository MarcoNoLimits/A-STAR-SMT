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

struct State 
{
  vector<int> sequence;
  bitset<32> covered_mask;
  double g_cost;
  double h_cost;
  double f_cost;
  int last_source_index; // Track the index of the previously placed word for
                         // distortion

  State() : g_cost(0.0), h_cost(0.0), f_cost(0.0), last_source_index(-1) 
  {
    covered_mask.reset();
  }

  // Comparator for priority queue
  bool operator<(const State &other) const 
  {
    return f_cost > other.f_cost;
  }
};

class LanguageModel 
{
public:
  bool load(const string &filepath);
  double get_transition_cost(const string &context, const string &word) const;
  double get_min_incoming_cost(const string &word) const;

private:
  unordered_map<string, double> bigram_costs;
  unordered_map<string, double> min_incoming_costs;
  double default_cost = 10.0;
};

class Dictionary 
{
public:
  bool load(const string &filepath);
  string translate(const string &en_word) const;

private:
  unordered_map<string, string> translations;
};

struct TranslationResult 
{
  vector<string> words;
  double final_f_cost;
  double final_g_cost;
  double final_h_cost;
  double initial_h_cost;
};

TranslationResult solve_permutation(const vector<string> &source_words,
                                    const LanguageModel &lm);

#endif
