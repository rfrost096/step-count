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

int countSteps(string filePath);
int countStepsWithDataOutput(map<string, deque<float>> data);
int countStepsContinuous(string filePath);
void splitString(string line, vector<string>& subParts);
float average(float data[], int size);
float average(float data[], int size, int startInterval, int endInterval);
bool isPeak(float localData[], int interval, int size);
int findPeakInterval(float localData[], int startInterval, int endInterval, int size);
int findPeak(float localData[], int i, int size);
void printArray(float arrayToBePrinted[], int size);
void printDeque(deque<float> dequeToBePrinted);
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

float THRESHOLD = 2.1;
int MEMORY_SIZE = 512; // Make this a power of 2
int MOVE_MEAN_SIZE = 11; // Make this a power of 2
float SAMPLE_RATE = 100;
float MIN_HERTZ = 1.42;
float MAX_HERTZ = 2.5;
int STEP_BUFFER = 10;
int PEAK_AVERAGE = 3;
int MIN_DELAY = ((int) (SAMPLE_RATE / MIN_HERTZ)) - ((int) (SAMPLE_RATE / MAX_HERTZ));

int main()
{
	vector<float> thresholdInterval = floatInterval(2.0, 2.2, 0.01);
	vector<int> moveMeanSizeInterval = intInterval(9, 15, 1);
	vector<float> minHertzInterval = floatInterval(1.35, 1.5, 0.01);
	vector<float> maxHertzInterval = floatInterval(2.45, 2.65, 0.01);
	vector<int> stepBufferInterval = intInterval(10, 11, 1);
	vector<int> peakAverageInterval = intInterval(3, 4, 1);
	// Map to all file paths
	// May need to change depending on environment
	// String is file path, int is expected step count
	map<string, int> filePathMap;
	filePathMap.insert(make_pair("./received.csv", 350));
    filePathMap.insert(make_pair("./p1.1_Female_20-29_170-179cm_Hand_held.out.csv", 70));
	filePathMap.insert(make_pair("./p1.4_Female_20-29_170-179cm_Handbag.out.csv", 70));
	filePathMap.insert(make_pair("./p2.2_Male_20-29_180-189cm_Hand_held.out.csv", 66));
	filePathMap.insert(make_pair("./p9.2_Female_15-19_160-169cm_Trousers_back_pocket.out.csv", 70));
	filePathMap.insert(make_pair("./p11.3_Male_20-29_170-179cm_Backpack.out.csv", 76));
	filePathMap.insert(make_pair("./p27.1_Male_15-19_170-179cm_Hand_held.dat.csv", 68));
    filePathMap.insert(make_pair("./100 Steps Josh Data.csv", 100));
    filePathMap.insert(make_pair("./150 Steps walking 2.csv", 150));
	
	float accuracy = 0;
	vector<vector<float>> variables = {};

	int totalTime = 0;
	int averageTime = 0;
	int counter = 0;
	int total = thresholdInterval.size() * moveMeanSizeInterval.size() * minHertzInterval.size() * maxHertzInterval.size() * stepBufferInterval.size() * peakAverageInterval.size();
	auto startTime = chrono::steady_clock::now();
	auto endTime = chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	bool optimization = false;

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

									int steps = countSteps(itr->first);
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

			int steps = countStepsContinuous(itr->first);
			cout << "File Name: " << itr->first << "\n\tStep Count: " << steps << " Expected: " << to_string(itr->second) << "\n\n";
		}
	}
}

int countSteps(string filePath) {

	// Create input file stream
	ifstream in;

	// Open file
	in.open(filePath.c_str());

    // Initialize file I/O variables
    string line;
    vector<string> subParts;

    // Initialize algorithm variables
    float moveMeanData[MOVE_MEAN_SIZE];
    float diff[2];
    float localData[MEMORY_SIZE];
    int i = 0;
	int stepCount = 0;
	int stepBuffer = 0;
	int delay = MEMORY_SIZE + MOVE_MEAN_SIZE;
	bool walking = false;
	
	// Loop while data is available
	while (in.good())
	{
		in >> line;

        stringstream ssComma(line);

		// Add each column element to the vector (subParts[0] = column 1)
		while (ssComma.good()) {
			string substring;
			getline(ssComma, substring, ',');
			subParts.push_back(substring);
		}

		// Convert string to float values
        // time values are stored in column 0, x values are store in column 1, 
        // y values are stored in column 2, z values are stored in column 3
        // Imitate reading from the accelerometer address
        // Store the last "MOVE_MEAN_SIZE" elements

        if (subParts.size() >= 4) {

            moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);
            subParts.clear();

            // Store two elements to take the difference
            diff[0] = diff[1];
            diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

            // Take the square of the difference
            // Store data 1 interval back so that interval "i" is the oldest piece of data
            localData[(i - 1) % MEMORY_SIZE] = diff[1] - diff[0];

            if (delay == 0) {
                if (!walking) {
                    // Check if the value is greater than the threshold
                    if (localData[i] > THRESHOLD || stepBuffer > 0) {
                        // Once the threshold is reached, check for the maximum peak within a window
                        // The window is between half of the max hertz and the minimum hertz
                        int peakInterval = findPeak(localData, i, MEMORY_SIZE);
                        
                        if (peakInterval != -1) {
                            // If a peak is found
                            // Find the difference in the current interval and the peak interval
                            if (peakInterval < i) {
                                delay = peakInterval + MEMORY_SIZE - i;
                            } else {
                                delay = peakInterval - i;
                            }
                            // Add one step to the step buffer
                            stepBuffer++;

                            // The "WALKING" state should be entered AFTER the previous peak
                            // The delay variable waits until the peak is reached
                        } else if (stepBuffer > 0) {
                            // If no peak is found, reduce the step buffer
                            stepBuffer--;
                        } else {
                            // Set a minimum delay so that a new peak isn't searched for every time there is a value greater than the threshold
                            delay = MIN_DELAY;
                        }

                        if (stepBuffer == STEP_BUFFER) {
                            stepCount += STEP_BUFFER;
                            walking = true;
                        }
                    }
                } else {	
                    // A peak was just detected, the current interval is at that peak
                    int peakInterval = findPeak(localData, i, MEMORY_SIZE);

                    if (peakInterval != -1) {
                        // Find the difference in the current interval and the peak interval
                        if (peakInterval < i) {
                            delay = peakInterval + MEMORY_SIZE - i;
                        } else {
                            delay = peakInterval - i;
                        }
                        stepCount++;
                    } else if (stepBuffer > 0) {
                        stepBuffer--;
                    } else {
                        walking = false;
                    }
                }
            } else {
                delay--;
            }
            i = (i + 1) % MEMORY_SIZE;
        }
    }

	return stepCount;
}

int countStepsWithDataOutput(string filePath) {

    // Create input file stream
	ifstream in;

	// Open file
	in.open(filePath.c_str());

    // Initialize file I/O variables
    string line;
    vector<string> subParts;
    stringstream ssComma(line);

    float moveMeanData[MOVE_MEAN_SIZE];
    float diff[2];
    float localData[MEMORY_SIZE];
    int i = 0;
	int stepCount = 0;
	int stepBuffer = 0;
	int delay = MEMORY_SIZE + MOVE_MEAN_SIZE;
	bool walking = false;
	bool startCollecting = false;
	float peakBuffer = 0.0;
	float peak = 0.0;
	ofstream outputFile("data.csv");
	outputFile << "x,y,z,avg,diff,localdata\n";

	// Loop while data is available
	while (in.good())
	{
		in >> line;

		// Add each column element to the vector (subParts[0] = column 1)
		while (ssComma.good()) {
			string substring;
			getline(ssComma, substring, ',');
			subParts.push_back(substring);
		}

		float x = atof(subParts[1].c_str());
		float y = atof(subParts[2].c_str());
		float z = atof(subParts[3].c_str());
        // Imitate reading from the accelerometer address
        // Store the last "MOVE_MEAN_SIZE" elements
        //moveMeanData[i % MOVE_MEAN_SIZE] = pow(readByte(data, "x"), 2) + pow(readByte(data, "y"), 2) + pow(readByte(data, "z"), 2);
		moveMeanData[i % MOVE_MEAN_SIZE] = pow(x, 2) + pow(y, 2) + pow(z, 2);

		float avg = average(moveMeanData, MOVE_MEAN_SIZE);

        // Store two elements to take the difference
        diff[0] = diff[1];
        //diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);
		diff[1] = avg;

        // Take the square of the difference
		// Store data 1 interval back so that interval "i" is the oldest piece of data
		float diffVal = diff[1] - diff[0];
        localData[(i - 1) % MEMORY_SIZE] = diff[1] - diff[0];

		if (delay == 0) {
			if (!walking) {
				// Check if the value is greater than the threshold
				if (localData[i] > THRESHOLD || stepBuffer > 0) {
					// Once the threshold is reached, check for the maximum peak within a window
					// The window is between half of the max hertz and the minimum hertz
					int peakInterval = findPeak(localData, i, MEMORY_SIZE);
					peak = peakBuffer;
					
					if (peakInterval != -1) {
						// If a peak is found
						// Find the difference in the current interval and the peak interval
						if (peakInterval < i) {
							delay = peakInterval + MEMORY_SIZE - i;
						} else {
							delay = peakInterval - i;
						}
						peakBuffer = localData[peakInterval];
						// Add one step to the step buffer
						stepBuffer++;

						// The "WALKING" state should be entered AFTER the previous peak
						// The delay variable waits until the peak is reached
					} else if (stepBuffer > 0) {
						peakBuffer = 0.0;
						// If no peak is found, reduce the step buffer
						stepBuffer--;
					} else {
						peakBuffer = 0.0;
						// Set a minimum delay so that a new peak isn't searched for every time there is a value greater than the threshold
						delay = MIN_DELAY;
					}

					if (stepBuffer == STEP_BUFFER) {
						stepCount += STEP_BUFFER;
						walking = true;
					}
				}
			} else {	
				// A peak was just detected, the current interval is at that peak
				int peakInterval = findPeak(localData, i, MEMORY_SIZE);
				peak = peakBuffer;

				if (peakInterval != -1) {
					// Find the difference in the current interval and the peak interval
					if (peakInterval < i) {
						delay = peakInterval + MEMORY_SIZE - i;
					} else {
						delay = peakInterval - i;
					}
					peakBuffer = localData[peakInterval];
					stepCount++;
				} else if (stepBuffer > 0) {
					peakBuffer = 0.0;
					stepBuffer--;
				} else {
					peakBuffer = 0.0;
					walking = false;
				}
			}
			startCollecting = true;
		} else {
			delay--;
		}
        i = (i + 1) % MEMORY_SIZE;
		if (startCollecting) {
			outputFile << x << "," << y << "," << z << "," << avg << "," << diffVal << "," << localData[i] << "," << peak << "\n";
			peak = 0.0;
		}
    }
	outputFile.close();

	return stepCount;
}

int countStepsContinuous(string filePath) {

	// Create input file stream
	ifstream in;

	// Open file
	in.open(filePath.c_str());

    // Initialize file I/O variables
    string line;
    vector<string> subParts;

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
	int initCounter = MEMORY_SIZE + MOVE_MEAN_SIZE;
	bool walking = false;
    enum State {INIT, STEP_COUNT};
    State currentState = INIT;
    stringstream ssComma;
	
	// Loop while data is available
    while(in.good()) {
        switch(currentState) {
            case INIT:

                in >> line;

                splitString(line, subParts);

                // Convert string to float values
                // time values are stored in column 0, x values are store in column 1, 
                // y values are stored in column 2, z values are stored in column 3
                // Imitate reading from the accelerometer address
                // Store the last "MOVE_MEAN_SIZE" elements

                if (subParts.size() >= 4) {

                    moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);
                    subParts.clear();

                    // Store two elements to take the difference
                    diff[0] = diff[1];
                    diff[1] = average(moveMeanData, MOVE_MEAN_SIZE);

                    // Take the square of the difference
                    // Store data PEAK_AVERAGE back, used in the isPeak() function to use some previous data
                    localData[(i - PEAK_AVERAGE - 1) % MEMORY_SIZE] = diff[1] - diff[0];
                }

                initCounter--;

                if (initCounter <= 0) {
                    currentState = STEP_COUNT;
                }
                break;


            case STEP_COUNT:
                in >> line;

                splitString(line, subParts);

                // Convert string to float values
                // time values are stored in column 0, x values are store in column 1, 
                // y values are stored in column 2, z values are stored in column 3
                // Imitate reading from the accelerometer address
                // Store the last "MOVE_MEAN_SIZE" elements

                if (subParts.size() >= 4) {

                    moveMeanData[i % MOVE_MEAN_SIZE] = pow(atof(subParts[1].c_str()), 2) + pow(atof(subParts[2].c_str()), 2) + pow(atof(subParts[3].c_str()), 2);
                    subParts.clear();

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
                }
                break;
        }
    }

	return stepCount;
}

float readByte(map<string, deque<float>>& data, string dimension) {
	data[dimension].pop_front();
    float value = data[dimension].front();
    return value;
}

void splitString(string line, vector<string>& subParts) {
    stringstream ssComma(line);

    // Add each column element to the vector (subParts[0] = column 1)
    while (ssComma.good()) {
        string substring;
        getline(ssComma, substring, ',');
        subParts.push_back(substring);
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

int findPeakInterval(float localData[], int startInterval, int endInterval, int size) {
	int peakInterval = -1;
	float peakValue = 0.0;
	for (int i = startInterval; i != endInterval; i = (i + 1) % size) {
		if (isPeak(localData, i, size) && localData[i] > peakValue) {
			peakValue = localData[i];
			peakInterval = i;
		}
	}
	return peakInterval;
}

int findPeak(float localData[], int i, int size) {
	return findPeakInterval(localData, (i + ((int) (SAMPLE_RATE / MAX_HERTZ))) % size, (i + ((int) (SAMPLE_RATE / MIN_HERTZ))) % size, size);
}

void printArray(float arrayToBePrinted[], int size) {
	for (int i = 0; i < size; i++) {
		cout << "Interval: " << i << " Value: " << arrayToBePrinted[i] << "\n";
	}
}

void printDeque(deque<float> dequeToBePrinted) {
	for (int i = 0; i < 20; i++) {
		cout << "Interval: " << i << " Value: " << dequeToBePrinted[i] << "\n";
	}
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
    for (int i = 0; i < vectorToWrite.size(); i++) {
		for (int j = 0; j < vectorToWrite[i].size(); j++) {
        	outputFile << vectorToWrite[i][j] << " ";
		}
		outputFile << "\n";
    }
    outputFile.close();
}