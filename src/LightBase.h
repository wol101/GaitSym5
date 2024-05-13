#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "NamedObject.h"

#include "PGDMath.h"

class LightBase : public NamedObject
{
public:
    LightBase();

    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();
};

class LightAmbient : public LightBase
{
public:
    LightAmbient();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();
};

class LightShadowCasting : public LightBase
{
public:
    LightShadowCasting();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    pgd::Vector3 m_position;
};

class LightDirectional : public LightShadowCasting
{
public:
    LightDirectional();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    float m_width = 2;
    float m_height = 2;
    float m_near = 0.1;
    float m_far = 1000;
    pgd::Vector3 m_lookAt;
    pgd::Vector3 m_up;
};

class LightSpot : public LightShadowCasting
{
public:
    LightSpot();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    float m_fov = 1;
    float m_asapect = 1;
    float m_near = 0.1;
    float m_far = 1000;
    pgd::Vector3 m_lookAt;
    pgd::Vector3 m_up;
};

class LightPoint : public LightShadowCasting
{
public:
    LightPoint();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    float m_near = 0.1;
    float m_far = 1000;
    pgd::Vector3 m_position;
};


#endif // LIGHTBASE_H

