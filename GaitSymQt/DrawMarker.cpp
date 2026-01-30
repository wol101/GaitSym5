/*
 *  DrawMarker.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 11/03/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#include "DrawMarker.h"
#include "Marker.h"
#include "FacetedObject.h"
#include "PGDMath.h"
#include "Preferences.h"
#include "FacetedSphere.h"

#include <QString>

DrawMarker::DrawMarker()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    m_objectCountAtCreation = m_objectCount++;
    std::cerr << m_objectCountAtCreation << " " << className() << " constructed\n";;
#endif
}

DrawMarker::~DrawMarker()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    std::cerr << m_objectCountAtCreation << " " << className() << " destructed\n";;
#endif
}

std::string DrawMarker::name()
{
    if (m_marker) return m_marker->name();
    else return std::string();
}

GaitSym::Marker *DrawMarker::marker() const
{
    return m_marker;
}

void DrawMarker::setMarker(GaitSym::Marker *marker)
{
    m_marker = marker;
}

void DrawMarker::initialise(SimulationWidget *simulationWidget)
{
    if (!m_marker) return;
    m_facetedObject = std::make_unique<FacetedObject>();
    m_facetedObject->setSimulationWidget(simulationWidget);
    m_facetedObject->readFromResource(":/objects/axes.tri");
    m_facetedObjectList.push_back(m_facetedObject.get());
}

void DrawMarker::updateEntityPose()
{
    pgd::Vector3 p = m_marker->worldPosition();
    pgd::Quaternion q = m_marker->worldQuaternion();
    setDisplayScale(m_marker->size1(), m_marker->size1(), m_marker->size1());
    setDisplayRotationFromQuaternion(q.constData());
    setDisplayPosition(p.x, p.y, p.z);
}

void DrawMarker::draw()
{
    m_facetedObject->draw();
    m_marker->setRedraw(false);
}


