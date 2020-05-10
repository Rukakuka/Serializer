
#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

#define GET_VARIABLE_NAME(var) (#var)

class XmlMessage
{
private:
	string Message;
	string Root;
	std::map<string, string> Childrens;
	string EmbraceOpen(string value);
	string EmbraceClose(string value);
	void CreateString();

public:
	XmlMessage();
	void SetRoot(string rootName);
	void SetTagByName(string tag, string value) {};
	void AddTag(string tag, string value);
	void AddTag(string tag);
	void Clear() {};

	template<typename T>
	std::string toString(const T& value)
	{
   		std::ostringstream oss;
   		oss << value;
   		return oss.str();
	};

	string GetValueByTag() {return "";};
	string GetString() { return "";};
};