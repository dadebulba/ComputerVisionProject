#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <string.h>
#include "opencv/cxcore.h"
#include "./modules/module3.h"
#include "./modules/module2.h"

#define MOD3_CONFIG_ROW_LEN 8
#define MOD2_CONFIG_ROW_LEN 4

using namespace std;
using namespace cv;

void stringSplit(string s, string delim, vector<string> &out){
	size_t pos_start = 0, pos_end, delim_len = delim.length();
	string token;

	while ((pos_end = s.find (delim, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		out.push_back (token);
	}

	out.push_back (s.substr (pos_start));
}

void extractRectangles(string s, vector<Rect> &out){
	vector<string> elem;
	vector<int> rect;

	boost::split(elem, s, [](char c){return c==' ' || c=='[' || c=='(' || c==')' || c==']' || c==';';});
	for (int i=0; i<elem.size(); i++){
		if (elem[i] != ""){
			rect.push_back(stoi(elem[i]));

			if (rect.size() == 4){
				out.push_back(Rect(rect[0], rect[1], rect[2], rect[3]));
				rect.clear();
			}
		}
	}
}

string computeOutputVideoPath(string inputPath){
	vector<string> outputVideoPathSplit;
	string out = "";
	bool prefixDone = false;

	stringSplit(inputPath, ".", outputVideoPathSplit);
	for (int i=0; i<outputVideoPathSplit.size(); i++) {
		if (outputVideoPathSplit[i] != ""){
			if (!prefixDone){
				prefixDone = true;
				out = out + outputVideoPathSplit[i] + "_OUTPUT.";
			}
			else {
				out = out + outputVideoPathSplit[i];
				break;
			}
		}
		else
			out = out + ".";
	}

	return out;
}

int executeModule3(bool verbose) {

	// Print stats
	cout<<"Executing module 3 --- verbose = "<<verbose<<endl<<endl;

	// Open config file
	ifstream config("./config/module3.csv");
	string line;

	// Read headers
	getline(config, line);

	while (getline(config, line)){
		vector<string> split;
		stringSplit(line, ",", split);

		if (split.size() != MOD3_CONFIG_ROW_LEN){
			cerr<<"Invalid config file - "<<split.size()<<" arguments instead of "<<MOD3_CONFIG_ROW_LEN<<endl;
			return 1;
		}

		// Parsing values
		module3Config configObj;
		vector<Rect> rectangles;

		configObj.videoPath = split[0];
		configObj.outputVideoPath = computeOutputVideoPath(split[0]);
		configObj.widthScale = stoi(split[1]);
		configObj.heightScale = stoi(split[2]);
		configObj.accumulationPhase_frameStart = stoi(split[3]);
		configObj.accumulationPhase_frameEnd = stoi(split[4]);
		configObj.thresholdHigh = stoi(split[5]);
		configObj.thresholdLow = stoi(split[6]);

		extractRectangles(split[7], rectangles);
		configObj.rectangles = rectangles;

		// Print stats
		cout<<"Parsing "<<configObj.videoPath<<" with parameters:\n\t- Output file: "<<configObj.outputVideoPath<<"\n\t- Width scale: "<<configObj.widthScale<<"\n\t- Height scale: "<<configObj.heightScale<<endl;
		cout<<"\t- ACC frame start: "<<configObj.accumulationPhase_frameStart<<"\n\t- ACC frame end: "<<configObj.accumulationPhase_frameEnd<<endl;
		cout<<"\t- Threshold high: "<<configObj.thresholdHigh<<"\n\t- Threshold low: "<<configObj.thresholdLow<<"\n\t- Rectangles: "<<split[7]<<endl<<endl;

		//Parse the video
		module3(configObj, verbose);

	}
	return 0;
}

int executeModule2(bool verbose) {

	// Print stats
	cout<<"Executing module 2 --- verbose = "<<verbose<<endl<<endl;

	// Open config file
	ifstream config("./config/module2.csv");
	string line;

	// Read headers
	getline(config, line);

	while (getline(config, line)){
		vector<string> split;
		stringSplit(line, ",", split);

		if (split.size() != MOD2_CONFIG_ROW_LEN){
			cerr<<"Invalid config file - "<<split.size()<<" arguments instead of "<<MOD2_CONFIG_ROW_LEN<<endl;
			return 1;
		}

		// Parsing values
		module2Config configObj;
		vector<Rect> rectangles;
		configObj.videoPath = split[0];
		configObj.threshold = stoi(split[1]);
		configObj.numAnomalyParam = stoi(split[2]);
		configObj.numNormalParam = stoi(split[3]);
		
		// Print stats
		cout<<"Parsing "<<configObj.videoPath
		<<" with parameters:\n\t- Threshold: "<<configObj.threshold
		<<"\n\t- Num consequent anomaly frame: "<<configObj.numAnomalyParam
		<<"\n\t- Num consequent normal frame: "<<configObj.numNormalParam<<endl;

		//Parse the video
		module2(configObj, verbose);

	}
	return 0;
}

void usage(){
	cout<<"USAGE: ./ComputerVisionProject <ModuleNumber> [-v|--verbose]"<<endl<<endl;
}

int main(int argc, char* argv[]){
	if (argc > 3 || argc == 1){
		usage();
		return 0;
	}

	bool verbose = false;
	if (argc == 3 && (strcmp(argv[2], "-v") == 0 || strcmp(argv[2], "--verbose") == 0))
		verbose = true;

	if (strcmp(argv[1], "2") == 0)
		return executeModule2(verbose);
	else if (strcmp(argv[1], "3") == 0)
		return executeModule3(verbose);
	else {
		usage();
		return 0;
	}

}
