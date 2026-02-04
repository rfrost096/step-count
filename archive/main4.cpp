#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>
#include <deque>
#include <cstring>
#include <cstdint>
#include <chrono>

using namespace std;

int countStepsContinuous(string filePath);
int countStepsContinuousData(string filePath);
void splitString(string line, string* subParts, int size);
float average(float data[], int size);
float average(float data[], int size, int startInterval, int endInterval);
bool isPeak(float localData[], int interval, int size);
vector<float> floatInterval(float start, float end, float interval);
vector<int> intInterval(int start, int end, int interval);
array<float, 2> floatMinMax(map<float, float> floatMap, float accuracy);
array<int, 2> intMinMax(map<int, float> floatMap, float accuracy);
void writeVectorToFile(const vector<vector<float>>& vectorToWrite, const string& filename);


// const float THRESHOLD = 2;
// const int MEMORY_SIZE = 512; // Make this a power of 2
// const int MOVE_MEAN_SIZE = 16; // Make this a power of 2
// const float SAMPLE_RATE = 100;
// const float MIN_HERTZ = 1.3;
// const float MAX_HERTZ = 2.7;
// const int STEP_BUFFER = 10;
// const int PEAK_AVERAGE = 3;
// const int MIN_DELAY = 10;

float THRESHOLD = 1.1;
int MEMORY_SIZE = 512; // Make this a power of 2
int MOVE_MEAN_SIZE = 32; // Make this a power of 2
float SAMPLE_RATE = 100;
float MIN_HERTZ = 1.4;
float MAX_HERTZ = 2.7;
int STEP_BUFFER = 10;
int PEAK_AVERAGE = 3;
int MIN_DELAY = ((int) (SAMPLE_RATE / MIN_HERTZ)) - ((int) (SAMPLE_RATE / MAX_HERTZ));

int main()
{
	vector<float> thresholdInterval = floatInterval(1.0, 2.5, 0.01);
	vector<int> moveMeanSizeInterval = intInterval(32, 33, 1);
	vector<float> minHertzInterval = floatInterval(1, 1.8, 0.02);
	vector<float> maxHertzInterval = floatInterval(2.2, 3, 0.02);
	vector<int> stepBufferInterval = intInterval(10, 11, 1);
	vector<int> peakAverageInterval = intInterval(3, 4, 1);
	// Map to all file paths
	// May need to change depending on environment
	// String is file path, int is expected step count
	map<string, int> filePathMap;
	filePathMap.insert(make_pair("./p1.1_Female_20-29_170-179cm_Hand_held.out.csv", 70));
	filePathMap.insert(make_pair("./p1.4_Female_20-29_170-179cm_Handbag.out.csv", 70));
	filePathMap.insert(make_pair("./p2.2_Male_20-29_180-189cm_Hand_held.out.csv", 66));
	filePathMap.insert(make_pair("./p9.2_Female_15-19_160-169cm_Trousers_back_pocket.out.csv", 70));
	filePathMap.insert(make_pair("./p11.3_Male_20-29_170-179cm_Backpack.out.csv", 76));
	filePathMap.insert(make_pair("./p27.1_Male_15-19_170-179cm_Hand_held.dat.csv", 68));
	
	float accuracy = 0;
	vector<vector<float>> variables = {};

	int totalTime = 0;
	int averageTime = 0;
	int counter = 0;
	int total = thresholdInterval.size() * moveMeanSizeInterval.size() * minHertzInterval.size() * maxHertzInterval.size() * stepBufferInterval.size() * peakAverageInterval.size();
	auto startTime = chrono::steady_clock::now();
	auto endTime = chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	bool optimization = true;

	if (optimization) {
		for (float thresholdBuffer : thresholdInterval) {
			for (int moveMeanSizeBuffer : moveMeanSizeInterval) {
				for (float minHertzBuffer : minHertzInterval) {
					for (float maxHertzBuffer : maxHertzInterval) {
						for (int stepBufferBuffer : stepBufferInterval) {
							for (int peakAverageBuffer : peakAverageInterval) {
								startTime = chrono::steady_clock::now();
								THRESHOLD = thresholdBuffer;
								MOVE_MEAN_SIZE = moveMeanSizeBuffer;
								MIN_HERTZ = minHertzBuffer;
								MAX_HERTZ = maxHertzBuffer;
								STEP_BUFFER = stepBufferBuffer;
								PEAK_AVERAGE = peakAverageBuffer;
								float accuracyBuffer = 0;
								for (auto itr = filePathMap.begin(); itr != filePathMap.end(); itr++) {

									int steps = countStepsContinuous(itr->first);
									//cout << "File Name: " << itr->first << "\n\tStep Count: " << steps << " Expected: " << to_string(itr->second) << "\n\n";
									accuracyBuffer += 1 - abs(steps - itr->second) / static_cast<float>(itr->second);
								}
								accuracyBuffer = accuracyBuffer / filePathMap.size();

								if (accuracyBuffer >= accuracy) {
									accuracy = accuracyBuffer;
									vector<float> buffer{accuracyBuffer, thresholdBuffer, (float) moveMeanSizeBuffer, minHertzBuffer, maxHertzBuffer, (float) stepBufferBuffer, (float)peakAverageBuffer};
									variables.push_back(buffer);
								}
								counter++;
								if (counter % 64 == 0) {
									averageTime = totalTime / counter;
									int timeLeft = averageTime * (total - counter);
									float hours = ((float) timeLeft) / 3600000;
									cout << "Percent Done: " << ((float) counter / (float) total) * 100 << " Time Left (hours): " << hours << "\n";
									
								}
								totalTime += duration.count();
								endTime = chrono::steady_clock::now();
								duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
							}
						}
					}
				}
			}
		}

		writeVectorToFile(variables, "variables.txt");

	} else {
		for (auto itr = filePathMap.begin(); itr != filePathMap.end(); itr++) {

			int steps = countStepsContinuousData(itr->first);
			cout << "File Name: " << itr->first << "\n\tStep Count: " << steps << " Expected: " << to_string(itr->second) << "\n\n";
		}
	}
}

int countStepsContinuous(string filePath) {

	// Create input file stream
	ifstream in;
    string line;
    int columns = 1;

	// Open file
	in.open(filePath.c_str());
    if (!(in.good())) {
        return 0;
    } else {
        in >> line;
        for (char c : line) {
            if (c == ',') {
                columns++;
            }
        }

    }

    // Initialize file I/O variables
    
    string subParts[columns];

    // Initialize algorithm variables
    float moveMeanData[MOVE_MEAN_SIZE];
    float diff[2];
    float localData[MEMORY_SIZE];
    int i = 0;
	int stepCount = 0;
	int stepBuffer = 0;
    float tempPeakValue = 0.0;
    int prevPeakCounter = 0;
    int peakCounter = 0;
    bool peakBool = false;
    bool tempPeakBool = false;
    int intervalCounter = 0;
    int intervalMin = ((int) SAMPLE_RATE / MAX_HERTZ);
    int intervalMax = ((int) SAMPLE_RATE / MIN_HERTZ);
    int intervalSize = intervalMax - intervalMin;
	int initCounter = MOVE_MEAN_SIZE + MEMORY_SIZE + PEAK_AVERAGE;
	bool walking = false;
    enum State {INIT, STEP_COUNT};
    State currentState = INIT;
	
	// Loop while data is available
    while(in.good()) {
        switch(currentState) {
            case INIT:

                in >> line;

                splitString(line, subParts, columns);

                moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);

                // Store two elements to take the difference
                diff[0] = diff[1];
                diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

                // Take the square of the difference
                // Store data PEAK_AVERAGE back, used in the isPeak() function to use some previous data
                localData[(i - PEAK_AVERAGE - 1) % MEMORY_SIZE] = diff[1] - diff[0];

                initCounter--;

                if (initCounter <= 0) {
                    currentState = STEP_COUNT;
                }
                i = (i + 1) % MEMORY_SIZE;
                break;


            case STEP_COUNT:
                in >> line;

                splitString(line, subParts, columns);

                // Convert string to float values
                // time values are stored in column 0, x values are store in column 1, 
                // y values are stored in column 2, z values are stored in column 3
                // Imitate reading from the accelerometer address
                // Store the last "MOVE_MEAN_SIZE" elements

                moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);

                // Store two elements to take the difference
                diff[0] = diff[1];
                diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

                // Take the square of the difference
                // Store data PEAK_AVERAGE back, used in the isPeak() function to use some previous data
                localData[(i - PEAK_AVERAGE - 1) % MEMORY_SIZE] = diff[1] - diff[0];

                // If we haven't found a peak yet (no movement)
                if (!peakBool) {
                    // Find the first peak
                    if (!tempPeakBool && isPeak(localData, i, MEMORY_SIZE)) {
                        // Once the peak is found, save the peak value and start an interval
                        // The interval is started with tempPeakBool, and it is the distance between MAX_HERTZ and MIN_HERTZ

                        tempPeakValue = localData[i];
                        tempPeakBool = true;
                    } else if (tempPeakBool) {

                        // If we've reached the end of the interval, save the distance traveled since the highest peak across the interval
                        // This is saved in peakCounter, and it is used later to search the next possible interval for peaks
                        if (intervalCounter >= intervalSize) {
                            peakCounter = prevPeakCounter;

                            // Reset values for future use
                            tempPeakValue = 0.0;
                            intervalCounter = 0;
                            prevPeakCounter = 0;
                            tempPeakBool = false;

                            // The peakBool variable is used to say that a peak has been found, 
                            // and a new interval should be searched based on that peak
                            peakBool = true;
                            stepBuffer++;
                        } else if (localData[i] > tempPeakValue && isPeak(localData, i, MEMORY_SIZE)) {
                            // While searching the interval, if a new higher peak is found, save that value and reset the counter

                            tempPeakValue = localData[i];
                            prevPeakCounter = 0;
                            intervalCounter++;
                        } else {

                            // Increase counters when the current data point is not a peak
                            prevPeakCounter++;
                            intervalCounter++;
                        }
                    }
                } else if (peakCounter >= intervalMin) {
                    // Count until within the next interval between MAX_HERTZ and MIN_HERTZ

                    if (peakCounter >= intervalMax) {
                        // At the end of the interval, check if a peak was found

                        if (tempPeakValue > 0.0) {
                            // A peak is found, increment the stepBuffer or stepCounter

                            peakCounter = prevPeakCounter;
                            prevPeakCounter = 0;
                            tempPeakValue = 0.0;
                            if  (walking) {
                                stepCount++;
                            } else if (stepBuffer == STEP_BUFFER) {
                                stepCount += STEP_BUFFER;
                                walking = true;
                            } else {
                                stepBuffer++;
                            }
                        } else if (stepBuffer > 0) {
                            // No peak is found, decrement the step buffer if it is not zero
                            // Pretend like a step was found in the middle of the last interval

                            prevPeakCounter = 0;
                            peakCounter = intervalSize / 2;
                            stepBuffer--;
                        } else {
                            // No peak is found, stop walking if the step buffer is zero

                            walking = false;
                            peakBool = false;
                        }
                    } else if (localData[i] > tempPeakValue && isPeak(localData, i, MEMORY_SIZE)) {
                        tempPeakValue = localData[i];
                        prevPeakCounter = 0;
                        peakCounter++;
                    } else {
                        peakCounter++;
                        prevPeakCounter++;
                    }
                } else {
                    peakCounter++;
                }
                i = (i + 1) % MEMORY_SIZE;
                break;
        }
    }

	return stepCount;
}

int countStepsContinuousData(string filePath) {

	// Create input file stream
	ifstream in;
    string line;
    int columns = 1;

	// Open file
	in.open(filePath.c_str());
    if (!(in.good())) {
        return 0;
    } else {
        in >> line;
        for (char c : line) {
            if (c == ',') {
                columns++;
            }
        }

    }

    // Initialize file I/O variables
    
    string subParts[columns];

    // Initialize algorithm variables
    float moveMeanData[MOVE_MEAN_SIZE];
    float diff[2];
    float localData[MEMORY_SIZE];
    int i = 0;
	int stepCount = 0;
	int stepBuffer = 0;
    float tempPeakValue = 0.0;
    int prevPeakCounter = 0;
    int peakCounter = 0;
    bool peakBool = false;
    bool tempPeakBool = false;
    int intervalCounter = 0;
    int intervalMin = ((int) SAMPLE_RATE / MAX_HERTZ);
    int intervalMax = ((int) SAMPLE_RATE / MIN_HERTZ);
    int intervalSize = intervalMax - intervalMin;
	int initCounter = MOVE_MEAN_SIZE + MEMORY_SIZE + PEAK_AVERAGE;
	bool walking = false;
    enum State {INIT, STEP_COUNT};
    State currentState = INIT;

    vector<vector<float>> dataRows;
    vector<vector<float>> actualPeaks;
    vector<vector<float>> tempPeaks;
    int counter = 0;
    int tempCounter = 0;
	
	// Loop while data is available
    while(in.good()) {
        switch(currentState) {
            case INIT:

                in >> line;

                splitString(line, subParts, columns);

                moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);

                // Store two elements to take the difference
                diff[0] = diff[1];
                diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

                // Take the square of the difference
                // Store data PEAK_AVERAGE back, used in the isPeak() function to use some previous data
                localData[(i - PEAK_AVERAGE - 1) % MEMORY_SIZE] = diff[1] - diff[0];

                initCounter--;

                if (initCounter <= 0) {
                    currentState = STEP_COUNT;
                }
                i = (i + 1) % MEMORY_SIZE;
                break;


            case STEP_COUNT:
                in >> line;

                splitString(line, subParts, columns);

                // Convert string to float values
                // time values are stored in column 0, x values are store in column 1, 
                // y values are stored in column 2, z values are stored in column 3
                // Imitate reading from the accelerometer address
                // Store the last "MOVE_MEAN_SIZE" elements

                moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);

                // Store two elements to take the difference
                diff[0] = diff[1];
                diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

                // Take the square of the difference
                // Store data PEAK_AVERAGE back, used in the isPeak() function to use some previous data
                localData[(i - PEAK_AVERAGE - 1) % MEMORY_SIZE] = diff[1] - diff[0];
                vector<float> row = {(float) counter, diff[1] - diff[0]};
                dataRows.push_back(row);

                // If we haven't found a peak yet (no movement)
                if (!peakBool) {
                    // Find the first peak
                    if (!tempPeakBool && isPeak(localData, i, MEMORY_SIZE)) {
                        // Once the peak is found, save the peak value and start an interval
                        // The interval is started with tempPeakBool, and it is the distance between MAX_HERTZ and MIN_HERTZ
                        tempCounter = counter;
                        tempPeakValue = localData[i];
                        tempPeakBool = true;
                    } else if (tempPeakBool) {

                        // If we've reached the end of the interval, save the distance traveled since the highest peak across the interval
                        // This is saved in peakCounter, and it is used later to search the next possible interval for peaks
                        if (intervalCounter >= intervalSize) {
                            peakCounter = prevPeakCounter;
                            vector<float> peakRow = {(float)tempCounter, tempPeakValue};
                            tempPeaks.push_back(peakRow);

                            // Reset values for future use
                            tempPeakValue = 0.0;
                            intervalCounter = 0;
                            prevPeakCounter = 0;
                            tempPeakBool = false;

                            // The peakBool variable is used to say that a peak has been found, 
                            // and a new interval should be searched based on that peak
                            peakBool = true;
                            stepBuffer++;
                        } else if (localData[i] > tempPeakValue && isPeak(localData, i, MEMORY_SIZE)) {
                            // While searching the interval, if a new higher peak is found, save that value and reset the counter
                            tempCounter = counter;
                            tempPeakValue = localData[i];
                            prevPeakCounter = 0;
                            intervalCounter++;
                        } else {

                            // Increase counters when the current data point is not a peak
                            prevPeakCounter++;
                            intervalCounter++;
                        }
                    }
                } else if (peakCounter >= intervalMin) {
                    // Count until within the next interval between MAX_HERTZ and MIN_HERTZ

                    if (peakCounter >= intervalMax) {
                        // At the end of the interval, check if a peak was found

                        if (tempPeakValue > 0.0) {
                            // A peak is found, increment the stepBuffer or stepCounter

                            peakCounter = prevPeakCounter;
                            prevPeakCounter = 0;
                            if  (walking) {
                                vector<float> peakRow = {(float)tempCounter, tempPeakValue};
                                actualPeaks.push_back(peakRow);
                                stepCount++;
                            } else if (stepBuffer == STEP_BUFFER) {
                                for (vector<float> tempRow : tempPeaks) {
                                    actualPeaks.push_back(tempRow);
                                }
                                tempPeaks.clear();
                                stepCount += STEP_BUFFER;
                                walking = true;
                            } else {
                                vector<float> peakRow = {(float)tempCounter, tempPeakValue};
                                tempPeaks.push_back(peakRow);
                                stepBuffer++;
                            }
                            tempPeakValue = 0.0;
                        } else if (stepBuffer > 0) {
                            // No peak is found, decrement the step buffer if it is not zero
                            // Pretend like a step was found in the middle of the last interval

                            prevPeakCounter = 0;
                            peakCounter = intervalSize / 2;
                            stepBuffer--;
                        } else {
                            // No peak is found, stop walking if the step buffer is zero

                            tempPeaks.clear();

                            walking = false;
                            peakBool = false;
                        }
                    } else if (localData[i] > tempPeakValue && isPeak(localData, i, MEMORY_SIZE)) {
                        tempCounter = counter;
                        tempPeakValue = localData[i];
                        prevPeakCounter = 0;
                        peakCounter++;
                    } else {
                        peakCounter++;
                        prevPeakCounter++;
                    }
                } else {
                    peakCounter++;
                }
                i = (i + 1) % MEMORY_SIZE;
                break;
        }
        counter++;
    }

    for (vector<float>& tempDataRow : dataRows) {
        for (vector<float>& tempPeakRow : actualPeaks) {
            if (tempDataRow[1] == tempPeakRow[1]) {
                tempDataRow.push_back(tempPeakRow[1]);
            }
        }
    }

    writeVectorToFile(dataRows, "test2.csv");

	return stepCount;
}



void splitString(string line, string* subParts, int size) {
    // Convert string to float values
    // time values are stored in column 0, x values are store in column 1, 
    // y values are stored in column 2, z values are stored in column 3
    // Imitate reading from the accelerometer address
    // Store the last "MOVE_MEAN_SIZE" elements
    stringstream ssComma(line);

    // Add each column element to the vector (subParts[0] = column 1)
    int i = 0;
    while (ssComma.good() && i < size) {
        string substring;
        getline(ssComma, substring, ',');
        subParts[i] = substring;
        i++;
    }
}

float average(float data[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

float average(float data[], int size, int startInterval, int endInterval) {
    float sum = 0;
    for (int i = startInterval; i != endInterval; i = (i + 1) % size) {
        sum += data[i];
    }
    return sum / size;
}

bool isPeak(float localData[], int interval, int size) {
	return localData[interval] > THRESHOLD && 
		localData[interval] > average(localData, size, (interval - PEAK_AVERAGE) % size, interval) &&
		localData[interval] > average(localData, size, interval, (interval + PEAK_AVERAGE) % size);
}

vector<float> floatInterval(float start, float end, float interval) {
	vector<float> intervals;
	for (float i = start; i < end; i += interval) {
		intervals.push_back(i);
	}
	return intervals;
}

vector<int> intInterval(int start, int end, int interval) {
	vector<int> intervals;
	for (int i = start; i < end; i += interval) {
		intervals.push_back(i);
	}
	return intervals;
}

array<float, 2> floatMinMax(map<float, float> floatMap, float accuracy) {
	float min;
	float max;

	bool stored = false;

	for (auto itr = floatMap.begin(); itr != floatMap.end(); itr++) {
		if (itr->second == accuracy) {
			if (stored) {
				if (itr->first < min) {
					min = itr->first;
				}
				if (itr->first > max) {
					max = itr->first;
				}
			} else {
				min = itr->first;
				max = itr->first;
				stored = true;
			}
		}
	}

	return {min, max};
}

array<int, 2> intMinMax(map<int, float> floatMap, float accuracy) {
	int min;
	int max;

	bool stored = false;

	for (auto itr = floatMap.begin(); itr != floatMap.end(); itr++) {
		if (itr->second == accuracy) {
			if (stored) {
				if (itr->first < min) {
					min = itr->first;
				}
				if (itr->first > max) {
					max = itr->first;
				}
			} else {
				min = itr->first;
				max = itr->first;
				stored = true;
			}
		}
	}

	return {min, max};
}

void writeVectorToFile(const vector<vector<float>>& vectorToWrite, const string& filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
	outputFile << "Accuracy, Threshold, MoveMeanSize, MinHertz, MaxHertz, StepBuffer, PeakAverage\n";
    for (vector<float> tempVector : vectorToWrite) {
		for (float tempFloat : tempVector) {
        	outputFile << tempFloat << ",";
		}
		outputFile << "\n";
    }
    outputFile.close();
}