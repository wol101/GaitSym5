/*
 *  DrawDataTarget.h
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 11/04/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#ifndef DRAWDATATARGET_H
#define DRAWDATATARGET_H

#include <Drawable.h>

#include <QColor>

#include <memory>

namespace GaitSym { class DataTarget; }
class FacetedObject;
class SimulationWidget;

class DrawDataTarget : public Drawable
{
public:
    DrawDataTarget();
    virtual ~DrawDataTarget();

    virtual void initialise(SimulationWidget *simulationWidget);
    virtual void Draw();
    virtual std::string name();

    void updateEntityPose();   

    GaitSym::DataTarget *dataTarget() const;
    void setDataTarget(GaitSym::DataTarget *newDataTarget);

private:
    GaitSym::DataTarget *m_dataTarget = nullptr;

    std::vector<std::unique_ptr<FacetedObject>> m_facetedObjectStore;

    QColor m_dataTargetColor1;
    size_t m_dataTargetSegments = 0;
    double m_dataTargetSize1 = 0;

};

#endif // DRAWDATATARGET_H
