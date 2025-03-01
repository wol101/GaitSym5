/*
 *  DrawGeom.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 11/04/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#include "DrawGeom.h"
#include "Geom.h"
#include "FacetedObject.h"
#include "FacetedSphere.h"
#include "FacetedBox.h"
#include "FacetedCappedCylinder.h"
#include "FacetedRect.h"
#include "Marker.h"
#include "CappedCylinderGeom.h"
#include "SphereGeom.h"
#include "BoxGeom.h"
#include "PlaneGeom.h"
#include "ConvexGeom.h"
#include "FacetedConicSegment.h"
#include "FacetedPolyline.h"
#include "FacetedCheckerboard.h"
#include "PGDMath.h"
#include "Preferences.h"

#include <QString>
#include <QDir>
#include <QDebug>

#include <vector>

DrawGeom::DrawGeom()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    m_objectCountAtCreation = m_objectCount++;
    std::cerr << m_objectCountAtCreation << " " << className() << " constructed\n";;
#endif
    m_geomColor1 = Preferences::valueQColor("GeomColour1");
    m_geomColor2 = Preferences::valueQColor("GeomColour2");
    m_geomSegments = size_t(Preferences::valueInt("GeomSegments"));
    m_geomSize1 = Preferences::valueDouble("GeomSize1");
    m_geomSize2 = Preferences::valueDouble("GeomSize2");
    m_geomSize3 = Preferences::valueDouble("GeomSize3");
}

DrawGeom::~DrawGeom()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    std::cerr << m_objectCountAtCreation << " " << className() << " destructed\n";;
#endif
}

std::string DrawGeom::name()
{
    if (m_geom) return m_geom->name();
    else return std::string();
}

GaitSym::Geom *DrawGeom::geom() const
{
    return m_geom;
}

void DrawGeom::setGeom(GaitSym::Geom *geom)
{
    m_geom = geom;
}

void DrawGeom::initialise(SimulationWidget *simulationWidget)
{
    if (!m_geom) return;

    m_geomColor1.setRedF(qreal(m_geom->colour1().r()));
    m_geomColor1.setGreenF(qreal(m_geom->colour1().g()));
    m_geomColor1.setBlueF(qreal(m_geom->colour1().b()));
    m_geomColor1.setAlphaF(qreal(m_geom->colour1().alpha()));
    m_geomColor2.setRedF(qreal(m_geom->colour2().r()));
    m_geomColor2.setGreenF(qreal(m_geom->colour2().g()));
    m_geomColor2.setBlueF(qreal(m_geom->colour2().b()));
    m_geomColor2.setAlphaF(qreal(m_geom->colour2().alpha()));
    m_geomSize1 = m_geom->size1();
    m_geomSize2 = m_geom->size2();
    m_geomSize3 = m_geom->size3();

    if (GaitSym::SphereGeom *sphereGeom = dynamic_cast<GaitSym::SphereGeom *>(m_geom))
    {
        m_facetedObject = std::make_unique<FacetedSphere>(sphereGeom->radius(), FacetedSphere::EstimateLevel(m_geomSegments), m_geomColor1, 1);
        m_facetedObject->setSimulationWidget(simulationWidget);
        m_facetedObjectList.push_back(m_facetedObject.get());
        return;
    }

    if (GaitSym::CappedCylinderGeom *cappedCylinderGeom = dynamic_cast<GaitSym::CappedCylinderGeom *>(m_geom))
    {
        double length, radius;
        cappedCylinderGeom->getLengthRadius(&length, &radius);
        m_facetedObject = std::make_unique<FacetedCappedCylinder>(length, radius, m_geomSegments / 4, m_geomColor1, 1);
        m_facetedObject->setSimulationWidget(simulationWidget);
        m_facetedObjectList.push_back(m_facetedObject.get());
        return;
    }

    if (GaitSym::BoxGeom *boxGeom = dynamic_cast<GaitSym::BoxGeom *>(m_geom))
    {
        double lx, ly, lz;
        boxGeom->GetDimensions(&lx, &ly, &lz);
        m_facetedObject = std::make_unique<FacetedBox>(lx, ly, lz, m_geomColor1, 1);
        m_facetedObject->setSimulationWidget(simulationWidget);
        m_facetedObjectList.push_back(m_facetedObject.get());
        return;
    }

    if (GaitSym::PlaneGeom *planeGeom = dynamic_cast<GaitSym::PlaneGeom *>(m_geom))
    {
        double planeSize = m_geomSize1;
        double checkerSize = m_geomSize2;
        double planeDepth = m_geomSize3; // can be zero, but if set to a small value then the wireframe underside will be hidden by the solid surface
        if (planeSize < 2 * checkerSize) planeSize = 2 * checkerSize;
        size_t nx = size_t(planeSize / checkerSize);
        size_t ny = nx;
        m_facetedObject = std::make_unique<FacetedCheckerboard>(nx, ny, checkerSize, checkerSize, m_geomColor1, m_geomColor2);
        m_facetedObject->setSimulationWidget(simulationWidget);
        m_facetedObject->setReceiveShadow(true);
        m_facetedObjectList.push_back(m_facetedObject.get());
        if (Preferences::valueBool("PlaneGeomWireframeBack", true))
        {
            // and on the back we want a wireframe version
            m_facetedObject2 = std::make_unique<FacetedCheckerboard>(nx, ny, checkerSize, checkerSize, m_geomColor1, m_geomColor2);
            m_facetedObject2->setSimulationWidget(simulationWidget);
            m_facetedObject2->setWireframe(true);
            m_facetedObject->setCastShadow(false);
            m_facetedObject2->ReverseWinding();
            m_facetedObject2->Move(0, 0, -planeDepth);
            m_facetedObjectList.push_back(m_facetedObject2.get());
        }
        return;
    }

    if (GaitSym::ConvexGeom *convexGeom = dynamic_cast<GaitSym::ConvexGeom *>(m_geom))
    {
        m_facetedObject = std::make_unique<FacetedObject>();
        m_facetedObject->setBlendColour(m_geomColor1, 1);
        m_facetedObject->setSimulationWidget(simulationWidget);
        std::vector<int> *triangles = convexGeom->triangles();
        std::vector<double> *vertices = convexGeom->vertices();
        double triangleVertices[9];
        m_facetedObject->AllocateMemory(triangles->size() / 3);
        for (size_t i = 0; i < triangles->size(); )
        {
            triangleVertices[0] = (*vertices)[(*triangles)[i] * 3];
            triangleVertices[1] = (*vertices)[(*triangles)[i] * 3 + 1];
            triangleVertices[2] = (*vertices)[(*triangles)[i] * 3 + 2];
            i++;
            triangleVertices[3] = (*vertices)[(*triangles)[i] * 3];
            triangleVertices[4] = (*vertices)[(*triangles)[i] * 3 + 1];
            triangleVertices[5] = (*vertices)[(*triangles)[i] * 3 + 2];
            i++;
            triangleVertices[6] = (*vertices)[(*triangles)[i] * 3];
            triangleVertices[7] = (*vertices)[(*triangles)[i] * 3 + 1];
            triangleVertices[8] = (*vertices)[(*triangles)[i] * 3 + 2];
            i++;
            m_facetedObject->AddTriangle(triangleVertices);
        }
        m_facetedObjectList.push_back(m_facetedObject.get());
        return;
    }

    qDebug() << "Error in DrawGeom::initialise: Unsupported GEOM type";
}

void DrawGeom::updateEntityPose()
{
    GaitSym::Marker *marker = m_geom->geomMarker();
    pgd::Quaternion q = marker->GetWorldQuaternion();
    pgd::Vector3 p = marker->GetWorldPosition();
    SetDisplayRotationFromQuaternion(q.data());
    SetDisplayPosition(p.x, p.y, p.z);
}

void DrawGeom::Draw()
{
    if (m_facetedObject) m_facetedObject->Draw();
    if (m_facetedObject2) m_facetedObject2->Draw();
    m_geom->setRedraw(false);
}

