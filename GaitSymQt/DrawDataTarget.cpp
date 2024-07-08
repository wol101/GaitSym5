/*
 *  DataTarget.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 11/04/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#include "DrawDataTarget.h"
#include "DataTarget.h"
#include "DataTargetVector.h"
#include "FacetedObject.h"
#include "FacetedSphere.h"
#include "Preferences.h"

#include <QString>
#include <QDir>
#include <QDebug>

#include <vector>

DrawDataTarget::DrawDataTarget()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    m_objectCountAtCreation = m_objectCount++;
    std::cerr << m_objectCountAtCreation << " " << className() << " constructed\n";;
#endif
    m_dataTargetColor1 = Preferences::valueQColor("DataTargetColour", QColor("#ffffdead"));
    m_dataTargetSegments = size_t(Preferences::valueInt("DataTargetSegments", 128));
    m_dataTargetSize1 = Preferences::valueDouble("DataTargetRadius", 0.01);
}

DrawDataTarget::~DrawDataTarget()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    std::cerr << m_objectCountAtCreation << " " << className() << " destructed\n";;
#endif
}

std::string DrawDataTarget::name()
{
    if (m_dataTarget) return m_dataTarget->name();
    else return std::string();
}

void DrawDataTarget::initialise(SimulationWidget *simulationWidget)
{
    if (!m_dataTarget) return;

    m_dataTargetColor1.setRedF(qreal(m_dataTarget->colour1().r()));
    m_dataTargetColor1.setGreenF(qreal(m_dataTarget->colour1().g()));
    m_dataTargetColor1.setBlueF(qreal(m_dataTarget->colour1().b()));
    m_dataTargetColor1.setAlphaF(qreal(m_dataTarget->colour1().alpha()));
    m_dataTargetSize1 = m_dataTarget->size1();

    FacetedObject dataTargetObject;
    dataTargetObject.ReadFromResource(":/objects/datatarget.tri");

    if (GaitSym::DataTargetVector *dataTargetVector = dynamic_cast<GaitSym::DataTargetVector *>(m_dataTarget))
    {
        m_facetedObjectStore.reserve(dataTargetVector->valueList()->size());
        m_facetedObjectList.reserve(dataTargetVector->valueList()->size());
        for (size_t i = 0; i < dataTargetVector->valueList()->size(); i++)
        {
            auto facetedObject = std::make_unique<FacetedObject>();
            facetedObject->setSimulationWidget(simulationWidget);
            facetedObject->AddFacetedObject(&dataTargetObject, false, true);
            facetedObject->SetDisplayScale(m_dataTargetSize1, m_dataTargetSize1, m_dataTargetSize1);
            m_facetedObjectList.push_back(facetedObject.get());
            m_facetedObjectStore.push_back(std::move(facetedObject));
        }
        return;
    }

    qDebug() << "Warning in DrawDataTarget::initialise: Unsupported GEOM type";
}

void DrawDataTarget::updateEntityPose()
{
    if (GaitSym::DataTargetVector *dataTargetVector = dynamic_cast<GaitSym::DataTargetVector *>(m_dataTarget))
    {
        for (size_t i = 0; i < dataTargetVector->valueList()->size(); i++)
        {
            m_facetedObjectList[i]->SetDisplayPosition(dataTargetVector->valueList()->at(i));
        }
        return;
    }
}

void DrawDataTarget::Draw()
{
    for (auto &&iter : m_facetedObjectList) { iter->Draw(); }
    m_dataTarget->setRedraw(false);
}


GaitSym::DataTarget *DrawDataTarget::dataTarget() const
{
    return m_dataTarget;
}

void DrawDataTarget::setDataTarget(GaitSym::DataTarget *newDataTarget)
{
    m_dataTarget = newDataTarget;
}
