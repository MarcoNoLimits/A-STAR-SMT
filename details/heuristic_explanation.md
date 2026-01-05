# The Magic Guessing Game (A* Heuristic Explained)

Imagine you are a detective trying to solve a puzzle. You want to find the **cheapest** way to stick a bunch of words together to make a French sentence.

## The Rule (Admissibility)
To be a perfect detective, you have a strict rule for guessing how much work is left:
**"Never be pessimistic. Always be optimistic."**

If you guess that the rest of the puzzle costs **$100**, but it really only costs **$50**, you might give up on a good path too early. That is bad!
But if you guess it costs **$10**, and it really costs **$50**, that is okay! You will keep exploring and eventually find out the truth.

So, your guess ($h$) must always be **lower** or **equal** to the real cost ($h^*$).

## Our Method: "The Best Coupon Strategy"

Imagine the words left in your bag are **"cat"** and **"eats"**. You have to buy them to put them in the sentence.

1.  **Real Life is Hard**:
    -   The price of a word depends on what came before it.
    -   If "cat" comes after "dog", it might stick badly and cost **$10**.
    -   If "cat" comes after "the", it fits perfectly and costs **$2**.

2.  **Our Optimistic Guess**:
    -   Since we don't know the order yet, we **pretend** correctly guessing the order is easy.
    -   We look at "cat" and say: "What is the absolute cheapest price for 'cat' anywhere?" -> **$2** (after 'the').
    -   We look at "eats" and say: "What is the absolute cheapest price for 'eats' anywhere?" -> **$3** (after 'cat').

3.  **The Total Guess**:
    -   We simply add up the cheapest prices: **$2 + $3 = $5**.

## Why this works (The Proof)
-   **Our Guess ($5)**: Assumes we used the best possible coupon for every single word.
-   **Real Cost**: In the real sentence, we might not be able to use the coupons. Maybe we are forced to put "cat" after "blue" and it costs **$20**.
-   So, **Real Cost** will always be **bigger** (or equal) to our **Guess**.

Because our guess is always "cheaper than reality", we represent the "Perfect Scenario". This guarantees our A* algorithm finds the true best path!
