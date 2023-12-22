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

class Genome;
class DataFile;
class ExpressionParser;
namespace pkpy { class VM; }

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

    std::string m_BaseXMLString;
    std::vector<std::string> m_SmartSubstitutionTextComponents;
    std::vector<std::string> m_SmartSubstitutionParserText;
    std::vector<double> m_SmartSubstitutionValues;
    size_t m_SmartSubstitutionTextComponentsSize = 0;
    pkpy::VM *m_pythonVM = nullptr;
};


#endif
