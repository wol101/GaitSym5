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

    float intensity() const;
    void setIntensity(float newIntensity);

private:
    float m_intensity = 0; // candela for three.js light models [Luminous intensity (candela) = Luminous flux (lumen) / Solid angle (steradian)]
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

    bool castShadow() const;
    void setCastShadow(bool newCastShadow);

    size_t mapWidth() const;
    void setMapWidth(size_t newMapWidth);

    size_t mapHeight() const;
    void setMapHeight(size_t newMapHeight);

    float near() const;
    void setNear(float newNear);

    float far() const;
    void setFar(float newFar);

    pgd::Vector3 position() const;
    void setPosition(const pgd::Vector3 &newPosition);

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_near = 0.1f;
    float m_far = 1000;
    pgd::Vector3 m_position;
};

class LightDirectional : public LightShadowCasting
{
public:
    LightDirectional();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    float width() const;
    void setWidth(float newWidth);

    float height() const;
    void setHeight(float newHeight);

    pgd::Vector3 lookAt() const;
    void setLookAt(const pgd::Vector3 &newLookAt);

    pgd::Vector3 up() const;
    void setUp(const pgd::Vector3 &newUp);

private:
    float m_width = 2;
    float m_height = 2;
    pgd::Vector3 m_lookAt;
    pgd::Vector3 m_up;
};

class LightSpot : public LightShadowCasting
{
public:
    LightSpot();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    float fov() const;
    void setFov(float newFov);

    float aspect() const;
    void setAspect(float newAspect);

    pgd::Vector3 lookAt() const;
    void setLookAt(const pgd::Vector3 &newLookAt);

    pgd::Vector3 up() const;
    void setUp(const pgd::Vector3 &newUp);

private:
    float m_fov = 1;
    float m_aspect = 1;
    pgd::Vector3 m_lookAt;
    pgd::Vector3 m_up;
};

class LightPoint : public LightShadowCasting
{
public:
    LightPoint();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();
};


#endif // LIGHTBASE_H

