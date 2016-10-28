#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <bitset>
#include <array>

using namespace std;

const int INPUTRANGE = 4;
const int INPUTRANGEDECR = INPUTRANGE - 1;
const int OUTPUTSIZE = 2;
const int TRELLISHEIGHT = 8;

const bool XOR1REF = false;
const bool XOR2REF = true;

const string INPUTDIR = ".\\input\\";
const string OUTPUTDIR = ".\\output\\";
const string FILEEXT = ".txt";
const string DECODEDEXT = "dec";

const int COLCURR = 0;
const int COLIN0 = 1;
const int COLIN1 = 2;

const int TABWIDTH = 3;
const int TABHEIGHT = 8;

const int BURSTERRMAX = 21;
const int BURSTERRTHRESH = 19;
const int BURSTERRSTREAK = 1;


typedef unsigned int uint;


struct boolAbstract {	//some convenience methods for both bool2 and bool3
	bool convCharToBool(const char &in) {
		if (in == '0') { return false; }
		else { return true; }
	}
	char convBoolToChar(const bool &in) {
		if (in == false) { return '0'; }
		else { return '1'; }
	}
	virtual void setData(const string &in) = 0;
	virtual const string toStr() = 0;
};

//a pair of bools and some methods, for storing outputs
struct bool2 : boolAbstract {
	bool data[2];

	void setData(const string &in) {
		data[0] = convCharToBool(in.at(0));
		data[1] = convCharToBool(in.at(1));
	}
	const string toStr() {
		string result = "";
		result += convBoolToChar(data[0]);
		result += convBoolToChar(data[1]);
		return result;
	}
};

//a triplet of bools for storing register state
struct bool3 : boolAbstract {
	bool data[3];

	void setData(const string &in) {
		data[0] = convCharToBool(in.at(0));
		data[1] = convCharToBool(in.at(1));
		data[2] = convCharToBool(in.at(2));
	}
	const string toStr() {
		string result = "";
		result += convBoolToChar(data[0]);
		result += convBoolToChar(data[1]);
		result += convBoolToChar(data[2]);
		return string(result);
	}
	const string getLeftPair() {
		string result = "";
		result += convBoolToChar(data[0]);
		result += convBoolToChar(data[1]);
		return result;
	}
	const int getInt() {
		int result = 0;
		if (data[0]) { result += 4; }
		if (data[1]) { result += 2; }
		if (data[2]) { result += 1; }
		return result;
	}
};

//a viterbi trellis node
struct vitNode {
	vitNode() {
		fromNode = 0;
		metric = 0;
	}

	vitNode(const int &fromNode, const int &metric) {
		this->fromNode = fromNode;
		this->metric = metric;
	}
	void toZero() {
		fromNode = 0;
		metric = 0;
	}
	
	int fromNode;
	int metric;
};


class Decoder {
public:
	Decoder(string xorSett1 = defSett1, string xorSett2 = defSett2, string inFilepath = defInFilepath, string outFilepath = defOutFilepath);
	~Decoder();

	void RunDecoderStandard();	//decodes with simple lookup table
	void RunDecoderViterbi(const bool &error);	//decodes with viterbi algo

	void ErrorInput();	//introduce errors to inputData

	void PrintTables();	//print out the state and input tables to console
	void DecoderSetting(bool xorNum, string xorSett);	//modify decoder settings

	void SetInputPath(string path);		//change where the data will be input from
	void SetOutputPath(string path);	//change where the data will be output to

protected:
	void PopulateStateTable();
	void PopulateInputTable();
	void PopulateNextTable();

	bool ReadInData();
	bool WriteOutData();

	inline string xorSettToStr(const bool &gate) {
		string resultStr = "";
		for (int i = 0; i < INPUTRANGE; i++) {
			if (xorInputs[gate][i] == false) 
				{ resultStr += '0'; }
			else 
				{ resultStr += '1'; }
		}

		return resultStr;
	}


	int HammVal(const bool2 &in1, const bool2 &in2);	//returns the hamming value from two pairs of bools

	vector<bool> inputData;
	vector<bool> outputData;


	bool3 stateTable[TABWIDTH][TABHEIGHT];	//state table stores the next state to go to
	bool3 inputTableCur[TABHEIGHT]; //input table stores what to output with each state
	bool2 inputTableOut[TABWIDTH - 1][TABHEIGHT];
	bool nextStateTable[TABHEIGHT][TABHEIGHT];	//next state table stores the input bits when going from one state to the next (used in viterbi)

	bool xorInputs[2][INPUTRANGE];	//gate settings
	 
	string inputFilepath;	//the path to read in from
	string outputFilepath;	//the path to output data to

	

	const static string defSett1;		//default input settings for xor gate 1
	const static string defSett2;		//default input settings for xor gate 2
	const static string defInFilepath;	//default input file location
	const static string defOutFilepath;	//default output file location
	const static string defErrFilepath;	//default error file location
};


