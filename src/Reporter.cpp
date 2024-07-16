/*
 *  Reporter.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 06/01/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include "Reporter.h"

using namespace std::string_literals;

namespace GaitSym {

Reporter::Reporter()
{
}

std::string *Reporter::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    return nullptr;
}

void Reporter::saveToAttributes()
{
    this->setTag("REPORTER"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

void Reporter::appendToAttributes()
{
    NamedObject::appendToAttributes();
}

} // namespace GaitSym


