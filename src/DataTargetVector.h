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

    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    virtual double calculateError(size_t valueListIndex);
    virtual double calculateError(double time);

    std::vector<pgd::Vector3> *valueList();

private:

    NamedObject *m_target = nullptr;
    std::vector<pgd::Vector3> m_valueList;

};

}

#endif // DATATARGETVECTOR_H
