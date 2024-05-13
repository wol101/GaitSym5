#include "LightBase.h"

#include "GSUtil.h"

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

LightShadowCasting::LightShadowCasting()
{
}

std::string *LightShadowCasting::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("CastsAShadow"s, &buf) == nullptr) return lastErrorPtr();
    m_castShadow = GSUtil::Double(buf);
    if (findAttribute("Near"s, &buf) == nullptr) return lastErrorPtr();
    m_near = float(GSUtil::Double(buf));
    if (findAttribute("Far"s, &buf) == nullptr) return lastErrorPtr();
    m_far = float(GSUtil::Double(buf));
    if (findAttribute("Position"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> position;
    GSUtil::Double(buf, &position);
    if (position.size() != 3)
    {
        setLastError("LIGHT ID=\""s + name() +"\" invalid Position format"s);
        return lastErrorPtr();
    }
    m_position.Set(position.data());
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

void LightShadowCasting::appendToAttributes()
{
    LightBase::appendToAttributes();
    setAttribute("CastsAShadow"s, GSUtil::ToString(m_castShadow));
    setAttribute("Near"s, GSUtil::ToString(m_near));
    setAttribute("Far"s, GSUtil::ToString(m_far));
    setAttribute("Position"s, GSUtil::ToString(m_position));
    std::vector<size_t> shadowMap = {m_mapWidth, m_mapHeight};
    setAttribute("ShadowMapSize"s, GSUtil::ToString(shadowMap));
}

LightDirectional::LightDirectional()
{
}

std::string *LightDirectional::createFromAttributes()
{
    if (LightShadowCasting::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("Width"s, &buf) == nullptr) return lastErrorPtr();
    m_width = float(GSUtil::Double(buf));
    if (findAttribute("Height"s, &buf) == nullptr) return lastErrorPtr();
    m_height = float(GSUtil::Double(buf));

    if (findAttribute("LookAt"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> lookAt;
    GSUtil::Double(buf, &lookAt);
    if (lookAt.size() != 3)
    {
        setLastError("LIGHT ID=\""s + name() +"\" invalid LookAt format"s);
        return lastErrorPtr();
    }
    m_lookAt.Set(lookAt.data());

    if (findAttribute("Up"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> up;
    GSUtil::Double(buf, &up);
    if (up.size() != 3)
    {
        setLastError("LIGHT ID=\""s + name() +"\" invalid Up format"s);
        return lastErrorPtr();
    }
    m_up.Set(m_up.data());

    return nullptr;
}

void LightDirectional::appendToAttributes()
{
    LightShadowCasting::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Directional"s);
    setAttribute("Width"s, GSUtil::ToString(m_width));
    setAttribute("Height"s, GSUtil::ToString(m_height));
    setAttribute("LookAt"s, GSUtil::ToString(m_lookAt));
    setAttribute("Up"s, GSUtil::ToString(m_up));
}

LightSpot::LightSpot()
{
}

std::string *LightSpot::createFromAttributes()
{
    if (LightShadowCasting::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("FoV"s, &buf) == nullptr) return lastErrorPtr();
    m_fov = float(GSUtil::Double(buf));
    if (findAttribute("Aspect"s, &buf) == nullptr) return lastErrorPtr();
    m_asapect = float(GSUtil::Double(buf));

    if (findAttribute("LookAt"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> lookAt;
    GSUtil::Double(buf, &lookAt);
    if (lookAt.size() != 3)
    {
        setLastError("LIGHT ID=\""s + name() +"\" invalid LookAt format"s);
        return lastErrorPtr();
    }
    m_lookAt.Set(lookAt.data());

    if (findAttribute("Up"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<double> up;
    GSUtil::Double(buf, &up);
    if (up.size() != 3)
    {
        setLastError("LIGHT ID=\""s + name() +"\" invalid Up format"s);
        return lastErrorPtr();
    }
    m_up.Set(m_up.data());

    return nullptr;
}

void LightSpot::appendToAttributes()
{
    LightShadowCasting::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Spot"s);
    setAttribute("FoV"s, GSUtil::ToString(m_fov));
    setAttribute("Aspect"s, GSUtil::ToString(m_asapect));
    setAttribute("LookAt"s, GSUtil::ToString(m_lookAt));
    setAttribute("Up"s, GSUtil::ToString(m_up));
}

LightPoint::LightPoint()
{
}

std::string *LightPoint::createFromAttributes()
{
    if (LightShadowCasting::createFromAttributes()) return lastErrorPtr();
    return nullptr;
}

void LightPoint::appendToAttributes()
{
    LightShadowCasting::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Point"s);
}


