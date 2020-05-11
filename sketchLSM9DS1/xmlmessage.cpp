#include "xmlmessage.h"

XmlMessage::XmlMessage() 
{
    Message.reserve(250);
}	

string XmlMessage::EmbraceOpen(string tagname)
{
    if (tagname.empty())
    {
        return "<>";
    }
    return "<" + tagname + ">";
}

string XmlMessage::EmbraceClose(string tagname)
{
    if (tagname.empty())
    {
        return "</>";
    }
    return "</" + tagname + ">";
}

void XmlMessage::SetRoot(string rootName)
{
    if (rootName.empty())
    {
        return;
    }
    Root = rootName;
}


void XmlMessage::AddTag(string tag)
{
    if (tag.empty())
    {
        return;
    }
    Childrens.insert(std::pair<string,string>(tag, ""));
}

void XmlMessage::AddTag(string tag, string value)
{
    if (tag.empty())
    {
        return;
    }
    if (value.empty())
    {
        value = "0";
    }
    Childrens.insert(std::pair<string,string>(tag, value));
}

void XmlMessage::SetValueByTag(string tag, string value)
{
    if (tag.empty())
    {
        return;
    }
    if (value.empty())
    {
        value = "0";
    }
    if (Childrens.find(tag) == Childrens.end())
    {
        return;
    }
    Childrens[tag] = value;
}

string XmlMessage::GetValueByTag(string tag)
{
     if (tag.empty() || Childrens.find(tag) == Childrens.end())
    {
        return "";
    }
    return Childrens[tag];
}

void XmlMessage::Clear()
{ 
	Childrens.clear();
};

int XmlMessage::Count()
{
    return Childrens.size();        
}

string XmlMessage::GetString()
{
    Message.clear();
    if (!Root.empty())
    {
        Root = "root";
    }
    Message += EmbraceOpen(Root) + "\n";
    std::map<string, string>::iterator it = Childrens.begin();
    while (it != Childrens.end())
    {
        Message += "\t" + EmbraceOpen(it->first) + it->second + EmbraceClose(it->first) + "\n";
        it++;
    }
    Message += EmbraceClose(Root);
    return Message;
}