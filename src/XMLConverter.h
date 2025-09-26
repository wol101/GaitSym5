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

    int loadBaseXMLFile(const char *filename);
    int loadBaseXMLString(const char *dataPtr, size_t length);
    int applyGenome(const std::vector<double> &genomeData);
    void getFormattedXML(std::string *formattedXML);

    const std::string &baseXMLString() const;

    void clear();

private:

    std::string m_baseXMLString;
    std::vector<std::string> m_smartSubstitutionTextComponents;
    std::vector<std::string> m_smartSubstitutionParserText;
    std::vector<double> m_smartSubstitutionValues;
    size_t m_smartSubstitutionTextComponentsSize = 0;
};

}

#endif
