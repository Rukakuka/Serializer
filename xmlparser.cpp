#include "xmlparser.h"

Xmlparser::Xmlparser() 
{
	std::vector<std::string> Tags(2);
	Tags[0] = "dummy1";
	Tags[1] = "dummy2";
	std::vector<int> Values(2);
	Values[0] = 4;
	Values[1] = 5;
	CreateXmlString(Tags, Values);
}


std::string Xmlparser::CreateXmlString(std::vector<std::string> Tags, std::vector<int> Values)
{
	Tiny::XMLDocument doc;
	Tiny::XMLElement* root = doc.NewElement("SerialDataTemplate");
	doc.InsertFirstChild(root);	

	for (int i = 0; i < Tags.size(); i++)
	{
		const char* tag = Tags[i].c_str();
		Tiny::XMLElement* element = doc.NewElement(Tags[i].c_str());
		element->SetText(std::to_string(Values[i]).c_str());
		root->InsertEndChild(element);
	}
	doc.SaveFile("d.xml");
	return std::string();
}
