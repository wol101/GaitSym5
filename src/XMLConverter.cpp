/*
 *  XMLConverter.cc
 *  GA
 *
 *  Created by Bill Sellers on Fri Dec 12 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#include "XMLConverter.h"
#include "DataFile.h"
#include "GSUtil.h"

#include "pystring.h"
#include "pocketpy.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std::string_literals;
namespace GaitSym {

XMLConverter::XMLConverter()
{
    m_pythonVM =  new pkpy::VM();
}

XMLConverter::~XMLConverter()
{
    if (m_pythonVM) delete m_pythonVM;
}

// load the base file for smart substitution file
int XMLConverter::LoadBaseXMLFile(const char *filename)
{
    DataFile smartSubstitutionBaseXMLFile;
    if (smartSubstitutionBaseXMLFile.ReadFile(filename)) return 1;
    LoadBaseXMLString(smartSubstitutionBaseXMLFile.GetRawData(), smartSubstitutionBaseXMLFile.GetSize());
    return 0;
}

void XMLConverter::Clear()
{
    m_SmartSubstitutionTextComponents.clear();
    m_SmartSubstitutionParserText.clear();
    m_SmartSubstitutionValues.clear();
    m_BaseXMLString.clear();
}

// load the base XML for smart substitution file
int XMLConverter::LoadBaseXMLString(const char *dataPtr, size_t length)
{
    m_SmartSubstitutionTextComponents.clear();
    m_SmartSubstitutionParserText.clear();
    m_SmartSubstitutionValues.clear();
    m_BaseXMLString.assign(dataPtr, length);

    const char *ptr1 = dataPtr;
    const char *ptr2 = strstr(ptr1, "{{");
    m_SmartSubstitutionTextComponentsSize = 0;
    while (ptr2)
    {
        std::string s(ptr1, static_cast<size_t>(ptr2 - ptr1));
        m_SmartSubstitutionTextComponentsSize += s.size();
        m_SmartSubstitutionTextComponents.push_back(std::move(s));

        ptr2 += 2;
        ptr1 = strstr(ptr2, "}}");
        if (ptr1 == nullptr)
        {
            std::cerr << "Error: could not find matching }}\n";
            exit(1);
        }
        std::string expressionParserText(ptr2, static_cast<size_t>(ptr1 - ptr2));
        m_SmartSubstitutionParserText.push_back(std::move(expressionParserText));
        m_SmartSubstitutionValues.push_back(0); // dummy values
        ptr1 += 2;
        ptr2 = strstr(ptr1, "{{");
    }
    std::string s(ptr1);
    m_SmartSubstitutionTextComponentsSize += s.size();
    m_SmartSubstitutionTextComponents.push_back(std::move(s));

    return 0;
}

void XMLConverter::GetFormattedXML(std::string *formattedXML)
{
    formattedXML->clear();
    formattedXML->reserve(m_SmartSubstitutionTextComponentsSize + 32 * m_SmartSubstitutionValues.size());
    char buffer[32];
    for (size_t i = 0; i < m_SmartSubstitutionValues.size(); i++)
    {
        formattedXML->append(m_SmartSubstitutionTextComponents[i]);
        int l = snprintf(buffer, sizeof(buffer), "%.18g", m_SmartSubstitutionValues[i]);
        formattedXML->append(buffer, l);
    }
    formattedXML->append(m_SmartSubstitutionTextComponents[m_SmartSubstitutionValues.size()]);
}

// this needs to be customised depending on how the genome interacts with
// the XML file specifying the simulation
int XMLConverter::ApplyGenome(const std::vector<double> &genomeData)
{
    std::vector<std::string> stringList;
    stringList.reserve(genomeData.size());
    for (auto &&x : genomeData) { stringList.push_back(GSUtil::ToString(x)); }
    std::string pythonString = "g=["s + pystring::join(","s, stringList) + "]"s;
    m_pythonVM->exec(pythonString);
    for (size_t i = 0; i < m_SmartSubstitutionParserText.size(); i++)
    {
        pkpy::PyObject *result = m_pythonVM->eval(m_SmartSubstitutionParserText[i]);
        m_SmartSubstitutionValues[i] = pkpy::py_cast<double>(m_pythonVM, result);
    }

    return 0;
}

const std::string &XMLConverter::BaseXMLString() const
{
    return m_BaseXMLString;
}


} // namespace GaitSym


