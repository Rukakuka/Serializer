#include "xmlmessage.h"

XmlMessage::XmlMessage() 
{

}	

string XmlMessage::EmbraceOpen(string value)
{
    if (value.empty())
    {
        return "<>";
    }
    return "<" + value + ">";
}

string XmlMessage::EmbraceClose(string value)
{
    if (value.empty())
    {
        return "</>";
    }
    return "</" + value + ">";
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
    Childrens.insert(std::pair<string,string>(Embrace(tag), ""));
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
    Childrens.insert(std::pair<string,string>(Embrace(tag), value));
}


