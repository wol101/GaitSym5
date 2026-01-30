/*
 *  ArgParse.h
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 22/12/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace GaitSym
{

class ArgParse
{
public:
    ArgParse();

    enum ArgType {String, Int, Bool, Double};

    // this needs to be called first and sets up some of the main information
    void initialise(int argc, const char **argv, const std::string &appHelpText, size_t maxNumEndArguments, size_t minNumEndArguments);

    // this is used for boolean flags that have no arguments. They return true when set and false otherwise
    void addArgument(const std::string &shortName, const std::string &longName, const std::string &helpText);

    // this is used for arguments with a fixed number of values
    void addArgument(const std::string &shortName, const std::string &longName, const std::string &helpText, const std::string &defaultValue, size_t numArgs, bool required, ArgType argType);

    // this is used for arguments with a variable number of values
    void addArgument(const std::string &shortName, const std::string &longName, const std::string &helpText, const std::string &defaultValue, size_t minArgs, size_t maxArgs, bool required, ArgType argType);

    // this needs to be called after the arguments have been added
    int parse();

    // this prints out a usage string
    void usage();

    // these get the values of arguments indexed on their longName values
    bool get(const std::string &argument, std::vector<std::string> *strings);
    bool get(const std::string &argument, std::vector<int> *ints);
    bool get(const std::string &argument, std::vector<bool> *bools);
    bool get(const std::string &argument, std::vector<double> *doubles);
    bool get(const std::string &argument, std::string *s);
    bool get(const std::string &argument, int *i);
    bool get(const std::string &argument, bool *b);
    bool get(const std::string &argument, double *d);

    // these get the values of end arguments
    bool get(std::vector<std::string> *strings);
    bool get(std::vector<int> *ints);
    bool get(std::vector<bool> *bools);
    bool get(std::vector<double> *doubles);
    bool get(std::string *s);
    bool get(int *i);
    bool get(bool *b);
    bool get(double *d);

    // mostly internal utilities
    static bool isNumber(const std::string &s);
    static bool isInt(const std::string &s);
    static bool isBool(const std::string &s);
    static int toBool(const std::string &s);
    static double toDouble(const std::string &buf);
    static int toInt(const std::string &buf);


private:
    struct Argument
    {
        std::string shortName;
        std::string longName;
        std::string helpText;
        std::string defaultValue;
        size_t minArgs;
        size_t maxArgs;
        bool required;
        ArgType argType;
    };

    static bool argumentsOK(const std::string s, const Argument &a);

    std::vector<std::string> m_rawArguments;
    std::vector<Argument> m_argumentList;
    std::unordered_map<std::string, size_t> m_argumentListIndex;
    std::unordered_map<std::string, std::vector<std::string>> m_parsedArguments;
    std::string m_appHelpText;
    std::vector<std::string> m_endArguments;
    size_t m_minNumEndArguments = 0;
    size_t m_maxNumEndArguments = 0;
    std::string m_lastError;
};

}

#endif // ARGPARSE_H
