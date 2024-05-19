#include "LightBase.h"

#include "GSUtil.h"
#include "Simulation.h"
#include "Marker.h"

using namespace std::string_literals;

Light::Light()
{
}

std::string *Light::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("Intensity"s, &buf) == nullptr) return lastErrorPtr();
    m_intensity = float(GSUtil::Double(buf));
    return nullptr;
}

void Light::saveToAttributes()
{
    this->setTag("LIGHT"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

void Light::appendToAttributes()
{
    NamedObject::appendToAttributes();
    setAttribute("Intensity"s, GSUtil::ToString(m_intensity));
}

float Light::intensity() const
{
    return m_intensity;
}

void Light::setIntensity(float newIntensity)
{
    m_intensity = newIntensity;
}

AmbientLight::AmbientLight()
{
}

std::string *AmbientLight::createFromAttributes()
{
    if (Light::createFromAttributes()) return lastErrorPtr();
    return nullptr;
}

void AmbientLight::appendToAttributes()
{
    Light::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Ambient"s);
}

DirectionalLight::DirectionalLight()
{
}

std::string *DirectionalLight::createFromAttributes()
{
    if (Light::createFromAttributes()) return lastErrorPtr();
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

void DirectionalLight::appendToAttributes()
{
    Light::appendToAttributes();
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

bool DirectionalLight::castShadow() const
{
    return m_castShadow;
}

void DirectionalLight::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t DirectionalLight::mapWidth() const
{
    return m_mapWidth;
}

void DirectionalLight::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t DirectionalLight::mapHeight() const
{
    return m_mapHeight;
}

void DirectionalLight::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float DirectionalLight::width() const
{
    return m_width;
}

void DirectionalLight::setWidth(float newWidth)
{
    m_width = newWidth;
}

float DirectionalLight::height() const
{
    return m_height;
}

void DirectionalLight::setHeight(float newHeight)
{
    m_height = newHeight;
}

float DirectionalLight::distance() const
{
    return m_distance;
}

void DirectionalLight::setDistance(float newDistance)
{
    m_distance = newDistance;
}

Marker *DirectionalLight::positionMarker() const
{
    return m_positionMarker;
}

void DirectionalLight::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

Marker *DirectionalLight::targetMarker() const
{
    return m_targetMarker;
}

void DirectionalLight::setTargetMarker(Marker *newTargetMarker)
{
    m_targetMarker = newTargetMarker;
}

float DirectionalLight::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}

SpotLight::SpotLight()
{
}

std::string *SpotLight::createFromAttributes()
{
    if (Light::createFromAttributes()) return lastErrorPtr();
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

void SpotLight::appendToAttributes()
{
    Light::appendToAttributes();
    setAttribute("Type"s, "Spot"s);
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Distance"s, GSUtil::ToString(m_distance));
    setAttribute("PositionMarkerID"s, m_positionMarker->name());
    setAttribute("TargetMarkerID"s, m_targetMarker->name());
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
    setAttribute("Angle"s, GSUtil::ToString(m_angle));
    setAttribute("Penumbra"s, GSUtil::ToString(m_penumbra));
}

bool SpotLight::castShadow() const
{
    return m_castShadow;
}

void SpotLight::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t SpotLight::mapWidth() const
{
    return m_mapWidth;
}

void SpotLight::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t SpotLight::mapHeight() const
{
    return m_mapHeight;
}

void SpotLight::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float SpotLight::distance() const
{
    return m_distance;
}

void SpotLight::setDistance(float newDistance)
{
    m_distance = newDistance;
}

float SpotLight::angle() const
{
    return m_angle;
}

void SpotLight::setAngle(float newAngle)
{
    m_angle = newAngle;
}

float SpotLight::penumbra() const
{
    return m_penumbra;
}

void SpotLight::setPenumbra(float newPenumbra)
{
    m_penumbra = newPenumbra;
}

Marker *SpotLight::positionMarker() const
{
    return m_positionMarker;
}

void SpotLight::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

Marker *SpotLight::targetMarker() const
{
    return m_targetMarker;
}

void SpotLight::setTargetMarker(Marker *newTargetMarker)
{
    m_targetMarker = newTargetMarker;
}

float SpotLight::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}

PointLight::PointLight()
{
}

std::string *PointLight::createFromAttributes()
{
    if (Light::createFromAttributes()) return lastErrorPtr();
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

void PointLight::appendToAttributes()
{
    Light::appendToAttributes();
    setAttribute("Type"s, "Point"s);
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Distance"s, GSUtil::ToString(m_distance));
    setAttribute("PositionMarkerID"s, m_positionMarker->name());
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
}

bool PointLight::castShadow() const
{
    return m_castShadow;
}

void PointLight::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t PointLight::mapWidth() const
{
    return m_mapWidth;
}

void PointLight::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t PointLight::mapHeight() const
{
    return m_mapHeight;
}

void PointLight::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float PointLight::distance() const
{
    return m_distance;
}

void PointLight::setDistance(float newDistance)
{
    m_distance = newDistance;
}

Marker *PointLight::positionMarker() const
{
    return m_positionMarker;
}

void PointLight::setPositionMarker(Marker *newPositionMarker)
{
    m_positionMarker = newPositionMarker;
}

float PointLight::minDistanceMultiplier() const
{
    return m_minDistanceMultiplier;
}


