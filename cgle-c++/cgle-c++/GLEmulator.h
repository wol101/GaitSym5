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
#include <array>

class GLEmulator
{
public:
    GLEmulator();

    void multMatrixd(const double m[16]);
    void loadMatrixd(const double m[16]);
    void popMatrix();
    void pushMatrix();

    void addPolygon(double polygon[][3], size_t numVertices);
    void texCoord2d(double x, double y);
    void beginTriangleStrip(int i, int len);
    void beginTriangleStrip(int i);
    void end();
    void normal3dv(double x[3]);
    void vertex3dv(double x[3], int j, int id);
    void vertex3dv(double x[3]);
    void color3fv(float c[3]);

    void beginContour(int i);
    void endContour(int i);
    void contourVertex(int i, double x[3], void *p);

private:
    static void makeIdentity(double m[16]);
    static void normalize(double v[3]);
    static void cross(double v1[3], double v2[3], double result[3]);
    void multMatrixVec(const double matrix[16], const double in[4], double out[4]);
    int invertMatrix(const double m[16], double invOut[16]);
    void multMatrices(const double a[16], const double b[16], double r[16]);

    double m_matrixStack[32][16];
    size_t m_matrixIndex = 0;

    std::vector<std::array<double, 3>> m_vertexList;
    std::vector<std::array<size_t, 3>> m_triangleList;
};

#endif // GLEMULATOR_H
