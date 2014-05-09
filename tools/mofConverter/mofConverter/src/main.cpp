/*
*(C)2011 Symplatonix Software
*/

#include <iostream>
#include <string>
#include "converter.h"

#pragma comment(lib, "assimp.lib")

using namespace std;

int main(int argc, char **argv)
{
	cout<<"MOF converter. Convert 3d models to MOF format"<<endl;
	cout<<"Symplatonix Software"<<endl<<endl;

	string sourceFile;
	string destinationFile;

	if(argc>3)
	{
		cout<<"usage1: mofconverter.exe"<<endl;
		cout<<"usage2: mofconverter.exe sourceFile destinationFile"<<endl;

		getchar();
		return 1;
	}
	else if(argc<3)
	{
		cout<<"source file: ";
		getline(cin, sourceFile);

		cout<<"destination file: ";
		getline(cin, destinationFile);

	}
	else
	{
		sourceFile = argv[1];
		destinationFile = argv[2];
	}

	//char *filename = new char[50];
	//strncpy_s(filename, 50, "..\\data\\box.obj",20);

	Converter Converter;
	if(Converter.read(sourceFile.c_str())!=0)
	{
		cout<<"error loading "<<sourceFile<<endl;
		getchar();
		return -1;
	}
	else
		cout<<"loaded file "<<sourceFile<<endl;

	if(Converter.convertToMof(destinationFile.c_str())!=0)
	{
		cout<<"error loading "<<destinationFile<<endl;
		getchar();
		return -1;
	}
	else
		cout<<"converted to: "<<destinationFile<<endl;

	getchar();

	return 0;
}
