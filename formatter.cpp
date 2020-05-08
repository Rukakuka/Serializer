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
	// TO REMOVE
	Doc.SaveFile("d.xml");
	// /TO REMOVE
}

std::string Formatter::GetStringFromCurrentDoc()
{
	Tiny::XMLNode* root = Doc.FirstChild();
	if (root == nullptr)
	{
		throw "No root";
	}
	int a;
	Tiny::XMLPrinter printer;
	root->Accept(&printer);
	std::string res = printer.CStr();
	return res;
}

