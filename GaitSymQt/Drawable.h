/*
 *  Drawable.h
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 08/10/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "PGDMath.h"

#include "threepp/threepp.hpp"

#include <vector>
#include <string>

class FacetedObject;
class SimulationWidget;

class Drawable
{
public:
    Drawable();
    virtual ~Drawable();

    virtual void initialise(SimulationWidget *simulationWidget) = 0;
    virtual void Draw() = 0;
    virtual std::string name() = 0;
    std::string className() const;

    void SetDisplayPosition(double x, double y, double z);
    void SetDisplayScale(double x, double y, double z);
    void SetDisplayRotation(const pgd::Matrix3x3 &R);
    void SetDisplayRotationFromQuaternion(const pgd::Quaternion &q);
    void setVisible(bool visible);

    const std::vector<FacetedObject *> &facetedObjectList() const;

    std::shared_ptr<threepp::Scene> scene() const;
    void setScene(const std::shared_ptr<threepp::Scene> &newScene);

protected:
    std::vector<FacetedObject *> m_facetedObjectList;
    std::shared_ptr<threepp::Scene> m_scene;

#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    static uint64_t m_objectCount;
    uint64_t m_objectCountAtCreation = 0;
#endif
};

#endif // DRAWABLE_H
