
#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdlib.h> 

using namespace std;

#define GET_VARIABLE_NAME(var) (#var)

class XmlMessage
{
private:
	string Message;
	string Root;
	std::map<string, string> Childrens;
	string EmbraceOpen(string tagname);
	string EmbraceClose(string tagname);

public:
	XmlMessage();

	void SetRoot(string rootName);
	void SetValueByTag(string tag, string value);
	void AddTag(string tag, string value);
	void AddTag(string tag);
	void Clear();

	string GetValueByTag(string tag);
	string GetString();
	int Count();

	template<typename T>
	std::string toString(const T& value)
	{
   		std::ostringstream oss;
   		oss << value;
   		return oss.str();
	};
};