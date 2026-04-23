# ESP32 Step Counting Algorithm

## Summary for Recruiters
This project features a high-performance **C++ step-counting algorithm** optimized for the **ESP32 microcontroller**. It processes raw accelerometer data in real-time to track physical activity with high precision.

**Key Highlights:**
- **High Accuracy:** Achieves >92% accuracy across diverse datasets.
- **Embedded Optimization:** Designed for low-latency, real-time execution with a minimal memory footprint.
- **Robustness:** Incorporates signal filtering and a validation state machine to eliminate false positives from non-walking movements.
- **Data-Driven Tuning:** Includes an automated optimization framework that brute-forces algorithm constants against labeled sensor data to find the mathematically ideal configuration.

---

## Technical Details for Engineers

### Algorithm Architecture
The core logic is implemented as a continuous signal processing pipeline and a state machine.

1.  **Signal Preprocessing:**
    *   **Magnitude:** Vector magnitude is calculated as $x^2 + y^2 + z^2$ (avoiding expensive square roots where possible).
    *   **Filtering:** A moving average filter (`MOVE_MEAN_SIZE`) smooths high-frequency sensor noise.
    *   **Differentiation:** Highlights impacts by calculating the delta between smoothed values.

2.  **Peak Detection & Validation:**
    *   **Dynamic Thresholding:** Identifies potential steps based on a `THRESHOLD` and local maxima verification (`PEAK_AVERAGE`).
    *   **Cadence Check:** Enforces valid human walking frequencies (1.9Hz - 3.1Hz) by timing the interval between peaks.
    *   **Step Buffering:** Uses a `STEP_BUFFER` (default: 10) to confirm a "walking" state before committing steps, preventing "phantom steps" from brief movements.

### Optimization Framework
The `main5.cpp` includes a brute-force optimizer that:
- Iterates through ranges of `THRESHOLD`, `MIN_HERTZ`, `MAX_HERTZ`, and buffer sizes.
- Evaluates accuracy against a diverse 10-file dataset of labeled movements (handheld, pocket, backpack).
- Outputs ranked configurations to `variables.txt`.

### Modes of Operation
- **Optimization Mode:** Exhaustive search for optimal constants.
- **Standard Mode:** Executes the algorithm with optimized constants.
- **Data Collection Mode:** Generates `analyzed.csv`, exporting the processed signal and peak markers for visualization/debugging.

### Setup & Usage
**Compilation:**
```bash
g++ -o step_counter main5.cpp
./step_counter
```

**Input Format:**
Expects CSV files with accelerometer data. File-to-step-count mappings are defined in the `filePathMap` within `main()`.

**Key Files:**
- `main5.cpp`: Core algorithm and optimization logic.
- `variables.txt`: Optimization results sorted by accuracy.
- `data/`: Raw sensor datasets.
- `analyzed.csv`: Debugging output for signal analysis.
