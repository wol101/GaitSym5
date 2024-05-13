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

LightShadowCasting::LightShadowCasting()
{
}

std::string *LightShadowCasting::createFromAttributes()
{
    if (LightBase::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("CastsAShadow"s, &buf) == nullptr) return lastErrorPtr();
    m_castShadow = GSUtil::Bool(buf);
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

bool LightShadowCasting::castShadow() const
{
    return m_castShadow;
}

void LightShadowCasting::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

size_t LightShadowCasting::mapWidth() const
{
    return m_mapWidth;
}

void LightShadowCasting::setMapWidth(size_t newMapWidth)
{
    m_mapWidth = newMapWidth;
}

size_t LightShadowCasting::mapHeight() const
{
    return m_mapHeight;
}

void LightShadowCasting::setMapHeight(size_t newMapHeight)
{
    m_mapHeight = newMapHeight;
}

float LightShadowCasting::near() const
{
    return m_near;
}

void LightShadowCasting::setNear(float newNear)
{
    m_near = newNear;
}

float LightShadowCasting::far() const
{
    return m_far;
}

void LightShadowCasting::setFar(float newFar)
{
    m_far = newFar;
}

pgd::Vector3 LightShadowCasting::position() const
{
    return m_position;
}

void LightShadowCasting::setPosition(const pgd::Vector3 &newPosition)
{
    m_position = newPosition;
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
    m_up.Set(up.data());

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

pgd::Vector3 LightDirectional::lookAt() const
{
    return m_lookAt;
}

void LightDirectional::setLookAt(const pgd::Vector3 &newLookAt)
{
    m_lookAt = newLookAt;
}

pgd::Vector3 LightDirectional::up() const
{
    return m_up;
}

void LightDirectional::setUp(const pgd::Vector3 &newUp)
{
    m_up = newUp;
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
    m_aspect = float(GSUtil::Double(buf));

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
    m_up.Set(up.data());

    return nullptr;
}

void LightSpot::appendToAttributes()
{
    LightShadowCasting::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Spot"s);
    setAttribute("FoV"s, GSUtil::ToString(m_fov));
    setAttribute("Aspect"s, GSUtil::ToString(m_aspect));
    setAttribute("LookAt"s, GSUtil::ToString(m_lookAt));
    setAttribute("Up"s, GSUtil::ToString(m_up));
}

float LightSpot::fov() const
{
    return m_fov;
}

void LightSpot::setFov(float newFov)
{
    m_fov = newFov;
}

float LightSpot::aspect() const
{
    return m_aspect;
}

void LightSpot::setAspect(float newAspect)
{
    m_aspect = newAspect;
}

pgd::Vector3 LightSpot::lookAt() const
{
    return m_lookAt;
}

void LightSpot::setLookAt(const pgd::Vector3 &newLookAt)
{
    m_lookAt = newLookAt;
}

pgd::Vector3 LightSpot::up() const
{
    return m_up;
}

void LightSpot::setUp(const pgd::Vector3 &newUp)
{
    m_up = newUp;
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


