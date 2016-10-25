#include <iostream>
#include "Decoder.h"

using namespace std;

int main() {

	Decoder decProg;

	/*
	int x = 0;
	bool exitValue = false;

	while (!exitValue) {
		//menu options!
		cout << endl << "Type the number of the instruction to execute:" << endl;
		cout << "0: Run encoder" << endl;
		cout << "1: Change xor gate settings" << endl;
		cout << "2: Change input file" << endl;
		cout << "3: Change output file" << endl;
		cout << "4: Run encoder on all possible permutations" << endl;
		cout << "5: End program" << endl;

		cin >> x;

		string stringInput;

		switch (x) {
		case 0: //Run encoder
			cout << endl << "Running encoder..." << endl;
			decProg.RunEncoderPreStep();
			decProg.RunEncoder();
			break;

		case 1: //Change xor gate settings
			cout << endl << "Which gate should be modified, 1 or 2?" << endl;
			cin >> x;
			cout << endl << "And what pattern should it be changed to? (E.g. \"0011\"" << endl;
			cin >> stringInput;

			if (x == 1) {
				decProg.EncoderSetting(false, stringInput);
			}
			else {
				decProg.EncoderSetting(true, stringInput);
			}

			cout << endl << "Gate setting changed." << endl;
			break;

		case 2: //Change input file
			cout << endl << "Where should file be input from?" << endl;
			cin >> stringInput;
			decProg.SetInputPath(stringInput);
			break;

		case 3: //Change output file
			cout << endl << "Where should file be output to?" << endl;
			cin >> stringInput;
			decProg.SetOutputPath(stringInput);
			break;

		case 4: //Change output file
			cout << endl << "Running all encoder permutations..." << endl;
			decProg.RunEncoderFullCycle();
			break;

		default: //end program
			exitValue = true;
			break;
		}

	}

	return 0;
	*/

	system("pause");


}

