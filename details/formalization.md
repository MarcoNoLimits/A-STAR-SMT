# SMT Decoder: Mathematical Formalization

This document provides the formal mathematical definition of the Word-Based Statistical Machine Translation (SMT) decoder implemented in this project.

## 1. Problem Definition
Given a source sentence $S$ of $N$ English words, we map each word $e_i$ to a French word $f_i$ using a dictionary. The goal is to find the optimal **permutation** $\pi$ of these French words that minimizes the total cost defined by a Language Model and a Distortion Model.

## 2. Global Objective
The decoder seeks to efficiently find the permutation $\pi^*$ that satisfies:
$$ \pi^* = \arg\min_{\pi} \left( \sum_{i=1}^{N} \text{Cost}(\pi_i | \pi_{i-1}) \right) $$

We solve this using **A* Search**, which traverses a state space DAG (Directed Acyclic Graph).

## 3. State Space
A state $n$ is defined as a tuple:
$$ n = \langle M, L \rangle $$
- $M$: A bitmask of length $N$, where $M_j=1$ if source word $j$ has been placed, and $0$ otherwise.
- $L$: The index of the *last* source word added to the sequence.

### 3.1. Choosing the First Word (Dynamic Start)
The "Starting State" of the search is always the **Empty State** ($\emptyset$). The selection of the *first actual word* is not hardcoded; it is determined dynamically by the A* search during the first expansion.
1.  **Initial Context**: We assume a virtual "Start of Sentence" (`<s>`) token exists.
2.  **Initial Position**: We assume a virtual cursor position at index $-1$.
3.  **The First Expansion**: The algorithm generates $N$ possible branches (one for each source word $i$ as the starter).
    -   $Cost = -\log P(\text{word}_i | \text{<s>}) + |i - (-1) - 1|$
    -   The Priority Queue naturally selects the branch with the lowest combined cost (best probability + lowest distortion).

## 4. Evaluation Function $f(n)$
The A* algorithm orders the search using:
$$ f(n) = g(n) + h(n) $$

### 4.1. Path Cost $g(n)$
$g(n)$ is the accumulated cost to reach state $n$ from the start. It has two components:

1.  **Language Model Cost ($C_{LM}$)**: Measures the "ungrammaticality" of the sequence.
    $$ C_{LM}(w_{curr} | w_{prev}) = -\log P(w_{curr} | w_{prev}) $$
    *(Implementation: We use pre-computed positive costs).*

2.  **Distortion Cost ($C_{D}$)**: Penalizes non-monotonic word ordering.
    $$ C_{D}(i_{curr}, i_{prev}) = | i_{curr} - i_{prev} - 1 | $$
    - If we translate word $i$ immediately after word $i-1$, cost is $|i - (i-1) - 1| = 0$.

**Total $g(n)$ for a path of length $k$:**
$$ g(n) = \sum_{t=1}^{k} \left( C_{LM}(w_t | w_{t-1}) + C_{D}(i_t, i_{t-1}) \right) $$

### 4.2. Heuristic Function $h(n)$
$h(n)$ estimates the cost to place all *remaining* words (where $M_j=0$).

To guarantee optimality, $h(n)$ must be **admissible** ($h(n) \le h^*(n)$). We use the **Sum of Min-Transitions** approximation.

For every remaining word $w \in \text{Unused}$:
$$ h(n) = \sum_{w \in \text{Unused}} \min_{v \in \text{Vocab}} \left( C_{LM}(w|v) \right) $$

**Proof of Admissibility:**
1.  The true future cost $h^*(n)$ contains the sum of **specific** transition costs for the remaining words plus distortion costs.
2.  The heuristic ignores distortion (which is $\ge 0$).
3.  The heuristic assumes every word is preceded by its "best friend" (the word that yields the minimal $C_{LM}$).
4.  Therefore, for every word $w$, the heuristic cost is $\le$ the actual cost.
    $$ \sum \min(C_{LM}) \le \sum \text{Actual}(C_{LM}) + \text{Distortion} $$
    $$ h(n) \le h^*(n) $$

## 5. Summary
The decoder finds the sentence that balances **fluency** (Language Model) and **word order fidelity** (Distortion Model) by exploring the state space using an optimal, optimistic search strategy.
