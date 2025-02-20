/*
 *  DataTargetQuaternion.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Tue July 14 2009.
 *  Copyright (c) 1009 Bill Sellers. All rights reserved.
 *
 */

#ifndef DATATARGETQUATERNION_H
#define DATATARGETQUATERNION_H

#include "DataTarget.h"
#include "PGDMath.h"

namespace GaitSym
{

class DataTargetQuaternion : public DataTarget
{
public:
    DataTargetQuaternion();

    void SetTarget(NamedObject *target);
    NamedObject *GetTarget();

    virtual std::string dumpToString() override;

    virtual std::string *createFromAttributes() override;
    virtual void appendToAttributes() override;

    virtual double calculateError(size_t valueListIndex) override;
    virtual double calculateError(size_t index, size_t indexNext, double time) override;
private:

    NamedObject *m_target = nullptr;
    std::vector<pgd::Quaternion> m_qValueList;

};

}

#endif // DATATARGETQUATERNION_H
