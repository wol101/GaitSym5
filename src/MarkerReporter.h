#ifndef MARKERREPORTER_H
#define MARKERREPORTER_H

#include "Reporter.h"

namespace GaitSym {

class Marker;

class MarkerReporter : public Reporter
{
public:
    MarkerReporter();

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    std::vector<Marker *> m_markerList;
    std::vector<double> m_reportTimes;
    size_t m_lastReportIndex;
    bool m_reportPosition;
    bool m_reportQuaternion;
};

}

#endif // MARKERREPORTER_H
