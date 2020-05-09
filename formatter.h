#include "tinyxml2.h"
#include "stdlib.h"
#include <string>
#include <vector>
#include <utility>

#define Tiny tinyxml2 
#define GET_VARIABLE_NAME(var) (#var)

class Formatter
{
public:
	Formatter();
	void CreateXmlFromList(std::string RootName, std::vector<std::string> Tags, std::vector<int> Values);
	std::string GetStringFromCurrentDoc();
	std::pair<std::string, int> CreatePairFromValue(int value);

private:
	Tiny::XMLDocument Doc;
};

