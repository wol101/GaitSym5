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
    py_resetvm();
    bool ok = py_exec("import math", "<string>", EXEC_MODE, NULL);
    if (!ok) std::cerr << "Error in XMLConverter.cpp Line = " << __LINE__ << "\n";
}

XMLConverter::~XMLConverter()
{
    py_resetvm();
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
    m_smartSubstitutionTextComponents.clear();
    m_smartSubstitutionParserText.clear();
    m_smartSubstitutionValues.clear();
    m_baseXMLString.clear();
}

// load the base XML for smart substitution file
int XMLConverter::LoadBaseXMLString(const char *dataPtr, size_t length)
{
    m_smartSubstitutionTextComponents.clear();
    m_smartSubstitutionParserText.clear();
    m_smartSubstitutionValues.clear();
    m_baseXMLString.assign(dataPtr, length);

    const char *ptr1 = dataPtr;
    const char *ptr2 = strstr(ptr1, "{{");
    m_smartSubstitutionTextComponentsSize = 0;
    while (ptr2)
    {
        std::string s(ptr1, static_cast<size_t>(ptr2 - ptr1));
        m_smartSubstitutionTextComponentsSize += s.size();
        m_smartSubstitutionTextComponents.push_back(std::move(s));

        ptr2 += 2;
        ptr1 = strstr(ptr2, "}}");
        if (ptr1 == nullptr)
        {
            std::cerr << "Error: could not find matching }}\n";
            exit(1);
        }
        std::string expressionParserText(ptr2, static_cast<size_t>(ptr1 - ptr2));
        m_smartSubstitutionParserText.push_back(std::move(expressionParserText));
        m_smartSubstitutionValues.push_back(0); // dummy values
        ptr1 += 2;
        ptr2 = strstr(ptr1, "{{");
    }
    std::string s(ptr1);
    m_smartSubstitutionTextComponentsSize += s.size();
    m_smartSubstitutionTextComponents.push_back(std::move(s));

    return 0;
}

void XMLConverter::GetFormattedXML(std::string *formattedXML)
{
    formattedXML->clear();
    formattedXML->reserve(m_smartSubstitutionTextComponentsSize + 32 * m_smartSubstitutionValues.size());
    char buffer[32];
    for (size_t i = 0; i < m_smartSubstitutionValues.size(); i++)
    {
        formattedXML->append(m_smartSubstitutionTextComponents[i]);
        int l = snprintf(buffer, sizeof(buffer), "%.17g", m_smartSubstitutionValues[i]);
        formattedXML->append(buffer, l);
    }
    formattedXML->append(m_smartSubstitutionTextComponents[m_smartSubstitutionValues.size()]);
}

// this needs to be customised depending on how the genome interacts with
// the XML file specifying the simulation
int XMLConverter::ApplyGenome(const std::vector<double> &genomeData)
{
    // slow but reliable version by creating a python command to assign the list
    std::vector<std::string> stringList;
    stringList.reserve(genomeData.size());
    for (auto &&x : genomeData) { stringList.push_back(GSUtil::ToString(x)); }
    std::string pythonString = "g=["s + pystring::join(","s, stringList) + "]"s;
    bool ok = py_exec(pythonString.c_str(), "<string>", EXEC_MODE, NULL);
    if (!ok) std::cerr << "Error in XMLConverter.cpp Line = " << __LINE__ << "\n";
    if (!ok) return __LINE__;

    // faster but I am not sure this actually works
    // Create a list: [1, 2, 3]
    // py_Ref r0 = py_getreg(0); // create on register
    // py_push(r0); // save on stack because I don't know what might effect the registers
    // py_Ref s0 = py_peek(-1); // get stack value
    // py_newlistn(r0, int(genomeData.size()));
    // for (size_t i = 0; i < genomeData.size(); ++i) { py_newfloat(py_list_getitem(s0, int(i)), genomeData[i]); }
    // py_setglobal(py_name("gg"), s0);

    // // test
    // for (size_t i = 0; i < genomeData.size(); ++i)
    // {
    //     std::string testString = "g["s + GSUtil::ToString(i) + "]==gg["s  + GSUtil::ToString(i) + "]"s;
    //     ok = py_eval(m_smartSubstitutionParserText[i].c_str(), NULL);
    //     if (!py_tobool(py_retval()))
    //     {
    //         if (!ok) std::cerr << "Error in XMLConverter.cpp Line = " << __LINE__ << "\n";
    //         return __LINE__;
    //     }
    // }

    double v = 0;
    for (size_t i = 0; i < m_smartSubstitutionParserText.size(); i++)
    {
        ok = py_eval(m_smartSubstitutionParserText[i].c_str(), NULL);
        if (!ok) std::cerr << "Error in XMLConverter.cpp Line = " << __LINE__ << "\n";
        if (!ok) return __LINE__;
        ok = py_castfloat(py_retval(), &v); // py_castfloat copes with int and float types
        if (!ok) std::cerr << "Error in XMLConverter.cpp Line = " << __LINE__ << "\n";
        if (!ok) return __LINE__;
        m_smartSubstitutionValues[i] = v;
    }

    return 0;
}

const std::string &XMLConverter::BaseXMLString() const
{
    return m_baseXMLString;
}


} // namespace GaitSym


