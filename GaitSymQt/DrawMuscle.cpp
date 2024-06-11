/*
 *  DrawMuscle.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 19/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include "DrawMuscle.h"
#include "Muscle.h"
#include "FacetedObject.h"
#include "Muscle.h"
#include "TwoPointStrap.h"
#include "NPointStrap.h"
#include "CylinderWrapStrap.h"
#include "FacetedPolyline.h"
#include "FacetedPolyCone.h"
#include "PGDMath.h"
#include "Preferences.h"
#include "DampedSpringMuscle.h"
#include "MAMuscle.h"
#include "MAMuscleComplete.h"
#include "Marker.h"
#include "TwoCylinderWrapStrap.h"

#include <QString>
#include <QDir>
#include <QDebug>

DrawMuscle::DrawMuscle()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    m_objectCountAtCreation = m_objectCount++;
    std::cerr << m_objectCountAtCreation << " " << className() << " constructed\n";;
#endif
    m_strapRadius = Preferences::valueDouble("StrapRadius");
    m_strapNumSegments = size_t(Preferences::valueInt("StrapSegments"));
    m_strapCylinderLength = Preferences::valueDouble("StrapCylinderLength");
    m_strapCylinderSegments = size_t(Preferences::valueDouble("StrapCylinderSegments"));
    m_strapCylinderWrapSegments = size_t(Preferences::valueInt("StrapCylinderWrapSegments"));
    m_displayMuscleForces = Preferences::valueBool("DisplayMuscleForces");
    m_strapForceScale = Preferences::valueDouble("StrapForceScale");
    m_strapForceRadius = Preferences::valueDouble("StrapForceRadius");
    m_strapForceColor = Preferences::valueQColor("StrapForceColour");
    m_strapColor = Preferences::valueQColor("StrapColour");
    m_strapCylinderColor = Preferences::valueQColor("StrapCylinderColour");
    m_strapColourMap = GaitSym::Colour::ColourMap(Preferences::valueInt("StrapColourMap"));
    m_strapNumSections = size_t(Preferences::valueInt("StrapSections"));
    m_muscleDrawStyle = MuscleDrawStyle(Preferences::valueInt("MuscleDrawStyle"));
}

DrawMuscle::~DrawMuscle()
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(GAITSYM_MEMORY_ALLOCATION_DEBUG)
    std::cerr << m_objectCountAtCreation << " " << className() << " destructed\n";;
#endif
}


void DrawMuscle::initialise(SimulationWidget *simulationWidget)
{
    if (!m_muscle) return;
    m_simulationWidget = simulationWidget;

    GaitSym::Colour colour(m_muscle->GetStrap()->colour1());
    switch (m_muscle->strapColourControl())
    {
    case GaitSym::Muscle::fixedColour:
        m_strapColor.setRedF(qreal(m_muscle->GetStrap()->colour1().r()));
        m_strapColor.setGreenF(qreal(m_muscle->GetStrap()->colour1().g()));
        m_strapColor.setBlueF(qreal(m_muscle->GetStrap()->colour1().b()));
        m_strapColor.setAlphaF(qreal(m_muscle->GetStrap()->colour1().alpha()));
        break;
    case GaitSym::Muscle::activationMap:
        GaitSym::Colour::SetColourFromMap(float(m_muscle->GetActivation()), m_strapColourMap, &colour, false);
        m_strapColor = QColor(QString::fromStdString(colour.GetHexARGB()));
        break;
    case GaitSym::Muscle::strainMap:
        if (dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle)) GaitSym::Colour::SetColourFromMap(float(m_muscle->GetLength() / dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle)->GetUnloadedLength()) - 0.5f, m_strapColourMap, &colour, false);
        else if (dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)) GaitSym::Colour::SetColourFromMap( float(m_muscle->GetLength() / (dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)->fibreLength() + dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)->tendonLength())) - 0.5f, m_strapColourMap, &colour, false);
        else if (dynamic_cast<GaitSym::MAMuscle *>(m_muscle)) GaitSym::Colour::SetColourFromMap(float(m_muscle->GetLength() / (dynamic_cast<GaitSym::MAMuscle *>(m_muscle)->fibreLength())) - 0.5f, m_strapColourMap, &colour, false);
        m_strapColor = QColor(QString::fromStdString(colour.GetHexARGB()));
        break;
    case GaitSym::Muscle::forceMap:
        if (dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle)) GaitSym::Colour::SetColourFromMap( float(m_muscle->GetTension() / (dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle)->GetUnloadedLength() * dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle)->GetSpringConstant())), m_strapColourMap, &colour, false);
        else if (dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)) GaitSym::Colour::SetColourFromMap( float(m_muscle->GetTension() / (dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)->forcePerUnitArea() * dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle)->pca())), m_strapColourMap, &colour, false);
        else if (dynamic_cast<GaitSym::MAMuscle *>(m_muscle)) GaitSym::Colour::SetColourFromMap( float(m_muscle->GetLength() / (dynamic_cast<GaitSym::MAMuscle *>(m_muscle)->forcePerUnitArea() * dynamic_cast<GaitSym::MAMuscle *>(m_muscle)->pca())), m_strapColourMap, &colour, false);
        m_strapColor = QColor(QString::fromStdString(colour.GetHexARGB()));
        break;
    }
    m_strapCylinderColor.setRedF(qreal(m_muscle->GetStrap()->colour2().r()));
    m_strapCylinderColor.setGreenF(qreal(m_muscle->GetStrap()->colour2().g()));
    m_strapCylinderColor.setBlueF(qreal(m_muscle->GetStrap()->colour2().b()));
    m_strapCylinderColor.setAlphaF(qreal(m_muscle->GetStrap()->colour2().alpha()));

    m_strapForceColor.setRedF(qreal(m_muscle->colour1().r()));
    m_strapForceColor.setGreenF(qreal(m_muscle->colour1().g()));
    m_strapForceColor.setBlueF(qreal(m_muscle->colour1().b()));
    m_strapForceColor.setAlphaF(qreal(m_muscle->colour1().alpha()));

    m_strapRadius = m_muscle->GetStrap()->size1();
    m_strapCylinderLength  = m_muscle->GetStrap()->size2();
    m_strapForceRadius  = m_muscle->size1();
    m_strapForceScale  = m_muscle->size2();


    while (true)
    {

        if (GaitSym::TwoPointStrap *twoPointStrap = dynamic_cast<GaitSym::TwoPointStrap *>(m_muscle->GetStrap()))
        {
            std::vector<std::unique_ptr<GaitSym::PointForce >> *pointForceList = twoPointStrap->GetPointForceList();
            m_polyline.reserve(pointForceList->size());
            m_polyline.push_back(pgd::Vector3(pointForceList->at(0)->point[0], pointForceList->at(0)->point[1], pointForceList->at(0)->point[2]));
            m_polyline.push_back(pgd::Vector3(pointForceList->at(1)->point[0], pointForceList->at(1)->point[1], pointForceList->at(1)->point[2]));
            // m_facetedObject1 = std::make_unique<FacetedPolyline>(&polyline, m_strapRadius, m_strapNumSegments, m_strapColor, 1);
            // m_facetedObject1->setSimulationWidget(simulationWidget);
            // m_facetedObjectList.push_back(m_facetedObject1.get());
            break;
        }

        if (GaitSym::NPointStrap *nPointStrap = dynamic_cast<GaitSym::NPointStrap *>(m_muscle->GetStrap()))
        {
            std::vector<std::unique_ptr<GaitSym::PointForce >> *pointForceList = nPointStrap->GetPointForceList();
            m_polyline.reserve(pointForceList->size());
            m_polyline.push_back(pgd::Vector3(pointForceList->at(0)->point[0], pointForceList->at(0)->point[1], pointForceList->at(0)->point[2]));
            for (size_t i = 2; i < pointForceList->size(); i++) m_polyline.push_back(pgd::Vector3(pointForceList->at(i)->point[0], pointForceList->at(i)->point[1], pointForceList->at(i)->point[2]));
            m_polyline.push_back(pgd::Vector3(pointForceList->at(1)->point[0], pointForceList->at(1)->point[1], pointForceList->at(1)->point[2]));
            // m_facetedObject1 = std::make_unique<FacetedPolyline>(&polyline, m_strapRadius, m_strapNumSegments, m_strapColor, 1);
            // m_facetedObject1->setSimulationWidget(simulationWidget);
            // m_facetedObjectList.push_back(m_facetedObject1.get());
            break;
        }

        if (GaitSym::CylinderWrapStrap *cylinderWrapStrap = dynamic_cast<GaitSym::CylinderWrapStrap *>(m_muscle->GetStrap()))
        {
            if (cylinderWrapStrap->GetNumWrapSegments() != int(m_strapCylinderWrapSegments))
            {
                cylinderWrapStrap->SetNumWrapSegments(int(m_strapCylinderWrapSegments));
                cylinderWrapStrap->Calculate();
            }
            m_polyline = *cylinderWrapStrap->GetPathCoordinates();
            // if (m_polyline.size())
            // {
            //     m_facetedObject1 = std::make_unique<FacetedPolyline>(&polyline, m_strapRadius, m_strapNumSegments, m_strapColor, 1);
            //     m_facetedObject1->setSimulationWidget(simulationWidget);
            //     m_facetedObjectList.push_back(m_facetedObject1.get());
            // }

            pgd::Vector3 position = cylinderWrapStrap->GetCylinderMarker()->GetWorldPosition();
            pgd::Vector3 cylinderVecWorld = pgd::QVRotate(cylinderWrapStrap->GetCylinderMarker()->GetWorldQuaternion(), pgd::Vector3(m_strapCylinderLength / 2, 0, 0));
            double radius = cylinderWrapStrap->cylinderRadius();
            // and draw it
            std::vector<pgd::Vector3> polyline;
            polyline.push_back(pgd::Vector3(position[0] - cylinderVecWorld.x, position[1] - cylinderVecWorld.y, position[2] - cylinderVecWorld.z));
            polyline.push_back(pgd::Vector3(position[0] + cylinderVecWorld.x, position[1] + cylinderVecWorld.y, position[2] + cylinderVecWorld.z));
            m_facetedObject2 = std::make_unique<FacetedPolyline>(&polyline, radius, m_strapCylinderSegments, m_strapCylinderColor, 1, true);
            m_facetedObject2->setSimulationWidget(simulationWidget);
            m_facetedObjectList.push_back(m_facetedObject2.get());
            break;
        }

        if (GaitSym::TwoCylinderWrapStrap *twoCylinderWrapStrap = dynamic_cast<GaitSym::TwoCylinderWrapStrap *>(m_muscle->GetStrap()))
        {
            if (twoCylinderWrapStrap->GetNumWrapSegments() != int(m_strapCylinderWrapSegments))
            {
                twoCylinderWrapStrap->SetNumWrapSegments(int(m_strapCylinderWrapSegments));
                twoCylinderWrapStrap->Calculate();
            }
            std::vector<pgd::Vector3> m_polyline = *twoCylinderWrapStrap->GetPathCoordinates();
            // if (polyline.size())
            // {
            //     m_facetedObject1 = std::make_unique<FacetedPolyline>(&polyline, m_strapRadius, m_strapNumSegments, m_strapColor, 1);
            //     m_facetedObject1->setSimulationWidget(simulationWidget);
            //     m_facetedObjectList.push_back(m_facetedObject1.get());
            // }
            pgd::Vector3 position = twoCylinderWrapStrap->GetCylinder1Marker()->GetWorldPosition();
            pgd::Vector3 cylinderVecWorld = pgd::QVRotate(twoCylinderWrapStrap->GetCylinder1Marker()->GetWorldQuaternion(), pgd::Vector3(m_strapCylinderLength / 2, 0, 0));
            double radius = twoCylinderWrapStrap->Cylinder1Radius();
            // and draw it
            std::vector<pgd::Vector3> polyline;
            polyline.push_back(pgd::Vector3(position[0] - cylinderVecWorld.x, position[1] - cylinderVecWorld.y, position[2] - cylinderVecWorld.z));
            polyline.push_back(pgd::Vector3(position[0] + cylinderVecWorld.x, position[1] + cylinderVecWorld.y, position[2] + cylinderVecWorld.z));
            m_facetedObject2 = std::make_unique<FacetedPolyline>(&polyline, radius, m_strapCylinderSegments, m_strapCylinderColor, 1, true);
            m_facetedObject2->setSimulationWidget(simulationWidget);
            m_facetedObjectList.push_back(m_facetedObject2.get());
            position = twoCylinderWrapStrap->GetCylinder2Marker()->GetWorldPosition();
            radius = twoCylinderWrapStrap->Cylinder2Radius();
            // and draw it
            polyline.clear();
            polyline.push_back(pgd::Vector3(position[0] - cylinderVecWorld.x, position[1] - cylinderVecWorld.y, position[2] - cylinderVecWorld.z));
            polyline.push_back(pgd::Vector3(position[0] + cylinderVecWorld.x, position[1] + cylinderVecWorld.y, position[2] + cylinderVecWorld.z));
            m_facetedObject3 = std::make_unique<FacetedPolyline>(&polyline, radius, m_strapCylinderSegments, m_strapCylinderColor, 1, true);
            m_facetedObject3->setSimulationWidget(simulationWidget);
            m_facetedObjectList.push_back(m_facetedObject3.get());
            break;
        }
        qDebug() << "Error in DrawMuscle::initialise: Unsupported STRAP type";
        break;
    }

    // the muscle visualisation depends on the type
    switch (m_muscleDrawStyle)
    {
    case FixedCylinder:
    case AreaCylinder:
    case VolumeCylinder:
        Cylinder();
        break;
    case FixedFusiform:
    case AreaFusiform:
    case VolumeFusiform:
        Fusiform();
        break;
    }

    if (m_displayMuscleForces)
    {
        std::vector<std::unique_ptr<GaitSym::PointForce >> *pointForceList = m_muscle->GetPointForceList();
        for (size_t i = 0; i < pointForceList->size(); i++)
        {
            std::vector<pgd::Vector3> polyline;
            polyline.reserve(2);
            polyline.clear();
            pgd::Vector3 f = pgd::Vector3(pointForceList->at(i)->vector[0], pointForceList->at(i)->vector[1], pointForceList->at(i)->vector[2]) * m_muscle->GetTension() * m_strapForceScale;
            polyline.push_back(pgd::Vector3(pointForceList->at(i)->point[0], pointForceList->at(i)->point[1], pointForceList->at(i)->point[2]));
            polyline.push_back(pgd::Vector3(pointForceList->at(i)->point[0], pointForceList->at(i)->point[1], pointForceList->at(i)->point[2]) + f);
            std::unique_ptr<FacetedPolyline>facetedPolyline = std::make_unique<FacetedPolyline>(&polyline, m_strapForceRadius, m_strapNumSegments, m_strapForceColor, 1, true);
            facetedPolyline->setSimulationWidget(simulationWidget);
            m_facetedObjectForceList.push_back(std::move(facetedPolyline));
            m_facetedObjectList.push_back(facetedPolyline.get());
        }
    }

//    qDebug() << "DrawMuscle::initialise: finished " << m_muscle->name().c_str();

    return;
}

void DrawMuscle::Draw()
{
    if (m_facetedObject1.get()) m_facetedObject1->Draw();
    if (m_facetedObject2.get()) m_facetedObject2->Draw();
    if (m_facetedObject3.get()) m_facetedObject3->Draw();
    for (size_t i = 0; i < m_facetedObjectForceList.size(); i++)
    {
        m_facetedObjectForceList.at(i)->Draw();
    }
    m_muscle->setRedraw(false);
    m_muscle->GetStrap()->setRedraw(false);
}

void DrawMuscle::Cylinder()
{
    if (m_polyline.size() < 2) return;
    double radius;
    while (true)
    {
        if (m_muscleDrawStyle == FixedCylinder)
        {
            radius = m_strapRadius;
            break;
        }
        if (GaitSym::MAMuscle *maMuscle = dynamic_cast<GaitSym::MAMuscle *>(m_muscle))
        {
            switch (m_muscleDrawStyle)
            {
            case VolumeCylinder:
                radius = std::sqrt((maMuscle->pca() * maMuscle->fibreLength())/ (M_PI * maMuscle->GetLength()));
                break;
            case AreaCylinder:
                radius = std::sqrt(maMuscle->pca()/ M_PI);
                break;
            default:
                assert(false);
                break;
            }
            break;
        }
        if (GaitSym::MAMuscleComplete *maMuscleComplete = dynamic_cast<GaitSym::MAMuscleComplete *>(m_muscle))
        {
            switch (m_muscleDrawStyle)
            {
            case VolumeCylinder:
                radius = std::sqrt((maMuscleComplete->pca() * maMuscleComplete->fibreLength())/ (M_PI * maMuscleComplete->GetLength()));
                break;
            case AreaCylinder:
                radius = std::sqrt(maMuscleComplete->pca()/ M_PI);
                break;
            default:
                assert(false);
            }
            break;
        }
        if (GaitSym::DampedSpringMuscle *dampedSpringMuscle = dynamic_cast<GaitSym::DampedSpringMuscle *>(m_muscle))
        {
            switch (m_muscleDrawStyle)
            {
            case VolumeCylinder:
                radius = std::sqrt((dampedSpringMuscle->GetArea() * dampedSpringMuscle->GetUnloadedLength())/ (M_PI * dampedSpringMuscle->GetLength()));
                break;
            case AreaCylinder:
                radius = std::sqrt(dampedSpringMuscle->GetArea()/ M_PI);
                break;
            default:
                assert(false);
            }
            break;
        }
        break;
    }
    m_facetedObject1 = std::make_unique<FacetedPolyline>(&m_polyline, radius, m_strapNumSegments, m_strapColor, 1, false);
    m_facetedObject1->setSimulationWidget(m_simulationWidget);
    m_facetedObjectList.push_back(m_facetedObject1.get());
}


void DrawMuscle::Fusiform()
{
    std::vector<pgd::Vector3> vertexList;
    std::vector<std::array<float, 3>> vertexColours;
    std::vector<double> radiusList;
    while (true)
    {
        if (GaitSym::MAMuscle *maMuscle = dynamic_cast<GaitSym::MAMuscle *>(m_muscle))
        {
            // shape is a volume of rotation about the x axis of y=(a/2)(cos(x)+1) from -pi to +pi
            // volume is integral(pi*(a/2)(cos(x)+1)^2) which is (3*pi^2*a^2)/4
            // so a = (2*sqrt(volume))/(sqrt(3)*pi)
            double volume, a;
            switch (m_muscleDrawStyle)
            {
            case VolumeCylinder:
                volume = 2 * M_PI * maMuscle->pca() * maMuscle->fibreLength() /  maMuscle->GetLength();
                a = (2*sqrt(volume))/(std::sqrt(3)*M_PI);
                break;
            case AreaCylinder:
                volume = 2 * M_PI * maMuscle->pca();
                a = (2*sqrt(volume))/(std::sqrt(3)*M_PI);
                break;
            case FixedCylinder:
                a = m_strapRadius;
                break;
            default:
                assert(false);
                break;
            }

            double delta = maMuscle->GetLength() / m_strapNumSections;
            double distance = 0;
            for (size_t i = 0; i < m_polyline.size() - 1; i++)
            {
                pgd::Vector3 vec = m_polyline[i + 1] - m_polyline[i];
                double vecLen = vec.Magnitude();
                if (vecLen <= delta)
                {
                    vertexList.push_back(m_polyline[i]);
                    vertexColours.push_back({m_strapColor.redF(), m_strapColor.greenF(), m_strapColor.blueF()});
                    double x = (distance / maMuscle->GetLength()) * ( 2 * M_PI) - M_PI;
                    double radius = (a/2) * (std::cos(x)+1);
                    radiusList.push_back(radius);
                    distance += vecLen;
                    continue;
                }
                size_t subdivisions = int(std::ceil(vecLen / delta));
                double newDelta = vecLen / subdivisions;
                pgd::Vector3 newVec = vec / subdivisions;
                for (size_t j = 0; j < subdivisions; j++)
                {
                    vertexList.push_back(m_polyline[i] + j * newVec);
                    vertexColours.push_back({m_strapColor.redF(), m_strapColor.greenF(), m_strapColor.blueF()});
                    double x = (distance / maMuscle->GetLength()) * ( 2 * M_PI) - M_PI;
                    double radius = (a/2) * (std::cos(x)+1);
                    radiusList.push_back(radius);
                    distance += newDelta;
                }
            }
            vertexList.push_back(m_polyline.back());
            vertexColours.push_back({m_strapColor.redF(), m_strapColor.greenF(), m_strapColor.blueF()});
            double x = (distance / maMuscle->GetLength()) * ( 2 * M_PI) - M_PI;
            double radius = (a/2) * (std::cos(x)+1);
            radiusList.push_back(radius);
            m_facetedObject1 = std::make_unique<FacetedPolyCone>(vertexList, vertexColours, radiusList, m_strapNumSegments, m_strapColor, 1.0);
            m_facetedObject1->setSimulationWidget(m_simulationWidget);
            m_facetedObjectList.push_back(m_facetedObject1.get());
            break;
        }

        qDebug() << "Error in DrawMuscle::initialise: Unsupported MUSCLE type";
        break;
    }
}

std::string DrawMuscle::name()
{
    if (m_muscle) return m_muscle->name();
    else return std::string();
}

GaitSym::Muscle *DrawMuscle::muscle() const
{
    return m_muscle;
}

double DrawMuscle::strapRadius() const
{
    return m_strapRadius;
}

void DrawMuscle::setStrapRadius(double strapRadius)
{
    m_strapRadius = strapRadius;
}

size_t DrawMuscle::strapNumSegments() const
{
    return m_strapNumSegments;
}

void DrawMuscle::setStrapNumSegments(size_t strapNumSegments)
{
    m_strapNumSegments = strapNumSegments;
}

double DrawMuscle::strapCylinderLength() const
{
    return m_strapCylinderLength;
}

void DrawMuscle::setStrapCylinderLength(double strapCylinderLength)
{
    m_strapCylinderLength = strapCylinderLength;
}

bool DrawMuscle::displayMuscleForces() const
{
    return m_displayMuscleForces;
}

void DrawMuscle::setDisplayMuscleForces(bool displayMuscleForces)
{
    m_displayMuscleForces = displayMuscleForces;
}

double DrawMuscle::strapForceScale() const
{
    return m_strapForceScale;
}

void DrawMuscle::setStrapForceScale(double strapForceScale)
{
    m_strapForceScale = strapForceScale;
}

double DrawMuscle::strapForceRadius() const
{
    return m_strapForceRadius;
}

void DrawMuscle::setStrapForceRadius(double strapForceRadius)
{
    m_strapForceRadius = strapForceRadius;
}

QColor DrawMuscle::strapForceColor() const
{
    return m_strapForceColor;
}

void DrawMuscle::setStrapForceColor(const QColor &strapForceColor)
{
    m_strapForceColor = strapForceColor;
}

QColor DrawMuscle::strapColor() const
{
    return m_strapColor;
}

void DrawMuscle::setStrapColor(const QColor &strapColor)
{
    m_strapColor = strapColor;
}

QColor DrawMuscle::strapCylinderColor() const
{
    return m_strapCylinderColor;
}

void DrawMuscle::setStrapCylinderColor(const QColor &strapCylinderColor)
{
    m_strapCylinderColor = strapCylinderColor;
}

GaitSym::Colour::ColourMap DrawMuscle::strapColourMap() const
{
    return m_strapColourMap;
}

void DrawMuscle::setStrapColourMap(const GaitSym::Colour::ColourMap &strapColourMap)
{
    m_strapColourMap = strapColourMap;
}

void DrawMuscle::setMuscle(GaitSym::Muscle *muscle)
{
    m_muscle = muscle;
}
