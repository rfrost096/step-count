# ESP32 Step Counting Algorithm

## Overview

This repository contains a C++ implementation of a continuous step-counting algorithm designed for embedded systems, specifically the ESP32 microcontroller. The algorithm processes raw accelerometer data to detect steps in real-time, filtering noise and validating movements based on frequency and intensity thresholds.

The project includes the core step-counting logic as well as a testing and optimization framework used to tune the algorithm's constants against a provided dataset of labeled accelerometer recordings.

## Features

* **Real-time Processing:** Designed to process data streams continuously with a fixed memory footprint, suitable for embedded environments.
* **Signal Smoothing:** Implements a moving mean average to filter high-frequency sensor noise.
* **Dynamic Peak Detection:** Identifies steps by analyzing signal magnitude relative to local averages.
* **Cadence Validation:** Filters false positives by strictly enforcing realistic walking frequencies (Minimum and Maximum Hertz).
* **Step Buffer:** Utilizes a buffer state machine to prevent "phantom steps" from short, non-walking movements. Steps are only registered after a continuous sequence is detected.
* **Parameter Optimization:** Includes a mode to brute-force iterate through ranges of constants (thresholds, buffer sizes, etc.) to mathematically find the optimal configuration for the highest accuracy.

## Algorithm Logic

The algorithm operates on a state machine model to handle continuous data streams:

1. **Preprocessing:**
* **Magnitude Calculation:** Computes the squared magnitude of the acceleration vector ().
* **Moving Average:** Smooths the magnitude data using a configurable window (`MOVE_MEAN_SIZE`).
* **Differentiation:** Calculates the delta between the current smoothed value and previous values to highlight impacts.


2. **Peak Detection:**
* The system checks if the processed signal exceeds a specific `THRESHOLD`.
* It verifies the point is a local maximum by comparing it to neighbors (`PEAK_AVERAGE`).


3. **Validation (State Machine):**
* **NOT_WALKING:** The system waits for an initial peak.
* **CHECK_PEAK:** Once a peak is found, the system verifies that the *next* peak occurs within a valid time window defined by `MIN_HERTZ` and `MAX_HERTZ`.
* **Step Buffering:** Valid steps are added to a temporary buffer (`STEP_BUFFER`). Only when the buffer fills is the user considered "Walking," and the steps are committed to the total count.



## Configuration & Constants

The algorithm relies on several constants that determine sensitivity. These were optimized using the included dataset:

* `THRESHOLD`: The minimum signal amplitude required to register a step.
* `MOVE_MEAN_SIZE`: The window size for the moving average filter.
* `MIN_HERTZ` / `MAX_HERTZ`: The valid frequency range for human walking (defaulted around 1.9Hz - 3.1Hz).
* `STEP_BUFFER`: The number of consecutive steps required to validate a walking session (default: 10).
* `PEAK_AVERAGE`: The neighbor width used to confirm a local peak.

## Usage

### Modes of Operation

The `main()` function contains boolean flags to control the operation mode:

1. **Optimization Mode (`optimization = true`)**
* Iterates through defined ranges of all algorithm constants.
* Runs the algorithm against all CSV files in `filePathMap`.
* Calculates an accuracy score for every combination.
* Outputs the results to `variables.txt`, sorted by accuracy, allowing you to select the best parameters.


2. **Standard Mode (`optimization = false`)**
* Runs the algorithm on the file list using the current global constants.
* **Data Collection (`dataCollection = true`):** Runs `countStepsContinuousData`, which generates `analyzed.csv`. This file contains the raw processed signal and markers for where peaks were detected, useful for visualization.
* **Simple Run (`dataCollection = false`):** Simply prints the file name, calculated steps, and expected steps to the console.



### Compilation

The code uses standard C++ libraries and can be compiled with `g++`:

```bash
g++ -o step_counter main5.cpp
./step_counter

```

### Input Data

The algorithm expects CSV files where specific columns represent accelerometer axes. The current implementation parses these lines in the `splitString` function. You map files to their expected step counts in the `filePathMap` within `main()`:

```cpp
filePathMap.insert(make_pair("./data/received.csv", 350));

```

## Output Files

* **`variables.txt`:** Generated during optimization; contains parameter sets and their resulting accuracy scores.
* **`analyzed.csv`:** Generated during data collection; contains the processed signal vs. detected peaks for debugging.