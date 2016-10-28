#include <iostream>
#include "Decoder.h"

using namespace std;

int main() {

	Decoder decProg;

	
	int x = 0;
	bool exitValue = false;

	while (!exitValue) {
		//menu options!
		cout << endl << "Type the number of the instruction to execute:" << endl;
		cout << "0: Run decoder - standard" << endl;
		cout << "1: Run decoder - viterbi" << endl;
		cout << "2: Run decoder - viterbi with errors" << endl;
		cout << "3: Change xor gate settings" << endl;
		cout << "4: Change input file" << endl;
		cout << "5: Change output file" << endl;
		cout << "6: Run decoder on setting-named file" << endl;
		cout << "7: Print out current state and input tables" << endl;
		cout << "8: End program" << endl;

		//temporary inputs for the options menu
		string stringInput;
		string temp1;
		string temp2;


		cin >> x;
		switch (x) {
		case 0: //Run encoder standard
			cout << endl << "Running decoder..." << endl;
			decProg.RunDecoderStandard();
			break;

		case 1: //Run encoder viterbi
			cout << endl << "Running viterbi decoder..." << endl;
			decProg.RunDecoderViterbi(false);
			break;

		case 2: //Run encoder viterbi + errors
			cout << endl << "Running viterbi decoder... (with errors)" << endl;
			decProg.RunDecoderViterbi(true);
			break;

		case 3: //Change xor gate settings
			cout << endl << "Which gate should be modified, 1 or 2?" << endl;
			cin >> x;
			cout << endl << "And what pattern should it be changed to? (E.g. \"0011\")" << endl;
			cin >> stringInput;

			if (x == 1) {
				decProg.DecoderSetting(XOR1REF, stringInput);
			}
			else {
				decProg.DecoderSetting(XOR2REF, stringInput);
			}

			cout << endl << "Gate setting changed." << endl;
			break;

		case 4: //Change input file
			cout << endl << "Where should file be input from?" << endl;
			cin >> stringInput;
			decProg.SetInputPath(stringInput);
			break;

		case 5: //Change output file
			cout << endl << "Where should file be output to?" << endl;
			cin >> stringInput;
			decProg.SetOutputPath(stringInput);
			break;

		case 6: //Run decoder on a mask-named file (like 0011-1100.txt)
			cout << endl << "What is the file called?" << endl;
			cin >> stringInput;
			decProg.SetInputPath(INPUTDIR + stringInput + FILEEXT);
			temp1 = stringInput.substr(0, 4);
			temp2 = stringInput.substr(5, 4);
			decProg.DecoderSetting(XOR1REF, temp1);
			decProg.DecoderSetting(XOR2REF, temp2);

			cout << endl << "Run in which mode?" << endl;
			cout << "0: Standard" << endl;
			cout << "1: Viterbi" << endl;
			cout << "2: Viterbi with errors" << endl;
			cin >> x;

			switch (x){
			case 0:
				decProg.RunDecoderStandard();
				break;

			case 1:
				decProg.RunDecoderViterbi(false);
				break;

			case 2:
				decProg.RunDecoderViterbi(true);
				break;

			default:
				cout << "Neither entered, cancelling." << endl << endl;
			}

			break;

		case 7: //Print tables
			decProg.PrintTables();
			break;

		default: //end program
			exitValue = true;
			break;
		}

	}


	return 0;
}

