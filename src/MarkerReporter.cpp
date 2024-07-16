#include "MarkerReporter.h"

#include "Marker.h"
#include "GSUtil.h"
#include "Simulation.h"

#include "pystring.h"

using namespace std::literals;

namespace GaitSym {

MarkerReporter::MarkerReporter() {}

std::string *MarkerReporter::createFromAttributes()
{
    if (Reporter::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("MarkerIDList"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> result;
    pystring::split(buf, result);
    if (result.empty())
    {
        setLastError("REPORTER ID=\""s + name() +"\" MarkerIDList list is empty"s);
        return lastErrorPtr();
    }
    std::vector<Marker *> markerList;
    markerList.reserve(result.size());
    for (size_t i = 0; i < result.size(); i++)
    {
        Marker *marker = simulation()->GetMarker(result[i]);
        if (!marker)
        {
            setLastError("REPORTER ID=\""s + name() +"\" marker \""s + result[i] +"\" not found"s);
            return lastErrorPtr();
        }
        markerList.push_back(marker);
    }
    m_markerList = std::move(markerList);

    if (findAttribute("ReportTimes"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> reportTimes;
    GSUtil::Double(buf, &reportTimes);
    if (reportTimes.empty())
    {
        setLastError("REPORTER ID=\""s + name() +"\" ReportTimes list is empty"s);
        return lastErrorPtr();
    }
    m_reportTimes = std::move(reportTimes);

    if (findAttribute("ReportPosition"s, &buf) == nullptr) return lastErrorPtr();
    m_reportPosition = GSUtil::Bool(buf);
    if (findAttribute("ReportQuaternion"s, &buf) == nullptr) return lastErrorPtr();
    m_reportQuaternion = GSUtil::Bool(buf);

    return nullptr;
}

void MarkerReporter::appendToAttributes()
{
    Reporter::appendToAttributes();
    setAttribute("Type"s, "MarkerReporter"s);
    std::vector<std::string> strings;
    strings.reserve(m_markerList.size());
    for (auto && marker : m_markerList) { strings.push_back(marker->name()); }
    setAttribute("MarkerIDList"s, GSUtil::ToString(strings));
    setAttribute("ReportTimes"s, GSUtil::ToString(m_reportTimes));
    setAttribute("ReportPosition"s, GSUtil::ToString(m_reportPosition));
    setAttribute("ReportQuaternion"s, GSUtil::ToString(m_reportQuaternion));
}

std::string MarkerReporter::dumpToString()
{
    std::string s;
    if (firstDump())
    {
        setFirstDump(false);
        std::vector<std::string> sList;
        sList.push_back("time"s);
        for (auto && marker : m_markerList)
        {
            if (m_reportPosition)
            {
                sList.push_back(marker->name() + "_x"s);
                sList.push_back(marker->name() + "_y"s);
                sList.push_back(marker->name() + "_z"s);
            }
            if (m_reportQuaternion)
            {
                sList.push_back(marker->name() + "_n"s);
                sList.push_back(marker->name() + "_x"s);
                sList.push_back(marker->name() + "_y"s);
                sList.push_back(marker->name() + "_z"s);
            }
        }
        s = pystring::join("\t"s, sList) + "\n"s;
    }
    double time = simulation()->GetTime();
    int64_t index = std::lower_bound(m_reportTimes.begin(), m_reportTimes.end(), time) - m_reportTimes.begin(); // this is an index for the time >= playbackTime
    if (index > m_lastReportIndex)
    {
        std::vector<double> vList;
        m_lastReportIndex = index;
        vList.push_back(time);
        pgd::Vector3 v;
        pgd::Quaternion q;
        for (auto && marker : m_markerList)
        {
            if (m_reportPosition)
            {
                v = marker->GetWorldPosition();
                vList.push_back(v.x);
                vList.push_back(v.y);
                vList.push_back(v.z);
            }
            if (m_reportQuaternion)
            {
                q = marker->GetWorldQuaternion();
                vList.push_back(q.n);
                vList.push_back(q.x);
                vList.push_back(q.y);
                vList.push_back(q.z);
            }
        }
        s.append(GSUtil::ToString(vList));
    }

    return s;
}

} // namespace GaitSym

