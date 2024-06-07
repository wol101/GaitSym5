/*
 *  GLEmulator.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 07/07/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef GLEMULATOR_H
#define GLEMULATOR_H

#include <vector>

class GLEmulator
{
public:
    GLEmulator();

    void multMatrix(const double m[16]);
    void loadMatrix(const double m[16]);
    void popMatrix();
    void pushMatrix();

private:
    static void makeIdentity(double m[16]);
    static void normalize(double v[3]);
    static void cross(double v1[3], double v2[3], double result[3]);
    void multMatrixVec(const double matrix[16], const double in[4], double out[4]);
    int invertMatrix(const double m[16], double invOut[16]);
    void multMatrices(const double a[16], const double b[16], double r[16]);

    std::vector<double[16]> m_matrixStack;
};

#endif // GLEMULATOR_H
