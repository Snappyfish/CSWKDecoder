#include "Decoder.h"


const string Decoder::defSett1 = "0011";
const string Decoder::defSett2 = "1100";
const string Decoder::defInFilepath = "outputFile.txt";
const string Decoder::defOutFilepath = "decodedFile.txt";



Decoder::Decoder(string xorSett1, string xorSett2, string inFilepath, string outFilepath) {
	
	EncoderSetting(XOR1REF, xorSett1);
	EncoderSetting(XOR2REF, xorSett2);
	
	inputFilepath = inFilepath;
	outputFilepath = outFilepath;
	
	PopulateStateTable();
	PopulateInputTable();

	PrintTables();



}

Decoder::~Decoder() {

}

void Decoder::PrintTables() {
	cout << "State." << endl;
	for (int i = 0; i < TABHEIGHT; i++) {
		for (int j = 0; j < TABWIDTH; j++) {
			cout << stateTable[j][i].toStr() << " ";
		}
		cout << endl;
	}
	cout << endl << endl;

	cout << "Output." << endl;
	for (int i = 0; i < TABHEIGHT; i++) {
		cout << inputTableCur[i].toStr() << " ";
		cout << inputTableOut[0][i].toStr() << " ";
		cout << inputTableOut[1][i].toStr();
		cout << endl;
	}

}

void Decoder::EncoderSetting(bool xorNum, string xorSett) {
	if (xorNum) {
		for (int i = 0; i < INPUTRANGE; i++) {
			char temp = xorSett.at(i);
			if (temp == '0') {
				xorInputs[0][i] = false;
			}
			else {
				xorInputs[0][i] = true;
			}
		}
	}
	else {
		for (int i = 0; i < INPUTRANGE; i++) {
			char temp = xorSett.at(i);
			if (temp == '0') {
				xorInputs[1][i] = false;
			}
			else {
				xorInputs[1][i] = true;
			}
		}
	}

}



void Decoder::PopulateStateTable() {
	for (int i = 0; i < TABHEIGHT; i++) {
		stateTable[0][i].setData(bitset<3>(i).to_string());
		stateTable[1][i].setData("0" + stateTable[0][i].getLeftPair());
		stateTable[2][i].setData("1" + stateTable[0][i].getLeftPair());
	}
	
}

void Decoder::PopulateInputTable() {
	//for every row
	for (int i = 0; i < TABHEIGHT; i++) {
		inputTableCur[i].setData(bitset<3>(i).to_string());

		//for each column
		for (int j = 0; j < OUTPUTSIZE; j++) {
			string inputData = to_string(j);
			inputData += inputTableCur[i].toStr();

			//left bool of that cell
			int result = 0;
			for (int k = 0; k < INPUTRANGE; k++) {
				if (xorInputs[j][k])
					result += atoi(&inputData[k]);
			}
			string temp = to_string(result % 2);

			//right bool
			result = 0;
			for (int k = 0; k < INPUTRANGE; k++) {
				if (xorInputs[j][k])
					result += atoi(&inputData[k]);
			}
			temp += to_string(result % 2);

			inputTableOut[j][i].setData(temp);
		}
	}



}


