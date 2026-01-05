#include "SMTDecoder.h"
#include <fstream>
#include <iostream>
using namespace std;

// =========================================================
// LanguageModel Implementation
// =========================================================

bool LanguageModel::load(const string &filepath) {
  ifstream infile(filepath);
  if (!infile.is_open())
    return false;

  string w1, w2;
  double val;
  // Simple extraction: reads word1, word2, then the number.
  // Skips whitespace automatically. No stringstream needed.
  while (infile >> w1 >> w2 >> val) {
    double cost = val;
    // Handle negative log probs if present
    if (cost < 0)
      cost = -cost;

    bigram_costs[w1 + "|" + w2] = cost;

    // Update Min Incoming Cost for w2 manually
    if (min_incoming_costs.find(w2) == min_incoming_costs.end()) {
      min_incoming_costs[w2] = cost;
    } else {
      if (cost < min_incoming_costs[w2]) {
        min_incoming_costs[w2] = cost;
      }
    }
  }
  return true;
}

double LanguageModel::get_transition_cost(const string &context,
                                          const string &word) const {
  string key = context + "|" + word;
  unordered_map<string, double>::const_iterator it = bigram_costs.find(key);
  if (it != bigram_costs.end()) {
    return it->second;
  }
  return default_cost;
}

double LanguageModel::get_min_incoming_cost(const string &word) const {
  unordered_map<string, double>::const_iterator it =
      min_incoming_costs.find(word);
  if (it != min_incoming_costs.end()) {
    return it->second;
  }
  return 0.0;
}

// =========================================================
// Dictionary Implementation
// =========================================================

bool Dictionary::load(const string &filepath) {
  ifstream infile(filepath);
  if (!infile.is_open())
    return false;

  string en, fr;
  // Simple loop
  while (infile >> en >> fr) {
    translations[en] = fr;
  }
  return true;
}

string Dictionary::translate(const string &en_word) const {
  unordered_map<string, string>::const_iterator it = translations.find(en_word);
  if (it != translations.end()) {
    return it->second;
  }
  return en_word;
}

// =========================================================
// A* Solver Implementation
// =========================================================

double calculate_heuristic(const vector<string> &words,
                           const bitset<32> &covered, const LanguageModel &lm) {
  double h_val = 0.0;
  int n = (int)words.size(); // Explicit cast to int

  for (int i = 0; i < n; ++i) {
    if (!covered.test(i)) {
      h_val += lm.get_min_incoming_cost(words[i]);
    }
  }
  return h_val;
}

struct StateComparator {
  bool operator()(const State &a, const State &b) {
    return a.f_cost > b.f_cost;
  }
};

TranslationResult solve_permutation(const vector<string> &source_words,
                                    const LanguageModel &lm) {
  int n = (int)source_words.size();

  priority_queue<State, vector<State>, StateComparator> open_set;

  State start_node;
  start_node.g_cost = 0.0;
  start_node.h_cost =
      calculate_heuristic(source_words, start_node.covered_mask, lm);
  start_node.f_cost = start_node.g_cost + start_node.h_cost;

  double initial_h_estimate = start_node.h_cost; // Capture initial heuristic

  open_set.push(start_node);

  while (!open_set.empty()) {
    State current = open_set.top();
    open_set.pop();

    if (current.covered_mask.count() == n) {
      TranslationResult result;
      for (int i = 0; i < (int)current.sequence.size(); ++i) {
        int original_index = current.sequence[i];
        result.words.push_back(source_words[original_index]);
      }
      result.final_f_cost = current.f_cost;
      result.final_g_cost = current.g_cost;
      result.final_h_cost = current.h_cost;
      result.initial_h_cost = initial_h_estimate;
      return result;
    }

    for (int i = 0; i < n; ++i) {
      if (!current.covered_mask.test(i)) {
        State next_state = current;
        next_state.sequence.push_back(i);
        next_state.covered_mask.set(i);

        string prev_word = "<s>";
        if (!current.sequence.empty()) {
          int last_idx = current.sequence.back();
          prev_word = source_words[last_idx];
        }
        string curr_word = source_words[i];

        double trans_cost = lm.get_transition_cost(prev_word, curr_word);

        // Distortion Cost: |index_curr - index_prev - 1|
        int prev_index = current.last_source_index;
        // If first word (prev_index == -1), we usually assume it follows a
        // virtual start at position -1, so 0 - (-1) - 1 = 0 dist. Or treat
        // start as separate. Let's assume monotonic start means i=0 is cost 0.
        // |0 - (-1) - 1| = 0.
        // If we pick i=2 first: |2 - (-1) - 1| = 2.
        int dist = std::abs(i - prev_index - 1);

        next_state.g_cost +=
            trans_cost + (double)dist; // Add distortion to g_cost
        next_state.last_source_index = i;

        next_state.h_cost =
            calculate_heuristic(source_words, next_state.covered_mask, lm);
        next_state.f_cost = next_state.g_cost + next_state.h_cost;

        open_set.push(next_state);
      }
    }
  }

  return TranslationResult(); // Return empty result
}
