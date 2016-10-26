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

void Decoder::RunDecoder() {
	ReadInData();
	
	int maxLoop = inputData.size() / 2;
	bool3 regState;
	regState.setData("000");
	bool currData[2];

	//for every pair of input bits
	for (int i = 0; i < maxLoop; i++) {
		currData[0] = inputData.at(2 * i);
		currData[1] = inputData.at((2 * i) + 1);

		//look up input bit with input table
		string compBits = "";
		if (currData[0] == false) {
			compBits += '0';
		}
		else {
			compBits += '1';
		}
		if (currData[1] == false) {
			compBits += '0';
		}
		else {
			compBits += '1';
		}

		int stateRow;
		bool loopCheck = true;
		for (int j = 0; j < TABHEIGHT && loopCheck; j++) {
			if (stateTable[0][j].toStr() == regState.toStr()) {
				stateRow = j;
				loopCheck = false;
			}
		}

		bool addedBit;
		if (compBits == inputTableOut[0][stateRow].toStr()) {
			outputData.push_back(false);
			addedBit = false;
		}
		else if (compBits == inputTableOut[1][stateRow].toStr()) {
			outputData.push_back(true);
			addedBit = true;
		}
		else {
			cout << "Error! Impossible state, terminating run." << endl;
			return;
		}

		//update regState with state table
		loopCheck = true;
		for (int j = 0; j < TABHEIGHT && loopCheck; j++) {
			if (stateTable[0][j].toStr() == regState.toStr()) {
				if (!addedBit) {
					regState.setData(stateTable[1][j].toStr());
				}
				else {
					regState.setData(stateTable[2][j].toStr());
				}
				loopCheck = false;
			}
		}

	}
	

	if (WriteOutData()) {
		cout << "Success." << endl;
	}
	else {
		cout << "Failed!" << endl;
	}


}

void Decoder::ErrorInput() {
	bool burstMode = false;
	int burstModeCount = 0;

	//for every character in inputData
	for (uint i = 0; i < inputData.size(); i++) {
		if (burstMode) {
			inputData[i] = (bool)((rand() % 2) != 0);
			burstModeCount++;
			if (burstModeCount > BURSTERRSTREAK) {
				burstMode = false;
				burstModeCount = 0;
			}
		}
		else {
			if ((rand() % BURSTERRMAX) > BURSTERRTHRESH) {
				burstMode = true;
			}
		}
	}




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
	for (int i = 0; i < INPUTRANGE; i++) {
		if (xorSett.at(i) == '0') {
			xorInputs[xorNum][i] = false;
		}
		else {
			xorInputs[xorNum][i] = true;
		}
	}

	//re-populate the input tables, as they change depending on the xor gate settings
	PopulateInputTable();

}

void Decoder::SetInputPath(string path) {
	inputFilepath = path;
}
void Decoder::SetOutputPath(string path) {
	outputFilepath = path;
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
				if (xorInputs[0][k] && inputData.at(k) == '1') {
					result++;
				}
					
			}
			string temp = to_string(result % 2);

			//right bool
			result = 0;
			for (int k = 0; k < INPUTRANGE; k++) {
				if (xorInputs[1][k] && inputData.at(k) == '1') {
					result++;
				}
			}
			temp += to_string(result % 2);

			inputTableOut[j][i].setData(temp);
		}
	}

	//check each of the rows for duplicates, and output a warning if it's a bad table


}

bool Decoder::ReadInData() {
	//clean out current stored data
	inputData.clear();

	//setup input stream
	ifstream f(inputFilepath.c_str(), ios::in);

	if (!f) {//Oh dear, it can't find the file :(
		cout << "Aw nuts. No file to read in." << endl;
		f.close();
		return false;
	}
	char currentChar;

	while (f >> currentChar) {	//loop until it returns false (at eof)
		if (currentChar == '0') {
			inputData.push_back(false);
		}
		else if (currentChar == '1') {
			inputData.push_back(true);
		}
		else {
			cout << "Warning; invalid char in file: " << currentChar << endl;
		}

	}

	f.close();
	cout << "Read in data successfully!" << endl;
	return true;
}

bool Decoder::WriteOutData() {
	//setup input stream
	ofstream f(outputFilepath.c_str(), ios::out);

	if (!f) {
		cout << "Aw nuts. File output failed." << endl;
		f.close();
		return false;
	}

	SetOutputPath(
		OUTPUTDIR + 
		xorSettToStr(XOR1REF) + "-" + xorSettToStr(XOR2REF) +
		DECODEDEXT + FILEEXT);


	//take characters from outputData and print to file
	uint dataSize = outputData.size();
	for (uint i = 0; i < dataSize; i++) {
		f << outputData[i];
	}

	f.close();
	return true;
}


