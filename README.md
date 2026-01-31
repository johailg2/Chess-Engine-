# C++ Chess Engine & Python GUI

A high-performance chess engine written in C++ with a modular Python GUI.
This project demonstrates bitboard-based move generation, custom evaluation heuristics, and C++/Python interoperability.

## Features

### Board Representation
* **Magic Bitboards:** Utilizes magic number hashing for constant-time (O(1)) sliding piece attack lookups.
* **Zobrist Hashing:** Implements incremental hash updates for fast position verification and transposition table keys.

### Search Algorithm
* **Principal Variation Search (PVS):** Minimax with Alpha-Beta pruning within an Iterative Deepening framework.
* **Quiescence Search:** Extends the search horizon for capture sequences to ensure positional stability.
* **Transposition Table:** Caches search results to handle move transpositions and improve pruning rates.
* **Move Ordering:**
    * **MVV-LVA:** Prioritizes high-value captures (Most Valuable Victim - Least Valuable Attacker).
    * **Killer Heuristic:** Reuses effective quiet moves from sibling nodes.
    * **History Heuristic:** Tracks historically successful moves for better sorting.

### Evaluation Function
* **Tapered Evaluation:** Interpolates between Middle Game and End Game weights for fluid phase transitions.
* **Piece-Square Tables (PST):** detailed positional scoring for every piece type.
* **Pawn Structure Analysis:** Evaluates isolated, doubled, and passed pawns with rank-dependent bonuses.
* **Mobility & Center Control:** dynamic scoring based on available moves and control of the critical D4/D5/E4/E5 squares.

## Refactoring
I recently refactored the legacy monolithic codebase into a modular design to improve readability and maintainability. 

**Key improvements:**
* **Decoupling:** Separated the GUI logic completely from the engine core. The engine now exposes a clean API via `wrapper.cpp`.
* **Modernization:** Updated the build system (`setup.py`) to support C++17 standards.
* **AI-Assisted Optimization:** Leveraged AI tools to identify platform-specific bottlenecks (like `__builtin_popcountll` on Windows) and implement cross-platform compatibility layers.

## 📦 How to Build
1.  **Install Dependencies:**
    ```bash
    pip install pybind11 pygame
    ```
2.  **Compile the C++ Extension:**
    ```bash
    python setup.py build_ext --inplace
    ```
3.  **Run the GUI:**
    ```bash
    python gui.py
    ```