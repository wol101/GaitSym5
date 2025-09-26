/*
 *  DataFile.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// DataFile.h - utility class to read in various sorts of data files

#ifndef DataFile_h
#define DataFile_h

#include <stddef.h>
#include <string>

namespace GaitSym
{

class DataFile
{
public:
    DataFile();
    virtual ~DataFile();

    // initialise by reading the file
    // this routine allocates a buffer and reads in the whole file
    // filenames are assumed to be UTF-8 and internal conversion happens
    // for Windows where the file system needs wide characters
    bool readFile(const std::string &name);
    // write the data out to a file
    bool writeFile(const std::string &name, bool binary = false);

    // retrieve basic types
    // the file is searched for the parameter and then the next token is read
    bool retrieveParameter(const char * const param, int *val, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, unsigned int *val, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, double *val, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, bool *val, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, char *val, size_t size, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, char **val, size_t *size, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, std::string *val, bool searchFromStart = true);
    bool retrieveQuotedStringParameter(const char * const param, char *val, size_t size, bool searchFromStart = true);
    bool retrieveQuotedStringParameter(const char * const param, char **val, size_t *size, bool searchFromStart = true);
    bool retrieveQuotedStringParameter(const char * const param, std::string *val, bool searchFromStart = true);

    // ranged functions
    // the file is searched for the parameter and then the next token is read
    bool retrieveRangedParameter(const char * const param, double *val, bool searchFromStart = true);
    void setRangeControl(double r) { m_rangeControl = r; }

    // line reading functions
    // optional comment character and can ignore empty lines
    bool readNextLine(char *line, size_t size, bool ignoreEmpty, char commentChar = 0, char continuationChar = 0);
    bool readNextLine2(char *line, size_t size, bool ignoreEmpty, const char *commentString, const char *continuationString = nullptr);

    // retrieve arrays
    // the file is searched for the parameter and then the next token is read
    bool retrieveParameter(const char * const param, size_t n, int *val, bool searchFromStart = true);
    bool retrieveParameter(const char * const param, size_t n, double *val, bool searchFromStart = true);
    bool retrieveRangedParameter(const char * const param, size_t n, double *val, bool searchFromStart = true);

    // utility settings
    void setExitOnErrorFlag(bool flag) { m_exitOnErrorFlag = flag; }
    bool exitOnErrorFlag() { return m_exitOnErrorFlag; }
    char *rawData() { return m_fileData; }
    void setRawData(const char *string, size_t stringLen);
    void resetIndex() { m_index = m_fileData; }
    size_t size() { return m_size; }
    void clear();
    size_t replace(const char *oldString, const char *newString);
    char *index() { return m_index; }
    void setIndex(char *index) { m_index = index; }
    std::string pathName() { return m_pathName; }

    // probably mostly for internal use
    // read the next ASCII token from the current index and bump index
    bool findParameter(const char * const param, bool searchFromStart = true);
    bool readNext(int *val);
    bool readNext(double *val);
    bool readNext(char *val, size_t size);
    bool readNext(char **val, size_t *size);
    bool readNext(std::string *val);
    bool readNextQuotedString(char *val, size_t size);
    bool readNextQuotedString(char **val, size_t *size);
    bool readNextQuotedString(std::string *val);
    bool readNextLine(char *line, size_t size);
    bool readNextRanged(double *val);

    // handy statics
    static bool endOfLineTest(char **p);
    static size_t countTokens(const char *string);
    static size_t returnTokens(char *string, char *ptrs[], size_t size);
    static size_t countTokens(const char *string, const char *separators);
    static size_t returnTokens(char *string, char *ptrs[], size_t size, const char *separators);
    static size_t countLines(const char *string);
    static size_t returnLines(char *string, char *ptrs[], size_t size);
    static void strip(char *str);
    static bool stringEndsWith(const char *str, const char *suffix);
    static bool stringStartsWith(const char *str, const char *prefix);
    static double toDouble(const char *buf);
    static void toDouble(const char *buf, size_t n, double *d);
    static int toInt(const char *buf);
    static void toInt(const char *buf, size_t n, int *d);
    static bool toBool(const char *buf);

    static std::wstring convertUTF8ToWide(const std::string& str);
    static std::string convertWideToUTF8(const std::wstring& wstr);

    // binary file operators
    // read the next binary value from current index and bump index
    bool readNextBinary(int *val);
    bool readNextBinary(float *val);
    bool readNextBinary(double *val);
    bool readNextBinary(char *val);
    bool readNextBinary(bool *val);
    bool readNextBinary(int *val, size_t n);
    bool readNextBinary(float *val, size_t n);
    bool readNextBinary(double *val, size_t n);
    bool readNextBinary(char *val, size_t n);
    bool readNextBinary(bool *val, size_t n);

    // file writing operators
    bool writeParameter(const char * const param, int val);
    bool writeParameter(const char * const param, double val);
    bool writeParameter(const char * const param, bool val);
    bool writeParameter(const char * const param, const char * const val);
    bool writeQuotedStringParameter(const char * const param, const char * const val);
    bool writeParameter(const char * const param, size_t n, int *val);
    bool writeParameter(const char * const param, size_t n, double *val);
    bool writeParameter(const char * const param, size_t n, bool *val);
    bool writeNext(int val, char after = 0);
    bool writeNext(double val, char after = 0);
    bool writeNext(bool val, char after = 0);
    bool writeNext(const char * const val, char after = 0);
    bool writeNextQuotedString(const char * const val, char after = 0);

private:

    char *m_fileData = nullptr;
    char *m_index = nullptr;
    bool m_exitOnErrorFlag = false;
    double m_rangeControl = false;
    size_t m_size = 0;
    std::string m_pathName;
#if defined(_WIN32) || defined(WIN32)
    // provide Windows specific wchar versions
    bool readFile(const std::wstring &name);
    bool writeFile(const std::wstring &name, bool binary = false);
    std::wstring wPathName() { return m_wPathName; }
    std::wstring m_wPathName;
#endif
};

}

#endif

