#include "Decoder.h"


const string Decoder::defSett1 = "0011";
const string Decoder::defSett2 = "1100";
const string Decoder::defInFilepath = "outputFile.txt";
const string Decoder::defOutFilepath = "decodedFile.txt";
const string Decoder::defErrFilepath = "errOutput";


Decoder::Decoder(string xorSett1, string xorSett2, string inFilepath, string outFilepath) {
	
	//set default settings
	DecoderSetting(XOR1REF, xorSett1);
	DecoderSetting(XOR2REF, xorSett2);
	
	inputFilepath = inFilepath;
	outputFilepath = outFilepath;
	
	//and pre-populate the tables, ready for use
	PopulateStateTable();
	PopulateInputTable();
	PopulateNextTable();

}

Decoder::~Decoder() {
	inputData.clear();
	outputData.clear();

}

void Decoder::RunDecoderStandard() {
	ReadInData();
	
	int maxLoop = (inputData.size() / OUTPUTSIZE) - INPUTRANGEDECR;	//loop through file for each pair of bits
	bool3 regState;
	regState.setData("000");
	bool currData[2];	//the current pair of bits we're looking at

	//for every pair of input bits
	for (int i = 0; i < maxLoop; i++) {
		currData[0] = inputData.at(2 * i);
		currData[1] = inputData.at((2 * i) + 1);

		//look up input bit with input table
		string compBits = "";

		//fill compBits with next two digits from file
		if (currData[0] == false) { compBits += '0'; }
		else { compBits += '1'; }
		if (currData[1] == false) { compBits += '0'; }
		else { compBits += '1'; }

		int stateRow;
		bool loopCheck = true;	//lookup the correct row of states
		for (int row = 0; row < TABHEIGHT && loopCheck; row++) {
			if (stateTable[0][row].toStr() == regState.toStr()) {
				stateRow = row;
				loopCheck = false;
			}
		}

		bool addedBit;	//if the pair of bits read from file match the first row, it must be a 0 input
		if (compBits == inputTableOut[0][stateRow].toStr()) {
			outputData.push_back(false);	//so add a 0 to the output queue
			addedBit = false;
		}	//else it must be a 1
		else if (compBits == inputTableOut[1][stateRow].toStr()) {
			outputData.push_back(true);	//so add a 1 to the output queue
			addedBit = true;
		}
		//if it's neither of the pairs in the table, there must be an error present in the file, as this state cannot be reached without erroneous data.
		else {
			cout << "Error! Impossible state, terminating run." << endl;
			outputData.clear();
			return;	//as we need to know the next state to calc. the next input, we cannot continue
		}

		//update regState from state table, ready for next cycle
		loopCheck = true;	//lookup which row contains the current state
		for (int row = 0; row < TABHEIGHT && loopCheck; row++) {
			if (stateTable[0][row].toStr() == regState.toStr()) {
				//then update the state depending on whether we just added a 0 or a 1 to the output queue
				if (!addedBit) {
					regState.setData(stateTable[1][row].toStr());
				}
				else {
					regState.setData(stateTable[2][row].toStr());
				}
				loopCheck = false;
			}
		}

	}
	
	//finally write it out to file!
	if (WriteOutData()) {
		cout << "Success." << endl;
	}
	else {
		cout << "Failed!" << endl;
	}
	outputData.clear();

}





void Decoder::RunDecoderViterbi(const bool &error) {
	ReadInData();

	//if specified, add some errors to the input
	if(error) { ErrorInput(); }

	int maxLoop = inputData.size() / 2;
	int currLoop = 0;
	bool2 realData;

	std::array<vitNode, 8> nullSample;	//a null array to add to the vector below

	//first, fill in the trellis
	vector<std::array<vitNode, 8>> vitTrellis;

	//set first column to zero
	vitTrellis.push_back(nullSample);
	

	//loop through each column (set of 8 nodes)
	for (int currCol = 1; currCol < maxLoop; currCol++) {
		vitTrellis.push_back(nullSample);

		//get the current bits to work on
		realData.data[0] = inputData.at(2 * currLoop);
		realData.data[1] = inputData.at((2 * currLoop) + 1);

		//then for each node, fill in the trellis data
		for (int currRow = 0; currRow < TRELLISHEIGHT; currRow++) {

			int fromStateStor[2];
			bool2 possData[2];

			//for both input bit possibilities
			for (int ib = 0; ib < 2; ib++) {
				int fromState = (currRow * 2 + ib) % 8;

				int stateRow;	//lookup which row matches the previous state
				bool loopCheck = true;
				for (int j = 0; j < TABHEIGHT && loopCheck; j++) {
					if (inputTableCur[j].toStr() == bitset<3>(fromState).to_string()) {
						stateRow = j;
						loopCheck = false;
					}
				}

				//store the data for later
				fromStateStor[ib] = fromState;
				int inputBit = 0;
				if (currRow > 3) { inputBit = 1; }
				possData[ib] = inputTableOut[inputBit][stateRow];
			}

			//calculate the 2 possible branch metrics
			int possBranchMetric[2];
			for (int i = 0; i < 2; i++) {	//add the predesessor's metric value to the current node's
				int preMetric = HammVal(realData, possData[i]);
				int thisMetric = vitTrellis[currCol - 1][fromStateStor[i]].metric;
				possBranchMetric[i] = preMetric + thisMetric;
			}
			
			//then pick the lower one and fill out the data!
			if (possBranchMetric[0] <= possBranchMetric[1]) {
				vitTrellis[currCol][currRow].fromNode = fromStateStor[0];
				vitTrellis[currCol][currRow].metric = possBranchMetric[0];
			}
			else {
				vitTrellis[currCol][currRow].fromNode = fromStateStor[1];
				vitTrellis[currCol][currRow].metric = possBranchMetric[1];
			}

		}

		currLoop++;
	}


	//the first node to select will always be in state 0, as we add trailling zeroes to the data (to flush it through)
	int nextNode = 0;
	vector<int> nextNodeTable;
	nextNodeTable.push_back(nextNode);

	//NOW backtrack through the nodes
	for (int currCol = maxLoop - 1; currCol >= 0; currCol--) {
		nextNode = vitTrellis[currCol][nextNode].fromNode;
		nextNodeTable.push_back(nextNode);
	}

	//remove the leading 0 at the start
	nextNodeTable.pop_back();
	
	//then flip the nodes back to correct order and remove trailling zeroes (from the register flushing)
	reverse(nextNodeTable.begin(), nextNodeTable.end());
	nextNodeTable.pop_back();
	nextNodeTable.pop_back();

	//penultimately lookup and add the data to outputData using the next state table
	for (uint i = 0; i < (nextNodeTable.size() - 1); i++) {
		outputData.push_back(nextStateTable[nextNodeTable[i]][nextNodeTable[i + 1]]);
	}


	//finally output to file!
	if (WriteOutData()) {
		cout << "Success." << endl;
	}
	else {
		cout << "Failed!" << endl;
	}

	outputData.clear();

}






void Decoder::ErrorInput() {
	bool burstMode = false;
	int burstModeCount = 0;

	//for every character in inputData
	for (uint i = 0; i < inputData.size(); i++) {
		if (burstMode) {	//if burst mode is active
			//add errors!
			inputData[i] = (bool)((rand() % 2) != 0);
			burstModeCount++;

			//check if we've added enough errors
			if (burstModeCount > BURSTERRSTREAK) {	//if so, end burst mode
				burstMode = false;
				burstModeCount = 0;
			}
		}
		else {	//roll the dice again and see if we should activate burst
			if ((rand() % BURSTERRMAX) > BURSTERRTHRESH) {
				burstMode = true;
			}
		}
	}

	//and output the file with errors in it for later viewing
	string errPath = OUTPUTDIR + defErrFilepath + FILEEXT;
	ofstream f(errPath.c_str(), ios::out);
	for (uint i = 0; i < inputData.size(); i++) {
		f << inputData[i];
	}
	f.close();

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

void Decoder::DecoderSetting(bool xorNum, string xorSett) {
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
		stateTable[0][i].setData(bitset<3>(i).to_string());	//this is a 3-long binary number
		stateTable[1][i].setData("0" + stateTable[0][i].getLeftPair());	//put a 0 before the current state (without the last bool)
		stateTable[2][i].setData("1" + stateTable[0][i].getLeftPair());	//like above but starts with a 1!
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

	//was going to do a quick analysis on the encoder to see if it's a good one or not
	//check each of the rows for duplicates, and output a warning if it's a bad table

}

void Decoder::PopulateNextTable() {
	for (int i = 0; i < TABHEIGHT; i++) {
		int curr = stateTable[0][i].getInt();
		int nextF = stateTable[1][i].getInt();
		int nextT = stateTable[2][i].getInt();

		nextStateTable[curr][nextF] = false;
		nextStateTable[curr][nextT] = true;
	}

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

	SetOutputPath(
		OUTPUTDIR +
		xorSettToStr(XOR1REF) + "-" + xorSettToStr(XOR2REF) +
		DECODEDEXT + FILEEXT);

	//setup input stream
	ofstream f(outputFilepath.c_str(), ios::out);

	if (!f) {
		cout << "Aw nuts. File output failed." << endl;
		f.close();
		return false;
	}

	//take characters from outputData and print to file
	uint dataSize = outputData.size();
	for (uint i = 0; i < dataSize; i++) {
		f << outputData[i];
	}

	f.close();
	return true;
}

int Decoder::HammVal(const bool2 &in1, const bool2 &in2) {
	int result = 0;

	if (in1.data[0] != in2.data[0]) { result++; }
	if (in1.data[1] != in2.data[1]) { result++; }
	return result;
}


