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
    return std::string();
}

} // namespace GaitSym

