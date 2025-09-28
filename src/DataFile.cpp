/*
 *  DataFile.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// DataFile.cpp - utility class to read in various sorts of data files

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <locale>
#include <codecvt>

#ifdef STRINGS_H_NEEDED
#include <strings.h>
#endif

#include "DataFile.h"

#ifdef _WIN32
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#include<Windows.h>
#endif

//#ifdef USE_STRCMPI
//#define strcasecmp strcmpi
//#define strncasecmp strcmp
//#endif

const static size_t kStorageIncrement = 65536;

namespace GaitSym {

// default constructor
DataFile::DataFile()
{
}

// default destructor
DataFile::~DataFile()
{
    if (m_fileData) delete [] m_fileData;
}

// set the raw data adding a terminating 0
void DataFile::setRawData(const char *string, size_t stringLen)
{
    if (m_fileData) delete [] m_fileData;
    if (stringLen == 0) m_size = strlen(string) + 1;
    else m_size = stringLen + 1;
    m_fileData = new char [m_size];
    memcpy(m_fileData, string, m_size);
    m_fileData[stringLen] = 0;
    m_index = m_fileData;
}

void DataFile::clear()
{
    if (m_fileData) delete [] m_fileData;
    m_size = 0;
    m_fileData = nullptr;
    m_index = m_fileData;
}

// preforms a global search and replace
size_t DataFile::replace(const char *oldString, const char *newString)
{
    size_t count = 0;
    size_t oldLen = strlen(oldString);
    size_t newLen = strlen(newString);
    char *startPtr = m_fileData;
    char *foundPtr;
    char *endPtr;
    size_t size;
    char *newBuffer;
    size_t i;
    char **segment = new char *[1 + m_size / oldLen]; // bound to be big enough

    do
    {
        foundPtr = strstr(startPtr, oldString);
        if (foundPtr)
        {
            size = size_t(foundPtr - startPtr); // always positive
            segment[count] = new char[size + 1];
            memcpy(segment[count], startPtr, size);
            segment[count][size] = 0;
            count ++;
            startPtr = foundPtr + oldLen;
            endPtr = startPtr;
        }
    } while (foundPtr);

    if (count) // safe but slow version - would be quicker with memcpy and lots of string length storage
    {
        newBuffer = new char[m_size + (newLen - oldLen) * count + 1];
        *newBuffer = 0;
        for (i = 0; i < count; i++)
        {
            strcat(newBuffer, segment[i]);
            delete segment[i];
            strcat(newBuffer, newString);
        }
        strcat(newBuffer, endPtr);
        delete [] m_fileData;
        m_size = strlen(newBuffer) + 1;
        m_fileData = newBuffer;
        m_index = m_fileData;
    }

    delete [] segment;
    return count;
}

// read the named file
// returns true on error
// note if used on windows this routine calls the wide character versions after converting the string
bool DataFile::readFile(const std::string &name)
{
#if (defined(_WIN32) || defined(WIN32)) && !defined(__MINGW32__)
    m_pathName = name;
    return readFile(convertUTF8ToWide(name));
#else
    struct stat fileStat;
    FILE *in;
    size_t count = 0;
    size_t index, read_block;
    size_t max_read_block = 256LL * 256LL * 256LL * 64LL;
    int error;

    m_pathName = name;

    if (m_fileData) delete [] m_fileData;
    m_fileData = nullptr;

    error = stat(name.c_str(), &fileStat);
    if (error && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::ReadFile(" << name << ") - Cannot stat file\n";
        exit(1);
    }
    if (error) return true;
    m_fileData = new char[size_t(fileStat.st_size) + 1];
    if (m_fileData == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::ReadFile(" << name << ") - Cannot allocate m_fileData\n";
        exit(1);
    }
    if (m_fileData == nullptr) return true;
    m_index = m_fileData;
    m_size = size_t(fileStat.st_size);
    m_fileData[m_size] = 0;

    in = fopen(name.c_str(), "rb");
    if (in == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::ReadFile(" << name << ") - Cannot open file\n";
        exit(1);
    }
    if (in == nullptr) return true;
    for (index = 0; index < size_t(fileStat.st_size); index += max_read_block)
    {
        read_block = (size_t(fileStat.st_size) - index);
        if (read_block > max_read_block) read_block = max_read_block;
        count = read_block;
        count = fread(m_fileData + index, count, 1, in);
        if (count != 1 && m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::ReadFile(" << name << ") - Cannot read file\n";
            exit(1);
        }
    }
    fclose(in);

    return false;

#endif
}

// write the data to a file
// if binary is true, the whole of the buffer (excluding terminating zero) is written
// otherwise it is just the string until the first terminating zero
bool DataFile::writeFile(const std::string &name, bool binary)
{
#if (defined(_WIN32) || defined(WIN32)) && !defined(__MINGW32__)
    return writeFile(convertUTF8ToWide(name), binary);
#else
    FILE *out;
    size_t count;

    out = fopen(name.c_str(), "wb");

    if (out == nullptr)
    {
        if (m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::WriteFile(" << name << ") - Cannot open file\n";
            exit(1);
        }
        else return true;
    }

    // write file
    if (binary) count = fwrite(m_fileData, m_size - 1, 1, out);
    else count = fwrite(m_fileData, strlen(m_fileData), 1, out);

    if (count != 1)
    {
        if (m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::WriteFile(" << name << ") - Cannot write file\n";
            exit(1);
        }
        else return true;
    }

    if (fclose(out))
    {
        if (m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::WriteFile(" << name << ") - Cannot close file\n";
            exit(1);
        }
        else return true;
    }

    return false;
#endif
}

#if defined(_WIN32) || defined(WIN32)
// provide Windows specific wchar versions
// read the named file
// returns true on error
bool DataFile::readFile(const std::wstring &name)
{
    struct _stat64 fileStat;
    FILE *in;
    size_t count = 0;
    size_t index, read_block;
    size_t max_read_block = 256LL * 256LL * 256LL * 64LL;
    int error;

    m_wPathName = name;

    if (m_fileData) delete [] m_fileData;
    m_fileData = nullptr;

    error = _wstat64(name.c_str(), &fileStat);
    if (error && m_exitOnErrorFlag)
    {
        std::wcerr << L"Error: DataFile::ReadFile(" << name << L") - Cannot stat file\n";
        exit(1);
    }
    if (error) return true;
    m_fileData = new char[size_t(fileStat.st_size) + 1];
    if (m_fileData == nullptr && m_exitOnErrorFlag)
    {
        std::wcerr << L"Error: DataFile::ReadFile(" << name << L") - Cannot allocate m_fileData\n";
        exit(1);
    }
    if (m_fileData == nullptr) return true;
    m_index = m_fileData;
    m_size = size_t(fileStat.st_size);
    m_fileData[m_size] = 0;

    in = _wfopen(name.c_str(), L"rb");
    if (in == nullptr && m_exitOnErrorFlag)
    {
        std::wcerr << L"Error: DataFile::ReadFile(" << name << L") - Cannot open file\n";
        exit(1);
    }
    if (in == nullptr) return true;
    for (index = 0; index < size_t(fileStat.st_size); index += max_read_block)
    {
        read_block = (size_t(fileStat.st_size) - index);
        if (read_block > max_read_block) read_block = max_read_block;
        count = read_block;
        count = fread(m_fileData + index, count, 1, in);
        if (count != 1 && m_exitOnErrorFlag)
        {
            std::wcerr << L"Error: DataFile::ReadFile(" << name << L") - Cannot read file\n";
            exit(1);
        }
    }
    fclose(in);

    return false;
}

// write the data to a file
// if binary is true, the whole of the buffer (excluding terminating zero) is written
// otherwise it is just the string until the first terminating zero
bool DataFile::writeFile(const std::wstring &name, bool binary)
{
    FILE *out;
    size_t count;

    out = _wfopen(name.c_str(), L"wb");

    if (out == nullptr)
    {
        if (m_exitOnErrorFlag)
        {
            std::wcerr << L"Error: DataFile::WriteFile(" << name << L") - Cannot open file\n";
            exit(1);
        }
        else return true;
    }

    // write file
    if (binary) count = fwrite(m_fileData, m_size - 1, 1, out);
    else count = fwrite(m_fileData, strlen(m_fileData), 1, out);

    if (count != 1)
    {
        if (m_exitOnErrorFlag)
        {
            std::wcerr << L"Error: DataFile::WriteFile(" << name << L") - Cannot write file\n";
            exit(1);
        }
        else return true;
    }

    if (fclose(out))
    {
        if (m_exitOnErrorFlag)
        {
            std::wcerr << L"Error: DataFile::WriteFile(" << name << L") - Cannot close file\n";
            exit(1);
        }
        else return true;
    }

    return false;
}
#endif

// read an integer parameter
// returns false on success
bool DataFile::retrieveParameter(const char * const param, int *val, bool searchFromStart)
{
    char buffer[64];

    if (retrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;

    *val = strtol(buffer, nullptr, 10);

    return false;
}

// read an unsigned integer parameter
// returns false on success
bool DataFile::retrieveParameter(const char * const param, unsigned int *val, bool searchFromStart)
{
    char buffer[64];

    if (retrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;

    *val = strtoul(buffer, nullptr, 10);

    return false;
}


// read a double parameter
// returns false on success
bool DataFile::retrieveParameter(const char * const param, double *val, bool searchFromStart)
{
    char buffer[64];

    if (retrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;

    *val = strtod(buffer, nullptr);

    return false;
}

// read a bool parameter
// returns false on success
bool DataFile::retrieveParameter(const char * const param, bool *val, bool searchFromStart)
{
    char buffer[64];

    if (retrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;

    if (strcmp(buffer, "true") == 0 || strcmp(buffer, "TRUE") == 0 || strcmp(buffer, "1") == 0)
    {
        *val = true;
        return false;
    }
    if (strcmp(buffer, "false") == 0 || strcmp(buffer, "FALSE") == 0 || strcmp(buffer, "0") == 0)
    {
        *val = false;
        return false;
    }

    return true;
}

// read a string parameter - up to (size - 1) bytes
// returns false on success
bool DataFile::retrieveParameter(const char * const param, char *val, size_t size, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNext(val, size));
}

// read a string parameter as a ptr and length (no copying)
// returns false on success
bool DataFile::retrieveParameter(const char * const param, char **val, size_t *size, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNext(val, size));
}

// read a std::string parameter
// returns false on success
bool DataFile::retrieveParameter(const char * const param, std::string *val, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNext(val));
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool DataFile::retrieveQuotedStringParameter(const char * const param, char *val, size_t size, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNextQuotedString(val, size));
}

// read a quoted string parameter -as a ptr and length (no copying)
// returns false on success
bool DataFile::retrieveQuotedStringParameter(const char * const param, char **val, size_t *size, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNextQuotedString(val, size));
}

// read a quoted std::string parameter
// returns false on success
bool DataFile::retrieveQuotedStringParameter(const char * const param, std::string *val, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;

    return (readNextQuotedString(val));
}


// return a parameter selected from a range of values
bool DataFile::retrieveRangedParameter(const char * const param, double *val, bool searchFromStart)
{
    if (findParameter(param, searchFromStart)) return true;
    if (readNextRanged(val)) return true;
    return false;
}

// find a parameter and move index to just after parameter
// NB can't have whitespace in parameter (might work but not guaranteed)
// in fact there are lots of ways this can be confused
// I'm assuming that the system strstr function is more efficient
// than anything I might come up with
bool DataFile::findParameter(const char * const param,
                             bool searchFromStart)
{
    char *p;
    size_t len = strlen(param);

    if (searchFromStart) p = m_fileData;
    else p = m_index;

    while (1)
    {
        p = strstr(p, param);
        if (p == nullptr) break; // not found at all
        if (p == m_fileData) // at beginning of file
        {
            if (*(p + len) < 33) // ends with whitespace
            {
                m_index = p + len;
                return false;
            }
        }
        else
        {
            if (*(p - 1) < 33) // character before is whitespace
            {
                if (*(p + len) < 33) // ends with whitespace
                {
                    m_index = p + len;
                    return false;
                }
            }
        }
        p += len;
    }
    if (m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::FindParameter(" << param
        << " - could not find parameter\n";
        exit(1);
    }
    return true;
}

// read the next whitespace delimited token - up to (size - 1) characters
// automatically copes with quote delimited strings
bool DataFile::readNext(char *val, size_t size)
{
    size_t len = 0;

    // find non-whitespace
    while (*m_index < 33)
    {
        if (*m_index == 0 && m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::readNext no non-whitespace found\n";
            exit(1);
        }
        if (*m_index == 0) return true;
        m_index++;
    }

    if (*m_index == '\"') return readNextQuotedString(val, size);

    // copy until whitespace
    while (*m_index > 32)
    {
        *val = *m_index;
        val++;
        m_index++;
        len++;
        if (len == size - 1) break;
    }
    *val = 0;
    return false;
}

// read the next whitespace delimited token - up to (size - 1) characters
// automatically copes with quote delimited strings
// returns the start pointer and length
bool DataFile::readNext(char **val, size_t *size)
{
    *size = 0;

    // find non-whitespace
    while (*m_index < 33)
    {
        if (*m_index == 0 && m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::readNext no non-whitespace found\n";
            exit(1);
        }
        if (*m_index == 0) return true;
        m_index++;
    }

    if (*m_index == '\"') return readNextQuotedString(val, size);

    *val = m_index;
    // count until whitespace
    while (*m_index > 32)
    {
        m_index++;
        (*size)++;
    }
    return false;
}

// read the next whitespace delimited token
// automatically copes with quote delimited strings
bool DataFile::readNext(std::string *val)
{
    // find non-whitespace
    while (*m_index < 33)
    {
        if (*m_index == 0 && m_exitOnErrorFlag)
        {
            std::cerr << "Error: DataFile::readNext no non-whitespace found\n";
            exit(1);
        }
        if (*m_index == 0) return true;
        m_index++;
    }

    if (*m_index == '\"') return readNextQuotedString(val);

    // copy until whitespace
    val->clear();
    while (*m_index > 32)
    {
        *val += *m_index++;
    }
    return false;
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool DataFile::readNextQuotedString(char *val, size_t size)
{
    char *start;
    char *end;
    size_t len;

    start = strstr(m_index, "\"");
    if (start == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find opening \"\n";
        exit(1);
    }
    if (start == nullptr) return true;

    end = strstr(start + 1, "\"");
    if (end == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find closing \"\n";
        exit(1);
    }
    if (end == nullptr) return true;

    len = size_t(end - start - 1); // this is always greater than 0
    if (len >= size) len = size - 1;
    m_index = start + len + 2;
    memcpy(val, start + 1, len);
    val[len] = 0;

    return false;
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
// returns the start pointer and length
bool DataFile::readNextQuotedString(char **val, size_t *size)
{
    char *start;
    char *end;

    start = strstr(m_index, "\"");
    if (start == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find opening \"\n";
        exit(1);
    }
    if (start == nullptr) return true;

    end = strstr(start + 1, "\"");
    if (end == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find closing \"\n";
        exit(1);
    }
    if (end == nullptr) return true;

    *size = size_t(end - start - 1); // this is always greater than 0
    m_index = start + *size + 2;
    *val = start + 1;

    return false;
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool DataFile::readNextQuotedString(std::string *val)
{
    char *start;
    char *end;
    size_t len;

    start = strstr(m_index, "\"");
    if (start == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find opening \"\n";
        exit(1);
    }
    if (start == nullptr) return true;

    end = strstr(start + 1, "\"");
    if (end == nullptr && m_exitOnErrorFlag)
    {
        std::cerr << "Error: DataFile::readNextQuotedString could not find closing \"\n";
        exit(1);
    }
    if (end == nullptr) return true;

    len = size_t(end - start - 1); // this is always greater than 0
    m_index = start + len + 2;
    val->clear();
    std::copy(start + 1, start + 1 + len, std::back_inserter(*val));

    return false;
}

// read the next integer
bool DataFile::readNext(int *val)
{
    char buffer[64];

    if (readNext(buffer, sizeof(buffer))) return true;

    *val = strtol(buffer, nullptr, 10);

    return false;
}

// read the next double
bool DataFile::readNext(double *val)
{
    char buffer[64];

    if (readNext(buffer, sizeof(buffer))) return true;

    *val = strtod(buffer, nullptr);

    return false;
}

// return the next ranged parameter
bool DataFile::readNextRanged(double *val)
{
    double low, high;
    if (readNext(&low)) return true;
    if (readNext(&high)) return true;

    // m_RangeControl is normally from 0 to 1.0
    *val = low + m_rangeControl * (high - low);
    return false;
}

// read an array of ints
bool DataFile::retrieveParameter(const char * const param, size_t n, int *val, bool searchFromStart)
{
    size_t i;
    if (findParameter(param, searchFromStart)) return true;

    for (i = 0; i < n; i++)
    {
        if (readNext(&(val[i]))) return true;
    }

    return false;
}

// read an array of doubles
bool DataFile::retrieveParameter(const char * const param, size_t n, double *val, bool searchFromStart)
{
    size_t i;
    if (findParameter(param, searchFromStart)) return true;

    for (i = 0; i < n; i++)
    {
        if (readNext(&(val[i]))) return true;
    }

    return false;
}

// read an array of ranged doubles
bool DataFile::retrieveRangedParameter(const char * const param, size_t n, double *val, bool searchFromStart)
{
    size_t i;
    if (findParameter(param, searchFromStart)) return true;

    for (i = 0; i < n; i++)
    {
        if (readNextRanged(&(val[i]))) return true;
    }

    return false;
}

// read a line, optionally ignoring blank lines and comments
// (comment string to end of line)
// returns true on error
bool DataFile::readNextLine2(char *line, size_t size, bool ignoreEmpty, const char *commentString, const char *continuationString)
{
    char *c;
    bool loopFlag = true;
    bool openQuotes = false;

    while (loopFlag)
    {
        if (readNextLine(line, size)) return true;

        if (commentString)
        {
            c = line;
            while (*c)
            {
                if (*c == '"') openQuotes = !openQuotes;
                if (strncmp(c, commentString, strlen(commentString)) == 0 && openQuotes == false)
                {
                    *c = 0;
                    break;
                }
                c++;
            }
        }

        if (ignoreEmpty)
        {
            c = line;
            while (*c)
            {
                if (*c > 32)
                {
                    loopFlag = false;
                    break;
                }
                c++;
            }
        }
        else loopFlag = false;

        if (continuationString)
        {
            c = line;
            if (stringEndsWith(c, continuationString))
            {
                loopFlag = true;
                size_t len = size_t(strlen(c)) - size_t(strlen(continuationString));
                line = line + len;
                size = size - len; // always positive
            }
        }
    }
    return false;
}


// read a line, optionally ignoring blank lines and comments
// (comment string to end of line)
// returns true on error
bool DataFile::readNextLine(char *line, size_t size, bool ignoreEmpty, char commentChar, char continuationChar)
{
    char *c;
    bool loopFlag = true;
    bool openQuotes = false;

    while (loopFlag)
    {
        if (readNextLine(line, size)) return true;

        if (commentChar)
        {
            c = line;
            while (*c)
            {
                if (*c == '"') openQuotes = !openQuotes;
                if (*c == commentChar && openQuotes == false)
                {
                    *c = 0;
                    break;
                }
                c++;
            }
        }

        if (ignoreEmpty)
        {
            c = line;
            while (*c)
            {
                if (*c > 32)
                {
                    loopFlag = false;
                    break;
                }
                c++;
            }
        }
        else loopFlag = false;

        if (continuationChar)
        {
            c = line;
            if (*c)
            {
                while (*c)
                {
                    c++;
                    size--;
                }
                c--;
                if (*c == continuationChar)
                {
                    loopFlag = true;
                    size++;
                    line = c;
                }
            }
        }
    }
    return false;
}


// read a line
// returns true on error
bool DataFile::readNextLine(char *line, size_t size)
{
    char *p = m_index;
    char *c = line;
    size_t count = 0;
    size--; // needs to be shrunk to make room for the zero

    if (*p == 0) return true; // at end of file

    while (endOfLineTest(&p) == false)
    {
        if (count >= size)
        {
            *c = 0;
            if (m_exitOnErrorFlag)
            {
                std::cerr << "Error: DataFile::ReadNextLine line longer than string\n";
                exit(1);
            }
            else return true;
        }

        *c = *p;
        count++;
        c++;
        p++;
    }
    m_index = p;
    *c = 0;
    return false;
}

// tests for end of line and bumps pointer
// note takes a pointer to a pointer
bool DataFile::endOfLineTest(char **p)
{
    if (**p == 0) return true; // don't bump past end of string
    if (**p == 10) // must be Unix style linefeed
    {
        (*p)++;
        return true;
    }
    if (**p == 13) // Mac or DOS
    {
        (*p)++;
        if (**p == 10) (*p)++; // DOS
        return true;
    }
    return false;
}

// Count token utility
size_t DataFile::countTokens(const char *string)
{
    const char *p = string;
    bool inToken = false;
    size_t count = 0;

    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            count++;
            if (*p == '"')
            {
                p++;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
        }
        p++;
    }
    return count;
}

// Return tokens utility
// note string is altered by this routine
// if returned count is >= size then there are still tokens
// (this is probably an error status)
// recommend that tokens are counted first
size_t DataFile::returnTokens(char *string, char *ptrs[], size_t size)
{
    char *p = string;
    bool inToken = false;
    size_t count = 0;

    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            if (count >= size) return count;
            ptrs[count] = p;
            count++;
            if (*p == '"')
            {
                p++;
                ptrs[count - 1] = p;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
                *p = 0;
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
            *p = 0;
        }
        p++;
    }
    return count;
}

// Count token utility
size_t DataFile::countTokens(const char *string, const char *separators)
{
    const char *p = string;
    bool inToken = false;
    size_t count = 0;

    while (*p != 0)
    {
        if (inToken == false && strchr(separators, *p) == nullptr)
        {
            inToken = true;
            count++;
            if (*p == '"')
            {
                p++;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
            }
        }
        else if (inToken == true && strchr(separators, *p) != nullptr)
        {
            inToken = false;
        }
        p++;
    }
    return count;
}

// Return tokens utility
// note string is altered by this routine
// if returned count is >= size then there are still tokens
// (this is probably an error status)
// recommend that tokens are counted first
size_t DataFile::returnTokens(char *string, char *ptrs[], size_t size, const char *separators)
{
    char *p = string;
    bool inToken = false;
    size_t count = 0;

    while (*p != 0)
    {
        if (inToken == false && strchr(separators, *p) == nullptr)
        {
            inToken = true;
            if (count >= size) return count;
            ptrs[count] = p;
            count++;
            if (*p == '"')
            {
                p++;
                ptrs[count - 1] = p;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
                *p = 0;
            }
        }
        else if (inToken == true && strchr(separators, *p) != nullptr)
        {
            inToken = false;
            *p = 0;
        }
        p++;
    }
    return count;
}

// Count lines utility
size_t DataFile::countLines(const char *string)
{
    const char *p = string;
    size_t count = 0;

    while (*p != 0)
    {
        if (*p == 10) // lf style line ending
        {
            count++;
            p++;
            continue;
        }

        if (*p == 13) // cr style line ending
        {
            count++;
            p++;
            if (*p == 10) // cr+lf style line ending
            {
                p++;
            }
            continue;
        }

        p++;
    }
    return count;
 }

// Return lines utility
// note string is altered by this routine
// if returned count is >= size then there are still tokens
// (this is probably an error status)
// recommend that tokens are counted first
size_t DataFile::returnLines(char *string, char *ptrs[], size_t size)
{
    char *p = string;
    size_t count = 0;
    ptrs[0] = p;

    while (*p != 0)
    {
        if (*p == 10) // lf style line ending
        {
            *p = 0; // replace the end of line with an end of string
            count++;
            if (count >= size) return count;
            p++;
            ptrs[count] = p;
            continue;
        }

        if (*p == 13) // cr style line ending
        {
            *p = 0; // replace the end of line with an end of string
            count++;
            if (count >= size) return count;
            p++;
            if (*p == 10) // cr+lf style line ending
            {
                p++;
            }
            ptrs[count] = p;
            continue;
        }

        p++;
    }
    return count;
}

// read the next integer
bool DataFile::readNextBinary(int *val)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(int)) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(int));
    m_index += sizeof(int);
    return false;
}

// read the next float
bool DataFile::readNextBinary(float *val)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(float)) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(float));
    m_index += sizeof(float);
    return false;
}

// read the next double
bool DataFile::readNextBinary(double *val)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(double)) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(double));
    m_index += sizeof(double);
    return false;
}

// read the next char
bool DataFile::readNextBinary(char *val)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(char)) return true;
    *val = *m_index;
    m_index += sizeof(char);
    return false;
}

// read the next bool
bool DataFile::readNextBinary(bool *val)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(bool)) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(bool));
    m_index += sizeof(bool);
    return false;
}

// read the next integer array
bool DataFile::readNextBinary(int *val, size_t n)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(int) * n) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(int) * n);
    m_index += sizeof(int) * n;
    return false;
}

// read the next float array
bool DataFile::readNextBinary(float *val, size_t n)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(float) * n) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(float) * n);
    m_index += sizeof(float) * n;
    return false;
}

// read the next double array
bool DataFile::readNextBinary(double *val, size_t n)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(double) * n) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(double) * n);
    m_index += sizeof(double) * n;
    return false;
}

// read the next character array
bool DataFile::readNextBinary(char *val, size_t n)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(char) * n) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(char) * n);
    m_index += sizeof(char) * n;
    return false;
}

// read the next bool array
bool DataFile::readNextBinary(bool *val, size_t n)
{
    if (size_t(m_index - m_fileData) > m_size - sizeof(bool) * n) return true;
    memcpy(reinterpret_cast<void *>((val)), m_index, sizeof(bool) * n);
    m_index += sizeof(bool) * n;
    return false;
}

// write an integer parameter
// returns false on success
bool DataFile::writeParameter(const char * const param, int val)
{
    char buffer[64];

    sprintf(buffer, "%d", val);
    if (writeParameter(param, buffer)) return true;

    return false;
}


// write a double parameter
// returns false on success
bool DataFile::writeParameter(const char * const param, double val)
{
    char buffer[64];

    sprintf(buffer, "%.17e", val);
    if (writeParameter(param, buffer)) return true;

    return false;
}

// write a bool parameter
// returns false on success
bool DataFile::writeParameter(const char * const param, bool val)
{
    char buffer[64];

    if (val) strcpy(buffer, "true");
    else strcpy(buffer, "false");
    if (writeParameter(param, buffer)) return true;

    return true;
}

// write a string parameter
// returns false on success
bool DataFile::writeParameter(const char * const param, const char * const val)
{
    if (writeNext(param, '\t')) return true;
    if (writeNext(val, '\n')) return true;

    return false;
}

// write a quoted string parameter
// returns false on success
bool DataFile::writeQuotedStringParameter(const char * const param, const char * const val)
{
    if (writeNext(param, '\t')) return true;
    if (writeNextQuotedString(val, '\n')) return true;

    return false;
}

// write an integer parameter array
// returns false on success
bool DataFile::writeParameter(const char * const param, size_t n, int *val)
{
    size_t i;
    if (n <= 0) return true;

    if (writeNext(param, '\t')) return true;

    for (i = 0; i < n - 1; i++)
        if (writeNext(val[i], '\t')) return true;

    if (writeNext(val[i], '\n')) return true;
    return false;
}


// write a double parameter array
// returns false on success
bool DataFile::writeParameter(const char * const param, size_t n, double *val)
{
    size_t i;
    if (n <= 0) return true;

    if (writeNext(param, '\t')) return true;

    for (i = 0; i < n - 1; i++)
        if (writeNext(val[i], '\t')) return true;

    if (writeNext(val[i], '\n')) return true;
    return false;
}

// write a bool parameter array
// returns false on success
bool DataFile::writeParameter(const char * const param, size_t n, bool *val)
{
    size_t i;
    if (n <= 0) return true;

    if (writeNext(param, '\t')) return true;

    for (i = 0; i < n - 1; i++)
        if (writeNext(val[i], '\t')) return true;

    if (writeNext(val[i], '\n')) return true;
    return false;
}

// write an integer
// returns false on success
bool DataFile::writeNext(int val, char after)
{
    char buffer[64];

    sprintf(buffer, "%d", val);
    if (writeNext(buffer, after)) return true;

    return false;
}

// write a double
// returns false on success
bool DataFile::writeNext(double val, char after)
{
    char buffer[64];

    sprintf(buffer, "%.17e", val);
    if (writeNext(buffer, after)) return true;

    return false;
}

// write a bool
// returns false on success
bool DataFile::writeNext(bool val, char after)
{
    char buffer[64];

    if (val) strcpy(buffer, "true");
    else strcpy(buffer, "false");
    if (writeNext(buffer, after)) return true;

    return true;
}

// write a string
// returns false on success
// note string must be shorter than kStorageIncrement
bool DataFile::writeNext(const char * const val, char after)
{
    char *p;
    const char *cp;
    bool needQuotes = false;
    size_t size = 0;

    // check for whitespace and measure actual size
    cp = val;
    while (*cp)
    {
        if (*cp < 33) needQuotes = true;
        cp++;
        size++;
    }

    if (m_index + size + 16 >= m_fileData + m_size)
    {
        p = new char[m_size + kStorageIncrement];
        if (p == nullptr)
        {
            if (m_exitOnErrorFlag)
            {
                std::cerr << "Error: DataFile::writeNext(" << val
                << ") - could not allocate memory\n";
                exit(1);
            }
            else
            {
                return true;
            }
        }
        memcpy(p, m_fileData, m_size);
        m_index = p + (m_index - m_fileData);
        delete [] m_fileData;
        m_fileData = p;
        m_size += kStorageIncrement;
    }

    if (needQuotes) *m_index++ = '"';
    memcpy(m_index, val, size);
    m_index += size;
    if (needQuotes) *m_index++ = '"';
    if (after) *m_index++ = after;
    *m_index = 0;

    return false;
}

// write a string
// returns false on success
// note string must be shorter than kStorageIncrement
bool DataFile::writeNextQuotedString(const char * const val, char after)
{
    char *p;
    const char *cp;
    size_t size = 0;

    // check for whitespace and measure actual size
    cp = val;
    while (*cp)
    {
        cp++;
        size++;
    }

    if (m_index + size + 16 >= m_fileData + m_size)
    {
        p = new char[m_size + kStorageIncrement];
        if (p == nullptr)
        {
            if (m_exitOnErrorFlag)
            {
                std::cerr << "Error: DataFile::writeNext(" << val
                << ") - could not allocate memory\n";
                exit(1);
            }
            else
            {
                return true;
            }
        }
        memcpy(p, m_fileData, m_size);
        m_index = p + (m_index - m_fileData);
        delete [] m_fileData;
        m_fileData = p;
        m_size += kStorageIncrement;
    }

    *m_index++ = '"';
    memcpy(m_index, val, size);
    m_index += size;
    *m_index++ = '"';
    if (after) *m_index++ = after;
    *m_index = 0;

    return false;
}

// strip out beginning and ending whitespace
void DataFile::strip(char *str)
{
    char *p1, *p2;

    if (*str == 0) return;

    // heading whitespace
    if (*str <= ' ')
    {
        p1 = str;
        while (*p1)
        {
            if (*p1 > ' ') break;
            p1++;
        }
        p2 = str;
        while (*p1)
        {
            *p2 = *p1;
            p1++;
            p2++;
        }
        *p2 = 0;
    }

    if (*str == 0) return;

    // tailing whitespace
    p1 = str;
    while (*p1)
    {
        p1++;
    }
    p1--;
    while (*p1 <= ' ')
    {
        p1--;
    }
    p1++;
    *p1 = 0;

    return;
}

/*  returns true iff str starts with prefix  */
bool DataFile::stringStartsWith(const char *str, const char *prefix)
{
    if (str == nullptr || prefix == nullptr)
        return false;

    size_t suffix_len = strlen(prefix);
    if (strncmp(str, prefix, suffix_len) == 0) return true;
    return false;
}

/*  returns true iff str ends with suffix  */
bool DataFile::stringEndsWith(const char *str, const char *suffix)
{
    if (str == nullptr || suffix == nullptr)
        return false;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
        return false;

    if (strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0) return true;
    return false;
}

// more handy statics
double DataFile::toDouble(const char *buf)
{
    return strtod(buf, nullptr);
}

void DataFile::toDouble(const char *buf, size_t n, double *d)
{
    char *ptr;
    d[0] = strtod(buf, &ptr);
    for (size_t i = 1; i < n; i++)
        d[i] = strtod(ptr, &ptr);
}

int DataFile::toInt(const char *buf)
{
    return strtol(buf, nullptr, 10);
}

void DataFile::toInt(const char *buf, size_t n, int *d)
{
    char *ptr;
    d[0] = strtol(buf, &ptr, 10);
    for (size_t i = 1; i < n; i++)
        d[i] = strtol(ptr, &ptr, 10);
}

bool DataFile::toBool(const char *buf)
{
    size_t l = strlen(buf);
    const char *pstart = buf;
    const char *pend = buf + l;
    while (*pstart)
    {
        if (*pstart > 32) break;
        pstart++;
    }
    while (pend > pstart)
    {
        pend--;
        if (*pend > 32) break;
    }
    l = size_t(pend - pstart);
    if (l == 5)
        if (strcasecmp(pstart, "false") == 0) return false;
    if (l == 4)
        if (strcasecmp(pstart, "true") == 0) return true;
    if (strtol(pstart, nullptr, 10) != 0) return true;
    return false;
}

std::wstring DataFile::convertUTF8ToWide(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.from_bytes(str);
//    using convert_typeX = std::codecvt_utf8<wchar_t>;
//    std::wstring_convert<convert_typeX, wchar_t> converterX;
//    return converterX.from_bytes(str);
}

std::string DataFile::convertWideToUTF8(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.to_bytes(wstr);
//    using convert_typeX = std::codecvt_utf8<wchar_t>;
//    std::wstring_convert<convert_typeX, wchar_t> converterX;
//    return converterX.to_bytes(wstr);
}

/* Windows specific code
std::wstring string_to_wide_string(const std::string& string)
{
    if (string.empty())
    {
        return L"";
    }

    const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);
    if (size_needed <= 0)
    {
        throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
    }

    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
    return result;
}

std::string wide_string_to_string(const std::wstring& wide_string)
{
    if (wide_string.empty())
    {
        return "";
    }

    const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0)
    {
        throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
    }

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), result.data(), size_needed, nullptr, nullptr);
    r
*/

} // namespace GaitSym


