// Assembler4104056004.cpp : 定義主控台應用程式的進入點。
//
/*
系級:資工二
學號:4104056004
姓名:李家駿

Program:The Assembler for SIC
*/
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <math.h>
using namespace std;
/*global variables*/
map<string, int> optable;
map<string, int> symtable;
/*functions decalaration*/
void buildOPTable();
void insertSymbolTable(string, int);
void outputToFile(fstream,int, int, string, string, string);
void showTable(map<string,int>);
int hexToInt(int hex);
int main()
{
	/*main() local variables*/
	fstream sourceFile;  //the source file
	fstream tempFile;    //the temporary file
	fstream objectFile;	 //the object file
	string fileName;     //file name
	string linetmp;      //temp for line
	string label, op, operand, tmp,objectcode;
	string programName;
	stringstream ss;
	int loc = 0, line = 5,programLength=0,start;
	/*Enter the file name and read the file*/
	//cout << "請輸入檔案名稱:";
	//cin >> fileName;
	fileName = "ExampleCode.txt";
	sourceFile.open(fileName, ios::in); //open and read the source file

	if (!sourceFile)
	{
		cout << "Source file does not exist.\n";
		//if file does not exist,end the program.
		exit(0);
	}

	/*buile the op table*/
	buildOPTable();

	/*open the temporary file*/
	tempFile.open("tmpfile.txt", ios::out);

	tempFile<<"Line\tLoc\t\tSource Statement\t\tObject code\n";//Titles
	/*read the first line*/
	getline(sourceFile, linetmp);
	ss << linetmp;
	//get the program name and the start location
	getline(ss, label, '\t');
	getline(ss, op, '\t');
	getline(ss, operand, '\t');
	//cout << label << " " << op << " " << operand << "\n";
	programName = "H" + label;
	ss.str("");
	ss.clear();
	tempFile << line << "\t\t";
	if (op == "START")
	{
		ss << operand;//string to int(hex)
		ss >> loc;
		loc = hexToInt(loc); //hex to int(dec)
		//cout << hex << loc << endl;
		start = loc;
		//output to file
		//outputToFile(tempFile,line, loc, label, op, operand);
	}
	else
	{
		//start from 0
		loc = 0;
		//outputToFile(tempFile, line, loc, label, op, operand);
	}
	line += 5;
	tempFile << hex << uppercase << loc << "\t" << linetmp << "\n";
	tempFile << dec << line << "\t\t";
	while (!sourceFile.eof())
	{
		/*scan all lines in sourceFile*/
		ss.str("");		//init sstream
		ss.clear();
		/*get a line*/
		getline(sourceFile, linetmp);
		if (linetmp[0] == '.')//comment
		{
			tempFile << "COMMENT\n";
			line += 5;
			tempFile << line << "\t\t";
		}
		else
		{
			ss << linetmp;//put into sstream to cut
			getline(ss, tmp, '\t');
			label = tmp;
			do
				getline(ss, tmp, '\t');
			while (tmp == "");
			op = tmp;
			do
				getline(ss, tmp, '\t');
			while (tmp == "");
			operand = tmp;

			if (op == "END") //end of code
			{
				tempFile << "\t" <<linetmp << "\n";
				break;
			}

			tempFile << hex << uppercase << loc << "\t" << linetmp << "\n";
			line += 5;
			tempFile << dec << line << "\t\t";

			if (label != " ")
			{
				/*If there is a label,check if it is in the symtable.*/
				if (symtable.find("label") != symtable.end())
				{
					/*found in symbol table*/
					cout << "Error:label重複\n";
					exit(0);
				}
				else
				{
					/*insert into symbol table*/
					insertSymbolTable(label, loc);
				}
			}
			if (optable.find(op) != optable.end())
			{
				/*opcode is found in optable*/
				loc += 3;
			}
			else if (op == "WORD")
			{
				loc += 3;
			}
			else if (op == "RESW")
			{
				loc += 3 * hexToInt(atoi(operand.c_str()));
			}
			else if (op == "RESB")
			{
				loc+= atoi(operand.c_str());
			}
			else if (op == "BYTE")
			{
				if (operand[0] == 'C')
				{
					loc += operand.length() - 3;
				}
				else if (operand[0] == 'X')
				{
					loc += (operand.length() - 3) / 2;
				}
				else
				{
					loc += 1;
				}
			}
			else
			{
				/*opcode is not found in optable,
				which means the operation is invalid.*/
				cout << "In line "<< line <<" Error:operation "<< op <<" doesn't exist\n";
				exit(0);
			}
			/*tempFile << label << endl;
			tempFile << op << endl;
			tempFile<< operand << endl;*/
		}
	}
	/*calculate program length*/
	programLength = loc - start;
	/*close the file streams*/
	sourceFile.close();
	tempFile.close();
	objectFile.close();
	cout << "show op table:\n";
	showTable(optable);
	cout << "show symbol table:\n";
	showTable(symtable);

    return 0;
}
void buildOPTable()
{
	/*create the hash optable*/
	optable["ADD"] = 0X18;
	optable["AND"] = 0X40;
	optable["COMP"] = 0X28;
	optable["DIV"] = 0X24;
	optable["J"] = 0X3C;
	optable["JEQ"] = 0X30;
	optable["JGT"] = 0X34;
	optable["JLT"] = 0X34;
	optable["JSUB"] = 0X48;
	optable["LDA"] = 0X00;
	optable["LDCH"] = 0X50;
	optable["LDL"] = 0X08;
	optable["LDX"] = 0X04;
	optable["MUL"] = 0X20;
	optable["OR"] = 0X44;
	optable["RD"] = 0XD8;
	optable["RSUB"] = 0X4C;
	optable["STA"] = 0X0C;
	optable["STCH"] = 0X54;
	optable["STL"] = 0X14;
	optable["STSW"] = 0XE8;
	optable["STX"] = 0X10;
	optable["SUB"] = 0X1C;
	optable["TD"] = 0XE0;
	optable["TIX"] = 0X2C;
	optable["WD"] = 0XDC;
}
void insertSymbolTable(string label, int loc)
{
	symtable[label] = loc;
}
void outputToFile(fstream f,int line,int loc ,string label,string op,string operand)
{
	f << line << "\t" << loc << "\t" << label << "\t "<< op << "\t" << operand << "\n";
}
void showTable(map<string, int>mmap)
{
	map<string, int>::iterator it;
	for (it = mmap.begin(); it != mmap.end(); ++it) {
		cout << it->first << " => " << hex << uppercase <<it->second << endl;
	}
}
int hexToInt(int hex)
{
	int i, tmp = 0,result=0,j;
	for (i = 10,j=0; i <= hex*10; i *= 10,j++)
	{
		tmp = (hex%i)/(i/10);
		result += tmp*pow(16, j);
		hex -= hex % i;
	}
	return result;
}