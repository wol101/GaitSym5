#include "LightBase.h"

#include "GSUtil.h"

using namespace std::string_literals;

LightBase::LightBase()
{
}

std::string *LightBase::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
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

bool m_castShadow = false;
size_t m_mapWidth = 1024;
size_t m_mapHeight = 1024;
pgd::Vector3 m_position;

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
    if (findAttribute("Position"s, &buf) == nullptr) return lastErrorPtr();
    GSUtil::Double(buf, 3, m_position.data());
    if (findAttribute("ShadowMapSize"s, &buf))
    {

    }
    return nullptr;
}

void LightShadowCasting::appendToAttributes()
{
    LightBase::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Ambient"s);
}

