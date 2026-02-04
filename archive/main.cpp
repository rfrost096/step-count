#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>

using namespace std;

void printVector(vector<int> vectorToPrint);
map<string, vector<float>> getData(string filePath);
vector<float> getDiff(vector<float> magnitude);
vector<float> getSquare(vector<float> diff);
vector<float> getMovingAverage(vector<float> square, int moveLength);
bool checkMax(vector<float> squareAverage, int j);
void writeVectorToFile(const vector<float>& vectorToWrite, const string& filename);
int walkingIntervals(vector<float> squareAverage, vector<float> time);

const int MAGNITUDE_AVERAGE_INTERVAL = 30; //Moving average interval
const int DIFF_AVERAGE_INTERVAL = 10; // Moving average interval
const int SQUARE_AVERAGE_INTERVAL = 5; // Moving average interval
const float MIN_HERTZ = 0.8;	// The minimum frequency for a consecutive step
const float MAX_HERTZ = 3;	// The maximum frequency for a consecutive step
const float THRESHOLD = 0.01; // The minimum threshold for signals to be counted
const int STARTING_HALF_STEPS = 3; // Number of consecutive steps to establish the start of walking
const int ENDING_HALF_STEPS = 2; // Number of missing steps to establish the end of walking
const int LOCAL_MAXIMUM = 4; // The range that a number must be greater than the numbers around to be considered a maximum
const int SAMPLE_RATE = 100; // The sample rate in hertz

int main()
{
	// Map to all file paths
	// May need to change depending on environment
	// String is file path, int is expected step count
	map<string, int> filePathMap;
	filePathMap.insert(make_pair("./p1.1_Female_20-29_170-179cm_Hand_held.out.csv", 70));
	// filePathMap.insert(make_pair("./p1.4_Female_20-29_170-179cm_Handbag.out.csv", 70));
	// filePathMap.insert(make_pair("./p2.2_Male_20-29_180-189cm_Hand_held.out.csv", 66));
	// filePathMap.insert(make_pair("./p9.2_Female_15-19_160-169cm_Trousers_back_pocket.out.csv", 70));
	// filePathMap.insert(make_pair("./p11.3_Male_20-29_170-179cm_Backpack.out.csv", 76));
	// filePathMap.insert(make_pair("./p27.1_Male_15-19_170-179cm_Hand_held.dat.csv", 68));
	
	// Iterate over all file paths
	for (auto itr = filePathMap.begin(); itr != filePathMap.end(); itr++) {

		// Get magnitude and time data from .csv file
		map<string, vector<float>> data = getData(itr->first);

		// Store magnitude data
		vector<float> magnitude = data["magnitude"];

		// Get moving average of magnitude data
		vector<float> magnitudeAverage = getMovingAverage(magnitude, MAGNITUDE_AVERAGE_INTERVAL);

		// Get diff of magnitude data
		vector<float> diff = getDiff(magnitudeAverage);

		// Get moving average of diff data
		vector<float> diffAverage = getMovingAverage(diff, DIFF_AVERAGE_INTERVAL);

		// Get square of diff data
		vector<float> square = getSquare(diffAverage);

		// Get moving average of square data
		vector<float> squareAverage = getMovingAverage(square, SQUARE_AVERAGE_INTERVAL);

		// Count steps based on walking intervals
		int stepCountIntervals = walkingIntervals(squareAverage, data["time"]);

		// Print calculated step count with expected
		cout << "File Name: " << itr->first << "\n\tStep Count: " << to_string(stepCountIntervals) << " Expected: " << to_string(itr->second) << "\n\n";

		writeVectorToFile(squareAverage, itr->first + ".txt");
	}
}

map<string, vector<float>> getData(string filePath) {
	map<string, vector<float>> vectorMap; // Map of magnitude and time data
	vector<float> magnitude; // Magnitude data
	vector<float> time; // Time Data
	int i = 0; // Iterating value to line up time data with analyzed magnitude data

	// Create input file stream
	ifstream in;

	// Open file
	in.open(filePath.c_str());
	
	// Loop while data is available
	while (in.good())
	{
		// Read a line
		string line = "";
		in >> line;

		// Initialize vector for storing line parts
		vector<string> subParts;

		// Initialize strinstream to parse each comma
		stringstream ssComma(line);

		// Add each column element to the vector (subParts[0] = column 1)
		while (ssComma.good()) {
			string substring;
			getline(ssComma, substring, ',');
			subParts.push_back(substring);
		}

		// Convert string to float values
		float t = 0.0;
		float x = 0.0;
		float y = 0.0;
		float z = 0.0;
		if (subParts.size() >= 4) {
			t = atof(subParts[0].c_str());
			x = atof(subParts[1].c_str());
			y = atof(subParts[2].c_str());
			z = atof(subParts[3].c_str());
		}

		if (x != 0.0) {
			// Take the magnitude of the x, y, and z columns and store them
			magnitude.push_back(sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));

			// Time data offset to match analyzed magnitude data
			if (i > MAGNITUDE_AVERAGE_INTERVAL + DIFF_AVERAGE_INTERVAL + SQUARE_AVERAGE_INTERVAL + 1) {
				time.push_back(t);
			}
		}
		i++;
	}

	// Map the vectors and return
	vectorMap["magnitude"] = magnitude;
	vectorMap["time"] = time;
	return vectorMap;
}

vector<float> getDiff(vector<float> magnitude) {
	// Find the difference of each value and the one preceding it
	vector<float> diff;
	for (int i = 0; i < magnitude.size() - 1; i++) {
		diff.push_back(magnitude[i + 1] - magnitude[i]);
	}
	return diff;
}

vector<float> getSquare(vector<float> diff) {
	// Square each value
	vector<float> square;
	for (int i = 0; i < diff.size(); i++) {
		square.push_back(pow(diff[i], 2));
	}
	return square;
}

vector<float> getMovingAverage(vector<float> square, int moveLength) {
	// Calculate a moving average based on set interval length
	vector<float> movingAverage;
	for (int i = moveLength; i < square.size(); i++) {
		float average = 0;
		for (int j = 0; j < moveLength; j++) {
			average += square[i - j];
		}
		average = average / moveLength;
		movingAverage.push_back(average);
	}
	return movingAverage;
}

int countPeaksWithThreshold(vector<float> squareAverage, float threshold) {
	// Count peaks based on a fixed threshold
	int stepCount = 0;
	for (int i = 1; i < squareAverage.size() - 1; i++) {
		if (squareAverage[i] > squareAverage[i - 1] && squareAverage[i] > squareAverage[i + 1] && squareAverage[i] > threshold) {
			stepCount++;
		}
	}
	return stepCount;
}

// Iterate through every interval until the value is greater than a threshold
// If a value is greater than a threshold, create a window based on 1 Hz and find the local maximum of the whole window
// Once the maximum is found, record the timestamp and create a new window between 1-3 Hz and find a new local maximum
// If the new local maximum is above the threshold, count a step and repeat the process

int walkingIntervals(vector<float> squareAverage, vector<float> time) {
	// Initialize necessary variables
	bool walking = false; // Assume not walking at the start
	vector<vector<int>> intervals; // Vector for storing intervals
	int stepCountInterval = 0; // Step counter
	int startHalfSteps = STARTING_HALF_STEPS; // Starting values
	int endHalfSteps = ENDING_HALF_STEPS; // Starting values

	for (int i = 0; i < squareAverage.size()) {
		if (squareAverage[i] > THRESHOLD) {
			
		}
	}

	for (int i = LOCAL_MAXIMUM; i < squareAverage.size() - 1; i++) {

		// Check if there is user is currently walking
		if (!walking) {

			// If we count "MIN_HALF_STEPS" consecutive steps, then begin walking
			if(startHalfSteps <= 0) {

				// Count those initial steps
				stepCountInterval += STARTING_HALF_STEPS;

				// Reset the step counter
				startHalfSteps = STARTING_HALF_STEPS;
				cout << "Walking started at : " << to_string(i) << "\n";

				// Set walking variable to true
				walking = true;
			}

			// Find the next local maximum
			for (int j = i; j < squareAverage.size() - 1; j++) {
				// Compare for local maximum
				if (squareAverage[j] <= THRESHOLD && checkMax(squareAverage, j)) {
					break;
				}
				
				// If this is the first "step", begin a new interval
				if (startHalfSteps == STARTING_HALF_STEPS) {

					// Push a new interval to start count
					intervals.push_back(vector<int> (1, j));
					
					// Reduce "minHalfSteps"
					startHalfSteps--;

				} else if (time[j] > (time[i] + (1 / MAX_HERTZ)) && time[j] < (time[i] + (1 / MIN_HERTZ))) {
					// If the next step is within the hertz interval, reduce "minHalfSteps"
					startHalfSteps--;
				} else {

					// If the next "step" doesn't fall in the hertz interval, restart the tracking
					intervals.erase(intervals.begin() + intervals.size() - 1);
					startHalfSteps = STARTING_HALF_STEPS;
				}
				break;
			}
		} else {
			// User is currently walking

			// If there are a set number of steps that haven't fallen within the interval, end walking
			if(endHalfSteps <= 0) {

				// Set the end of the interval
				intervals[intervals.size() - 1].push_back(i);

				// Set "walking" to false
				endHalfSteps = ENDING_HALF_STEPS;

				cout << "Walking ended at : " << to_string(i) << "\n";
				walking = false;
				
			}

			// Find the next local maximum
			for (int j = i; j < squareAverage.size() - 1; j++) {

				if (squareAverage[j] <= THRESHOLD) {
					break;
				}
				// Compare for local maximum
				for (int k = 1; k < LOCAL_MAXIMUM; k++) {
					if (squareAverage[j] <= squareAverage[j - k] && squareAverage[j] <= squareAverage[j + k]) {
						break;
					}
				}
					
				if (time[j] > (time[i] + (1 / MAX_HERTZ)) && time[j] < (time[i] + (1 / MIN_HERTZ))) {

					// If the step falls within the hertz interval
					endHalfSteps = ENDING_HALF_STEPS;
					stepCountInterval++;
				} else {

					// If the step falls outside of the hertz interval, reduce "minHalfSteps"
					endHalfSteps--;
					stepCountInterval++;
				}
				break;
			}
		}
	}
	for (int k = 0; k < intervals.size(); k++) {
		//printVector(intervals[k]);
		cout << "\n";
	}
	return stepCountInterval / 2;
}

bool checkMax(vector<float> squareAverage, int j) {
	for (int k = 1; k < LOCAL_MAXIMUM; k++) {
		cout << "Lower val at: " << to_string(j - k) << " val is: " << squareAverage[j - k] << "\n";
		cout << "Higher val at: " << to_string(j + k) << " val is: " << squareAverage[j + k] << "\n";
		if (squareAverage[j] <= squareAverage[j - k] && squareAverage[j] <= squareAverage[j + k]) {
			return false;
		}
	}
	cout << "Maximum found at : " << to_string(j) << " val is: " << squareAverage[j] << "\n";
	return true;
}

void printVector(vector<int> vectorToPrint) {
	for (int i = 0; i < vectorToPrint.size(); i++) {
		cout << "value: " << vectorToPrint[i] << "\n";
	}
}

// Standard output file so I could see what was happening
// Source: https://slaystudy.com/c-how-to-write-a-string-vector-to-file/
void writeVectorToFile(const vector<float>& vectorToWrite, const string& filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    for (int i = 0; i < vectorToWrite.size(); i++) {
        outputFile << vectorToWrite[i] << "\n";
    }
    outputFile.close();
}