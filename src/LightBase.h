#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "NamedObject.h"

class Marker;

namespace GaitSym
{

class Light : public NamedObject
{
public:
    Light();

    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    float intensity() const;
    void setIntensity(float newIntensity);

private:
    float m_intensity = 0; // candela for three.js light models [Luminous intensity (candela) = Luminous flux (lumen) / Solid angle (steradian)]
};

class AmbientLight : public Light
{
public:
    AmbientLight();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();
};

class DirectionalLight : public Light
{
public:
    DirectionalLight();

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

    float minDistanceMultiplier() const;

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_width = 100;
    float m_height = 100;
    float m_distance = 100;
    Marker *m_positionMarker = nullptr;
    Marker *m_targetMarker = nullptr;

    float m_minDistanceMultiplier = 10000.0f; // means that with a distance of 100, shadows are created from 0.01 to 100
};

class SpotLight : public Light
{
public:
    SpotLight();

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

    float minDistanceMultiplier() const;

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_distance = 100;
    float m_angle = 0.1f;
    float m_penumbra = 0.9f;
    Marker *m_positionMarker = nullptr;
    Marker *m_targetMarker = nullptr;

    float m_minDistanceMultiplier = 10000.0f; // means that with a distance of 100, shadows are created from 0.01 to 100
};

class PointLight : public Light
{
public:
    PointLight();

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

    float minDistanceMultiplier() const;

private:
    bool m_castShadow = false;
    size_t m_mapWidth = 1024;
    size_t m_mapHeight = 1024;
    float m_distance = 100;
    Marker *m_positionMarker = nullptr;

    float m_minDistanceMultiplier = 10000.0f; // means that with a distance of 100, shadows are created from 0.01 to 100
};

}

#endif // LIGHTBASE_H

