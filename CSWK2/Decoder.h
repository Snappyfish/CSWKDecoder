#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

const int INPUTRANGE = 4;
//const int MAXPERMU = INPUTRANGE * INPUTRANGE;
const int OUTPUTSIZE = 2;

const bool XOR1REF = false;
const bool XOR2REF = true;

const string OUTPUTDIR = ".\\output\\";
const string FILEEXT = ".txt";

const int COLCURR = 0;
const int COLIN0 = 1;
const int COLIN1 = 2;

const int TABWIDTH = 3;
const int TABHEIGHT = 8;

typedef unsigned int uint;


struct char2 {
	char data[2];

	void setData(string in) {
		data[0] = in.at(0);
		data[1] = in.at(1);
	}
	string const toStr() {
		string result = "";
		result += data[0];
		result += data[1];
		return string(result);
	}
};

struct char3 {
	char data[3];

	void setData(string in) {
		data[0] = in.at(0);
		data[1] = in.at(1);
		data[2] = in.at(2);
	}
	string const toStr() {
		string result = "";
		result += data[0];
		result += data[1];
		result += data[2];
		return string(result);
	}
	string const getLeftPair() {
		string result = "";
		result += data[0];
		result += data[1];
		return result;
	}
};


class Decoder {
public:
	Decoder(string xorSett1 = defSett1, string xorSett2 = defSett2, string inFilepath = defInFilepath, string outFilepath = defOutFilepath);
	~Decoder();

	void PrintTables();
	void EncoderSetting(bool xorNum, string xorSett);

protected:
	void PopulateStateTable();
	void PopulateInputTable();



	char3 stateTable[TABWIDTH][TABHEIGHT];
	char3 inputTableCur[TABHEIGHT];
	char2 inputTableOut[TABWIDTH - 1][TABHEIGHT];

	bool xorInputs[2][INPUTRANGE];

	string inputFilepath;	//the path to read in from
	string outputFilepath;	//the path to output data to


	const static string defSett1;		//default input settings for xor gate 1
	const static string defSett2;		//default input settings for xor gate 2
	const static string defInFilepath;	//default input file location
	const static string defOutFilepath;	//default input file location
};


