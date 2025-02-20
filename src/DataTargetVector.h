/*
 *  DataTargetVector.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Tue July 14 2009.
 *  Copyright (c) 1009 Bill Sellers. All rights reserved.
 *
 */

#ifndef DATATARGETVECTOR_H
#define DATATARGETVECTOR_H

#include "DataTarget.h"
#include "PGDMath.h"

namespace GaitSym
{

class DataTargetVector : public DataTarget
{
public:
    DataTargetVector();

    void SetTarget(NamedObject *target);
    NamedObject *GetTarget();

    virtual std::string dumpToString() override;

    virtual std::string *createFromAttributes() override;
    virtual void appendToAttributes() override;

    virtual double calculateError(size_t valueListIndex) override;
    virtual double calculateError(size_t index, size_t indexNext, double time) override;

    std::vector<pgd::Vector3> *valueList();

private:

    NamedObject *m_target = nullptr;
    std::vector<pgd::Vector3> m_valueList;
    pgd::Vector3 m_vectorValue;
    pgd::Vector3 m_vectorTarget;
    pgd::Vector3 m_vectorError;
};

}

#endif // DATATARGETVECTOR_H
