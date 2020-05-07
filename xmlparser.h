
#include "tinyxml2.h"
#include <string>
#include <vector>

#define Tiny tinyxml2 

class Xmlparser
{
public:
	Xmlparser();
	static std::string CreateXmlString(std::vector<std::string> Tags, std::vector<int> Values);
private:
	
};

