#include "SMTDecoder.cpp" // Enable single-file compilation
#include "SMTDecoder.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
  cout << "Initializing SMT Decoder..." << endl;

  // 1. Load Model and Dictionary
  Dictionary dict;
  if (!dict.load("data/dict.txt")) 
  {
    cerr << "Error: Could not load data/dict.txt" << endl;
    return 1;
  }

  LanguageModel lm;
  if (!lm.load("data/bigrams.txt")) 
  {
    cerr << "Error: Could not load data/bigrams.txt" << endl;
    return 1;
  }

  // 2. Input Sentence
  string english_sentence = "the black cat ";
  cout << "Source English: " << english_sentence << endl;

  // 3. Word by word translation
  vector<string> bag_of_french_words;

  string current_word = "";
  for (int i = 0; i < (int)english_sentence.length(); i++) 
  {
    char c = english_sentence[i];
    if (c == ' ') 
    {
      if (!current_word.empty()) 
      {
        string french_word = dict.translate(current_word);
        bag_of_french_words.push_back(french_word);
        cout << "  Mapped '" << current_word << "' -> '" << french_word << "'"
             << endl;
        current_word = "";
      }
    } else 
    {
      current_word += c;
    }
  }

  // Push last word
  if (!current_word.empty()) 
  {
    string french_word = dict.translate(current_word);
    bag_of_french_words.push_back(french_word);
    cout << "  Mapped '" << current_word << "' -> '" << french_word << "'"
         << endl;
  }

  // 4. Decode
  cout << "Decoding optimal permutation..." << endl;
  TranslationResult result = solve_permutation(bag_of_french_words, lm);

  // 5. Output Result
  cout << "Translated and Reordered: ";
  for (int i = 0; i < (int)result.words.size(); i++) 
  {
    cout << result.words[i] << " ";
  }
  cout << endl;

  // Output Costs
  cout << "Final Costs:" << endl;
  cout << "  Initial Heuristic (Estimate): " << result.initial_h_cost << endl;
  cout << "  Final G Cost (Actual): " << result.final_g_cost << endl;
  cout << "  Final F Cost (Total): " << result.final_f_cost << endl;

  return 0;
}
