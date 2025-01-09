/*
 *  XMLConverter.h
 *  GA
 *
 *  Created by Bill Sellers on Fri Dec 12 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#ifndef XMLConverter_h
#define XMLConverter_h

#include <vector>
#include <string>

namespace GaitSym
{

class Genome;
class DataFile;
class ExpressionParser;

class XMLConverter
{
public:
    XMLConverter();
    virtual ~XMLConverter();

    int LoadBaseXMLFile(const char *filename);
    int LoadBaseXMLString(const char *dataPtr, size_t length);
    int ApplyGenome(const std::vector<double> &genomeData);
    void GetFormattedXML(std::string *formattedXML);

    const std::string &BaseXMLString() const;

    void Clear();

private:

    std::string m_baseXMLString;
    std::vector<std::string> m_smartSubstitutionTextComponents;
    std::vector<std::string> m_smartSubstitutionParserText;
    std::vector<double> m_smartSubstitutionValues;
    size_t m_smartSubstitutionTextComponentsSize = 0;
};

}

#endif
