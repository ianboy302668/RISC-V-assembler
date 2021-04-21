#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>  //find()
#include <bitset> // convert to binary integer
#include <fstream>
#include <iterator>

using namespace std;



string type_i[15] = { "jalr", "lb", "lh",  "lw",  "lbu",  "lhu",  "addi",  "slti",  "sltiu",  "xori", "ori", "andi", "slli", "srli", "srai"};
string type_r[8] = { "add",  "sub",  "sll",  "slt",  "sltu", "xor", "or", "and" };
string type_sb[6] = { "beq", "bne", "blt", "bge", "bltu", "bgeu" };

void type_i_machine_code(string, string);
void type_s_machine_code(string, string);
void type_r_machine_code(string, string);
void type_sb_machine_code(string, string, string[], int);
void type_uj_machine_code(string, string);
void type_u_machine_code(string, string);
int find_string_target(string[], string);

int main(int argc, char* argv[])
{
    //* input *//
	ifstream file;
	string input, action, ass_code, colon(":");
	string label[100] = {" "};
	stringstream input_ss;
	int type_state = 0; //type I:1 S:2 R:3 SB:4 UJ:5 U:6
	int label_to_imm = 0, counter = 0;

	// * open file and idiot-proof * //
	file.open(argv[1]);
	while (!file) {
		cout << "Can't open the file. Please enter the file name again.\n";
		cin >> input; //filename
		file.open(argv[1]);
	}

	// * put whole file into string * //
	input.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	// * find the line number of label in the file * // 
	input_ss << input;
	string temp;
	size_t pos1, pos2;
	while (!input.empty()) {
		counter++;

		pos1 = input.find("\n");
		temp = input.substr(0, pos1);
		
		pos2 = temp.find(colon);
		if (pos2 != string::npos) {
			temp = temp.substr(0, pos2);
			label[counter] = temp;
		}
		
		input.erase(0, pos1+1);
	}
	counter = 0;
	// * processing * //
	while (!input_ss.eof()) {	
		counter++;

		//* Distinguish type *//
		while (input_ss >> action) {
			string* p = find(type_i, type_i+15, action);
			if (p != type_i + 16 && *p == action) {
				type_state = 1;
				break;
			}

			if (action == "sw" || action == "sh" || action == "sw") {
				type_state = 2;
				break;
			}

			p = find(type_r, type_r + 8, action);
			if (p != type_r + 9 && *p == action) {
				type_state = 3;
				break;
			}

			p = find(type_sb, type_sb + 6, action);
			if (p != type_sb + 7 && *p == action) {
				type_state = 4;
				break;
			}

			if (action == "jal") {
				type_state = 5;
				break;
			}

			if (action == "lui" || action == "auipc") {
				type_state = 6;
				break;
			}
		}

		input_ss >> ass_code;	

		//* transfer into machine code *//
		switch (type_state) {
			case 1:
				type_i_machine_code(action,ass_code);
				break;
			case 2:
				type_s_machine_code(action, ass_code);
				break;
			case 3:
				type_r_machine_code(action, ass_code);
				break;
			case 4:
				type_sb_machine_code(action, ass_code, label, counter);
				break;
			case 5:
				type_uj_machine_code(action, ass_code);
				break;
			case 6:
				type_u_machine_code(action, ass_code);
				break;
			default:
				break;
		}

		//* Clear stringstream *//
		//input_ss.str("");
		//input_ss.clear();
		type_state = 0;
	}
}

void type_i_machine_code(string action, string code)
{
	int counter = 1;
	string delimiter = ",";
	size_t pos;
	string token;  //store part of code before every delimiter
	string rd, rs1, offset;  //output
	string funct3 = "000", opcode = "0010011";

	//* opcode / funct3 *//
	if (action == "jalr")	opcode = "1100111";
	else if (action == "lb" || action == "lh" || action == "lw" || action == "lbu" || action == "lhu") {
		opcode = "0000011";

		if (action == "lh") funct3 = "001";
		else if(action == "lw") funct3 = "010";
		else if (action == "lbu") funct3 = "100";
		else if (action == "lhu") funct3 = "101";
	}

	if (action == "addi") funct3 = "000";
	else if (action == "slti") funct3 = "010";
	else if (action == "sltiu") funct3 = "011";
	else if (action == "xori") funct3 = "100";
	else if (action == "ori") funct3 = "110";
	else if (action == "andi") funct3 = "111";
	else if (action == "slli") funct3 = "001";
	else if (action == "srli" || action == "srai") funct3 = "101";


	while (!code.empty()) {
		pos = code.find(",");

		token = code.substr(0, pos);

		//* for output *//
		if (opcode == "1100111" || opcode == "0000011") {
			switch (counter) {
			case 1:
				token.erase(0, 1); //erase 'x'
				rd = bitset<5>(stoi(token)).to_string();
				code.erase(0, code.find(delimiter) + 1);
				break;
			case 2: {
				token = code.substr(0, code.find("("));
				offset = bitset<12>(stoi(token)).to_string();
				code.erase(0, code.find("(") + 1);
				break;
			}
			default:
				token = code.substr(0, code.find(")"));
				token.erase(0, 1); //erase 'x'
				rs1 = bitset<5>(stoi(token)).to_string();
				code.erase(0, code.find(")")+2);
				break;
			}
		}
		else {
			switch (counter) {
			case 1:
				token.erase(0, 1); //erase 'x'
				rd = bitset<5>(stoi(token)).to_string();
				break;
			case 2: {
				token.erase(0, 1); //erase 'x'
				rs1 = bitset<5>(stoi(token)).to_string();
				break;
			}
			default:
				offset = bitset<12>(stoi(token)).to_string();
				code.erase(0, token.length());
				if (action == "srai")	offset[1] = '1';
				break;
			}
			code.erase(0, code.find(delimiter) + 1); //since the length of ",x" is 2
		}

		counter++;
	}

	cout << "Type I";  //debug
	cout.width(8);
	cout << cout.fill(' ') << "11   7  5" << setw(5) << "0\n";
	cout << "Machine code : " << offset << ' ' << rs1 << ' ' << funct3 << ' ' << rd << ' ' <<  opcode << endl;
}

void type_s_machine_code(string action, string code)  //OK//
{
	cout << "Type S \n";  //debug

	size_t pos;
	string token;
	string offset, rs1, rs2;  //output
	string funct3 = "000";
	
	//* funct3 *//
	if (action == "sh" || action == "SH")	funct3 = "001";
	else if (action == "sw" || action == "SW")	funct3 = "010";

	//* convert rs2 *//
	token = code.substr(0, code.find(","));
	token.erase(0, 1); //erase 'x'
	rs2 = bitset<5>(stoi(token)).to_string();
	code.erase(0, code.find(",")+1);

	//* convert offet *//
	token = code.substr(0, code.find("("));
	offset = bitset<12>(stoi(token)).to_string();
	code.erase(0, code.find("(")+1);

	//* convert rs1 *//
	token = code.substr(0, code.find(")"));
	token.erase(0, 1); //erase 'x'
	rs1 = bitset<5>(stoi(token)).to_string();
	code.erase(0, code.find(")"));

	cout << "Machine code : ";
	for (int i = 5; i < 12; i++)
		cout << offset[i];
	cout << rs2 << ' ' << rs1 << ' ' << funct3 << ' ';
	for (int i = 0; i < 5; i++)
		cout << offset[i];
	cout << " 0100011\n";  //opcode
}

void type_r_machine_code(string action, string code)  //ok//
{
	cout << "Type R \n";  //debug

	int counter = 1;
	string delimiter = ",";
	size_t pos;
	string token;
	string rd, rs1, rs2;  //output
	string funct5 = "00000", funct2 = "00", funct3 = "000", opcode = " 0110011";

	//* funct5 *//
	if (action == "sub" || action == "sra")	funct5 = "01000";
	
	//* funct3 *//
	if (action == "sll" || action == "SLL")	funct3 = "001";
	else if (action == "slt" || action == "SLT")	funct3 = "010";
	else if (action == "sltu" || action == "SLTU")	funct3 = "011";
	else if (action == "xor" || action == "XOR")	funct3 = "100";
	else if (action == "srl" || action == "sra" || action == "SRL" || action == "SRA")	funct3 = "101";
	else if (action == "or" || action == "OR")	funct3 = "110";
	else if (action == "and" || action == "AND")	funct3 = "111";
	
	
	while (!code.empty()) {
		pos = code.find(",");

		token = code.substr(0, pos);

		//* for output *//
		switch (counter) {
		case 1:
			token.erase(0, 1); //erase 'x'
			rd = bitset<5>(stoi(token)).to_string();
			break;
		case 2:
			rs1 = bitset<5>(stoi(token)).to_string();
			break;
		default:
			rs2 = bitset<5>(stoi(token)).to_string();
			code.erase(0, token.length());
			break;
		}

		//if(counter)	code.erase(0, code.find(delimiter) + 1);  //since the length of delimiter is 1
		code.erase(0, code.find(delimiter) + 2); //since the length of ",x" is 2
		counter++;
	}

	cout << "Machine code : " << funct5 << ' ' << funct2 << ' ' << rs2 << ' ' << rs1 << " 000 " << rd << opcode << endl;
}

void type_sb_machine_code(string action, string code, string imm[], int current_line)
{
	int index = 0;  //store which line the label is in 
	int counter = 1;  //rs1, rs2, offset : 1,2,3
	string delimiter = ",";
	size_t pos;
	string token;
	string rs2, rs1, offset;  //output
	string funct3 = "000", opcode = "1100011";

	//* funct3 *//
	if (action == "bne") funct3 = "001";
	else if (action == "blt") funct3 = "100";
	else if (action == "bge") funct3 = "101";
	else if (action == "bltu") funct3 = "110";
	else if (action == "bgeu") funct3 = "111";

	while (!code.empty()) {
		pos = code.find(",");

		token = code.substr(0, pos);

		//* for output *//
		switch (counter) {
		case 1:
			token.erase(0, 1); //erase 'x'
			rs1 = bitset<5>(stoi(token)).to_string();
			break;
		case 2:
			token.erase(0, 1); //erase 'x'
			rs2 = bitset<5>(stoi(token)).to_string();
			break;
		default:
			index = find_string_target(imm, token);
			offset = bitset<12>(index + current_line).to_string();
			code.erase(0, token.length());
			if (action == "srai")	offset[1] = '1';
			break;
		}

		//if(counter)	code.erase(0, code.find(delimiter) + 1);  //since the length of delimiter is 1
		code.erase(0, code.find(delimiter) + 1); //since the length of ",x" is 2
		counter++;
	}

	cout << "Type SB\nMachine code : ";  
	cout << offset[11] << ' ';
	for (int i = 5; i < 11; i++)
		cout << offset[i];
	cout << ' ' << rs2 << ' ' << rs1 << ' ' << funct3 << ' ';
	for (int i = 0; i < 4; i++)
		cout << offset[i];
	cout << ' ' << offset[10] << ' ' << opcode << endl;
}

void type_uj_machine_code(string action, string code )
{
	int counter = 1;
	string delimiter = ",";
	size_t pos;
	string token;  //store part of code before every delimiter
	string rd, offset;  //output
	string opcode = "1101111";

	while (!code.empty()) {
		pos = code.find(",");

		token = code.substr(0, pos);

		//* for output *//
		switch (counter) {
		case 1:
			token.erase(0, 1); //erase 'x'
			rd = bitset<5>(stoi(token)).to_string();
			break;
		default:
			offset = bitset<20>(stoi(token)).to_string();
			code.erase(0, token.length());
			break;
		}

		code.erase(0, code.find(delimiter) + 1); //since the length of ",x" is 2
		counter++;
	}

	cout << "Type U\n";
	cout << "Machine code : " << offset << ' ' << rd << ' ' << opcode << endl;
}

void type_u_machine_code(string action, string code)  //ok
{
	int counter = 1;
	string delimiter = ",";
	size_t pos;
	string token;  //store part of code before every delimiter
	string rd, offset;  //output
	string opcode = "0110111";
	
	if (action == "auipc") opcode == "0010111";

	while (!code.empty()) {
		pos = code.find(",");

		token = code.substr(0, pos);

		//* for output *//
		switch (counter) {
		case 1:
			token.erase(0, 1); //erase 'x'
			rd = bitset<5>(stoi(token)).to_string();
			break;
		default:
			offset = bitset<20>(stoi(token)).to_string();
			code.erase(0, token.length());
			break;
		}

		code.erase(0, code.find(delimiter) + 1); //since the length of ",x" is 2
		counter++;
	}

	cout << "Type U\n";  
	cout << "Machine code : " << offset << ' ' << rd << ' ' << opcode << endl;
}

int find_string_target(string resource[], string target)
{
	for (int i = 0; i < 100; i++)
		if (resource[i] == target)
			return i;
}