#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "NamedObject.h"

class Marker;

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

class LightDirectional : public LightBase
{
public:
    LightDirectional();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    bool castShadow() const;
    void setCastShadow(bool newCastShadow);

    size_t mapWidth() const;
    void setMapWidth(size_t newMapWidth);

    size_t mapHeight() const;
    void setMapHeight(size_t newMapHeight);

    float width() const;
    void setWidth(float newWidth);

    float height() const;
    void setHeight(float newHeight);

    float distance() const;
    void setDistance(float newDistance);

    Marker *positionMarker() const;
    void setPositionMarker(Marker *newPositionMarker);

    Marker *targetMarker() const;
    void setTargetMarker(Marker *newTargetMarker);

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_width = 100;
    float m_height = 100;
    float m_distance = 100;
    Marker *m_positionMarker = nullptr;
    Marker *m_targetMarker = nullptr;
};

class LightSpot : public LightBase
{
public:
    LightSpot();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    bool castShadow() const;
    void setCastShadow(bool newCastShadow);

    size_t mapWidth() const;
    void setMapWidth(size_t newMapWidth);

    size_t mapHeight() const;
    void setMapHeight(size_t newMapHeight);

    float distance() const;
    void setDistance(float newDistance);

    float angle() const;
    void setAngle(float newAngle);

    float penumbra() const;
    void setPenumbra(float newPenumbra);

    Marker *positionMarker() const;
    void setPositionMarker(Marker *newPositionMarker);

    Marker *targetMarker() const;
    void setTargetMarker(Marker *newTargetMarker);

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_distance = 100;
    float m_angle = 0.1f;
    float m_penumbra = 0.9f;
    Marker *m_positionMarker = nullptr;
    Marker *m_targetMarker = nullptr;
};

class LightPoint : public LightBase
{
public:
    LightPoint();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    bool castShadow() const;
    void setCastShadow(bool newCastShadow);

    size_t mapWidth() const;
    void setMapWidth(size_t newMapWidth);

    size_t mapHeight() const;
    void setMapHeight(size_t newMapHeight);

    float distance() const;
    void setDistance(float newDistance);

    Marker *positionMarker() const;
    void setPositionMarker(Marker *newPositionMarker);

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_distance = 100;
    Marker *m_positionMarker = nullptr;
};


#endif // LIGHTBASE_H

