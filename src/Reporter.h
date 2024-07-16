/*
 *  Reporter.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 06/01/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef REPORTER_H
#define REPORTER_H

#include "NamedObject.h"

namespace GaitSym
{

class SimulationWindow;

class Reporter : public NamedObject
{
public:
    Reporter();

    virtual bool ShouldAbort() { return false; }

    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

};

}

#endif // REPORTER_H
