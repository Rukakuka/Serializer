#include "formatter.h"

Formatter::Formatter() 
{

}	

void Formatter::CreateXmlFromList(std::string RootName, std::vector<std::string> Tags, std::vector<int> Values)
{
	if (Tags.empty() || Values.empty() || RootName.empty())
	{
		throw "Empty input";
	}
	if (Tags.size() != Values.size())
	{
		throw "Sizes not match";
	}
	Doc.Clear();
	Tiny::XMLElement* root = Doc.NewElement(RootName.c_str());
	Doc.InsertFirstChild(root);	

	for (int i = 0; i < Tags.size(); i++)
	{
		const char* tag = Tags[i].c_str();
		Tiny::XMLElement* element = Doc.NewElement(Tags[i].c_str());
		element->SetText(std::to_string(Values[i]).c_str());
		root->InsertEndChild(element);
	}
}

std::string Formatter::GetStringFromCurrentDoc()
{
	Tiny::XMLNode* root = Doc.FirstChild();
	if (root == nullptr)
	{
		throw "No root";
	}
	Tiny::XMLPrinter printer;
	root->Accept(&printer);
	return printer.CStr();
}

std::pair<std::string, int> Formatter::CreatePairFromValue(int value)
{
	const char* var_name = GET_VARIABLE_NAME(value);
	std::pair<std::string, int> pair = { var_name, value };
	std::cout << pair.first << pair.second << std::endl;
	return pair;
}
