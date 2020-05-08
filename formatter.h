
#include "tinyxml2.h"
#include <string>
#include <vector>
#include <iostream>
#define Tiny tinyxml2 

class Formatter
{
public:
	Formatter();
	void CreateXmlFromList(std::string RootName, std::vector<std::string> Tags, std::vector<int> Values);
	std::string GetStringFromCurrentDoc();
private:
	Tiny::XMLDocument Doc;
};

