// Serializer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "serializer.h"
#include "formatter.h"
int main()
{
	std::vector<std::string> Tags(2);
	Tags[0] = "dummy1";
	Tags[1] = "dummy2";
	std::vector<int> Values(2);
	Values[0] = 4;
	Values[1] = 5;

	Formatter f;
	f.CreateXmlFromList("shit", Tags, Values);

	std::string s = f.GetStringFromCurrentDoc();
	std::cout << s.c_str() << std::endl;

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
