/*
 *  Drivable.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 6/3/2011.
 *  Copyright 2011 Bill Sellers. All rights reserved.
 *
 */

#ifndef DRIVABLE_H
#define DRIVABLE_H

#include <cstdint>
#include <limits>

namespace GaitSym
{

class Drivable
{
public:
    Drivable();
    virtual ~Drivable();

    virtual void ReceiveData(double receivedData, int64_t receiveDataStepCount);

protected:
    double dataSum() const;
    void setDataSum(double dataSum);

private:
    double m_dataSum = 0;
    int64_t m_receiveDataStepCount = std::numeric_limits<int64_t>::min();
};

}

#endif // DRIVABLE_H
