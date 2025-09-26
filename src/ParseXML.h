/*
 *  ParseXML.cpp
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 28/04/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#ifndef PARSEXML_H
#define PARSEXML_H

#include "NamedObject.h"
#include "rapidxml.hpp"

#include <string>
#include <vector>
#include <memory>

namespace GaitSym
{

class ParseXML : NamedObject
{
public:
    ParseXML();

    struct XMLElement
    {
        std::string tag;
        std::map<std::string, std::string> attributes;
    };

    void addElement(const std::string &tag, const std::map<std::string, std::string> &attributeList);

    std::string *loadModel(const char *buffer, size_t length, std::string *rootNodeTag);
    std::string saveModel(const std::string &rootNodeTag, const std::string &comment);

    std::vector<std::unique_ptr<XMLElement>> *elementList();

private:
    rapidxml::xml_attribute<char> *createXMLAttribute(rapidxml::xml_node<char> *cur, const std::string &name, const std::string &newValue, bool sorted);
    rapidxml::xml_node<char> *createXMLNode(rapidxml::xml_node<char> *parent, const std::string &name);
    rapidxml::xml_node<char> *createXMLNode(rapidxml::xml_node<char> *parent, const std::string &name, const std::string newValue);
    bool removeXMLAttribute(rapidxml::xml_node<char> *cur, const std::string &name, bool caseSensitive);
    rapidxml::xml_attribute<char> *findXMLAttribute(rapidxml::xml_node<char> *cur, const std::string &name, bool caseSensitive);
    rapidxml::xml_attribute<char> *getXMLAttribute(rapidxml::xml_node<char> *cur, const std::string &name, std::string *attributeValue, bool caseSensitive);

    rapidxml::xml_document<char> m_inputConfigDoc;
    rapidxml::xml_document<char> m_ouputConfigDoc;
    std::vector<char> m_inputConfigData;
    std::vector<std::unique_ptr<XMLElement>> m_elementList;
};

}

#endif // PARSEXML_H
