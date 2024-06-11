/*
 *  DrawMuscle.h
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 19/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#ifndef DRAWMUSCLE_H
#define DRAWMUSCLE_H

#include "Drawable.h"

#include "Colour.h"
#include "SmartEnum.h"

#include <QColor>

#include <vector>
#include <memory>

namespace GaitSym { class Muscle; }
class FacetedObject;
class SimulationWidget;

class DrawMuscle : public Drawable
{
public:
    DrawMuscle();
    virtual ~DrawMuscle();

    SMART_ENUM(MuscleDrawStyle, muscleDrawStyleStrings, muscleDrawStyleCount, FixedCylinder, VolumeCylinder, AreaCylinder, FixedFusiform, VolumeFusiform, AreaFusiform);

    virtual void initialise(SimulationWidget *simulationWidget);
    virtual void Draw();
    virtual std::string name();

    GaitSym::Muscle *muscle() const;
    void setMuscle(GaitSym::Muscle *muscle);

    double strapRadius() const;
    void setStrapRadius(double strapRadius);

    size_t strapNumSegments() const;
    void setStrapNumSegments(size_t strapNumSegments);

    double strapCylinderLength() const;
    void setStrapCylinderLength(double strapCylinderLength);

    bool displayMuscleForces() const;
    void setDisplayMuscleForces(bool displayMuscleForces);

    double strapForceScale() const;
    void setStrapForceScale(double strapForceScale);

    double strapForceRadius() const;
    void setStrapForceRadius(double strapForceRadius);

    QColor strapForceColor() const;
    void setStrapForceColor(const QColor &strapForceColor);

    QColor strapColor() const;
    void setStrapColor(const QColor &strapColor);

    QColor strapCylinderColor() const;
    void setStrapCylinderColor(const QColor &strapCylinderColor);

    GaitSym::Colour::ColourMap strapColourMap() const;
    void setStrapColourMap(const GaitSym::Colour::ColourMap &strapColourMap);

private:
    GaitSym::Muscle *m_muscle = nullptr;

    std::unique_ptr<FacetedObject> m_facetedObject1;
    std::unique_ptr<FacetedObject> m_facetedObject2;
    std::unique_ptr<FacetedObject> m_facetedObject3;
    std::vector<std::unique_ptr<FacetedObject>> m_facetedObjectForceList;

    SimulationWidget *m_simulationWidget = nullptr;;
    double m_strapRadius = 0;
    size_t m_strapNumSegments = 0;
    double m_strapCylinderLength = 0;
    size_t m_strapCylinderSegments = 0;
    size_t m_strapCylinderWrapSegments = 0;
    bool m_displayMuscleForces = 0;
    double m_strapForceScale = 0;
    double m_strapForceRadius = 0;
    QColor m_strapForceColor;
    QColor m_strapColor;
    QColor m_strapCylinderColor;
    GaitSym::Colour::ColourMap m_strapColourMap = GaitSym::Colour::ColourMap::JetColourMap;
    size_t m_strapNumSections = 0;
    std::vector<pgd::Vector3> m_polyline;

    MuscleDrawStyle m_muscleDrawStyle = AreaFusiform;
    void Cylinder();
    void Fusiform();
};

#endif // DRAWMUSCLE_H
