#include "LightBase.h"

#include "GSUtil.h"
#include "Simulation.h"
#include "Marker.h"

using namespace std::string_literals;

LightBase::LightBase()
{
}

std::string *LightBase::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("Intensity"s, &buf) == nullptr) return lastErrorPtr();
    m_intensity = float(GSUtil::Double(buf));
    return nullptr;
}

void LightBase::saveToAttributes()
{
    this->setTag("LIGHT"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

void LightBase::appendToAttributes()
{
    NamedObject::appendToAttributes();
}

float LightBase::intensity() const
{
    return m_intensity;
}

void LightBase::setIntensity(float newIntensity)
{
    m_intensity = newIntensity;
}

LightAmbient::LightAmbient()
{
}

std::string *LightAmbient::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    return nullptr;
}

void LightAmbient::appendToAttributes()
{
    LightBase::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Ambient"s);
}

LightDirectional::LightDirectional()
{
}

std::string *LightDirectional::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("CastsAShadow"s, &buf) == nullptr) return lastErrorPtr();
    m_castShadow = GSUtil::Bool(buf);
    if (findAttribute("Distance"s, &buf) == nullptr) return lastErrorPtr();
    m_distance = float(GSUtil::Double(buf));

    if (findAttribute("PositionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto positionMarker = simulation()->GetMarkerList()->find(buf);
    if (positionMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("LIGHT ID=\""s + name() +"\" PositionMarkerID not found"s);
        return lastErrorPtr();
    }
    m_positionMarker = positionMarker->second.get();

    if (findAttribute("TargetMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto targetMarker = simulation()->GetMarkerList()->find(buf);
    if (targetMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("LIGHT ID=\""s + name() +"\" TargetMarkerID not found"s);
        return lastErrorPtr();
    }
    m_targetMarker = targetMarker->second.get();

    if (findAttribute("ShadowMapSize"s, &buf))
    {
        std::vector<int> shadowMap;
        GSUtil::Int(buf, &shadowMap);
        switch (shadowMap.size())
        {
        case 1:
            m_mapWidth = m_mapHeight = shadowMap[0];
            break;
        case 2:
            m_mapWidth = shadowMap[0];
            m_mapHeight = shadowMap[1];
            break;
        default:
            setLastError("LIGHT ID=\""s + name() +"\" invalid ShadowMapSize format"s);
            return lastErrorPtr();
        }
    }

    if (findAttribute("Width"s, &buf)) { m_width = float(GSUtil::Double(buf)); }
    if (findAttribute("Height"s, &buf)) { m_height = float(GSUtil::Double(buf)); }

    return nullptr;
}

void LightDirectional::appendToAttributes()
{
    LightBase::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Directional"s);
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Distance"s, GSUtil::ToString(m_distance));
    setAttribute("PositionMarkerID"s, m_positionMarker->name());
    setAttribute("TargetMarkerID"s, m_targetMarker->name());
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
    setAttribute("Width"s, GSUtil::ToString(m_width));
    setAttribute("Height"s, GSUtil::ToString(m_height));
}

bool LightDirectional::castShadow() const
{
    return m_castShadow;
}

void LightDirectional::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t LightDirectional::mapWidth() const
{
    return m_mapWidth;
}

void LightDirectional::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t LightDirectional::mapHeight() const
{
    return m_mapHeight;
}

void LightDirectional::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float LightDirectional::width() const
{
    return m_width;
}

void LightDirectional::setWidth(float newWidth)
{
    m_width = newWidth;
}

float LightDirectional::height() const
{
    return m_height;
}

void LightDirectional::setHeight(float newHeight)
{
    m_height = newHeight;
}

float LightDirectional::distance() const
{
    return m_distance;
}

void LightDirectional::setDistance(float newDistance)
{
    m_distance = newDistance;
}

Marker *LightDirectional::positionMarker() const
{
    return m_positionMarker;
}

void LightDirectional::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

Marker *LightDirectional::targetMarker() const
{
    return m_targetMarker;
}

void LightDirectional::setTargetMarker(Marker *newTargetMarker)
{
    m_targetMarker = newTargetMarker;
}

float LightDirectional::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}

LightSpot::LightSpot()
{
}

std::string *LightSpot::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("CastsAShadow"s, &buf) == nullptr) return lastErrorPtr();
    m_castShadow = GSUtil::Bool(buf);
    if (findAttribute("Distance"s, &buf) == nullptr) return lastErrorPtr();
    m_distance = float(GSUtil::Double(buf));

    if (findAttribute("PositionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto positionMarker = simulation()->GetMarkerList()->find(buf);
    if (positionMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("LIGHT ID=\""s + name() +"\" PositionMarkerID not found"s);
        return lastErrorPtr();
    }
    m_positionMarker = positionMarker->second.get();

    if (findAttribute("TargetMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto targetMarker = simulation()->GetMarkerList()->find(buf);
    if (targetMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("LIGHT ID=\""s + name() +"\" TargetMarkerID not found"s);
        return lastErrorPtr();
    }
    m_targetMarker = targetMarker->second.get();

    if (findAttribute("ShadowMapSize"s, &buf))
    {
        std::vector<int> shadowMap;
        GSUtil::Int(buf, &shadowMap);
        switch (shadowMap.size())
        {
        case 1:
            m_mapWidth = m_mapHeight = shadowMap[0];
            break;
        case 2:
            m_mapWidth = shadowMap[0];
            m_mapHeight = shadowMap[1];
            break;
        default:
            setLastError("LIGHT ID=\""s + name() +"\" invalid ShadowMapSize format"s);
            return lastErrorPtr();
        }
    }

    if (findAttribute("Angle"s, &buf) == nullptr) return lastErrorPtr();
    m_angle = float(GSUtil::Double(buf));
    if (findAttribute("Penumbra"s, &buf)) { m_penumbra = float(GSUtil::Double(buf)); }

    return nullptr;
}

void LightSpot::appendToAttributes()
{
    LightBase::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Directional"s);
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Distance"s, GSUtil::ToString(m_distance));
    setAttribute("PositionMarkerID"s, m_positionMarker->name());
    setAttribute("TargetMarkerID"s, m_targetMarker->name());
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
    setAttribute("Angle"s, GSUtil::ToString(m_angle));
    setAttribute("Penumbra"s, GSUtil::ToString(m_penumbra));
}

bool LightSpot::castShadow() const
{
    return m_castShadow;
}

void LightSpot::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t LightSpot::mapWidth() const
{
    return m_mapWidth;
}

void LightSpot::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t LightSpot::mapHeight() const
{
    return m_mapHeight;
}

void LightSpot::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float LightSpot::distance() const
{
    return m_distance;
}

void LightSpot::setDistance(float newDistance)
{
    m_distance = newDistance;
}

float LightSpot::angle() const
{
    return m_angle;
}

void LightSpot::setAngle(float newAngle)
{
    m_angle = newAngle;
}

float LightSpot::penumbra() const
{
    return m_penumbra;
}

void LightSpot::setPenumbra(float newPenumbra)
{
    m_penumbra = newPenumbra;
}

Marker *LightSpot::positionMarker() const
{
    return m_positionMarker;
}

void LightSpot::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

Marker *LightSpot::targetMarker() const
{
    return m_targetMarker;
}

void LightSpot::setTargetMarker(Marker *newTargetMarker)
{
    m_targetMarker = newTargetMarker;
}

float LightSpot::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}

LightPoint::LightPoint()
{
}

std::string *LightPoint::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("CastsAShadow"s, &buf) == nullptr) return lastErrorPtr();
    m_castShadow = GSUtil::Bool(buf);
    if (findAttribute("Distance"s, &buf) == nullptr) return lastErrorPtr();
    m_distance = float(GSUtil::Double(buf));

    if (findAttribute("PositionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto positionMarker = simulation()->GetMarkerList()->find(buf);
    if (positionMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("LIGHT ID=\""s + name() +"\" PositionMarkerID not found"s);
        return lastErrorPtr();
    }
    m_positionMarker = positionMarker->second.get();

    if (findAttribute("ShadowMapSize"s, &buf))
    {
        std::vector<int> shadowMap;
        GSUtil::Int(buf, &shadowMap);
        switch (shadowMap.size())
        {
        case 1:
            m_mapWidth = m_mapHeight = shadowMap[0];
            break;
        case 2:
            m_mapWidth = shadowMap[0];
            m_mapHeight = shadowMap[1];
            break;
        default:
            setLastError("LIGHT ID=\""s + name() +"\" invalid ShadowMapSize format"s);
            return lastErrorPtr();
        }
    }

    return nullptr;
}

void LightPoint::appendToAttributes()
{
    LightBase::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Point"s);
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Distance"s, GSUtil::ToString(m_distance));
    setAttribute("PositionMarkerID"s, m_positionMarker->name());
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
}

bool LightPoint::castShadow() const
{
    return m_castShadow;
}

void LightPoint::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t LightPoint::mapWidth() const
{
    return m_mapWidth;
}

void LightPoint::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t LightPoint::mapHeight() const
{
    return m_mapHeight;
}

void LightPoint::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float LightPoint::distance() const
{
    return m_distance;
}

void LightPoint::setDistance(float newDistance)
{
    m_distance = newDistance;
}

Marker *LightPoint::positionMarker() const
{
    return m_positionMarker;
}

void LightPoint::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

float LightPoint::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}


