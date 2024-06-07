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

    void clear();

    void multMatrixd(const double m[16]);
    void loadMatrixd(const double m[16]);
    void popMatrix();
    void pushMatrix();

    void texCoord2d(double x, double y);
    void beginTriangleStrip();
    void endDraw();
    void normal3dv(const double *x);
    void vertex3dv(const double *x);
    void color3fv(const float *c);

    void beginContour();
    void endContour();

    const std::vector<std::array<double, 3> > *colourList() const;

    const std::vector<std::array<double, 3> > *vertexList() const;

    const std::vector<std::array<double, 2> > *uvList() const;

    const std::vector<std::array<double, 3> > *normalList() const;

private:
    enum VertexState { idle, triangleStrip, polygon };

    void DecodeTriangleStrip();
    void DecodePolygon();

    static void makeIdentity(double m[16]);
    static void normalize(double v[3]);
    static void cross(double v1[3], double v2[3], double result[3]);
    static void multMatrixVec(const double matrix[16], const double in[4], double out[4]);
    static void multMatrixVec3(const double matrix[16], const double in[3], double out[3]);
    static void multMatrixVec34(const double matrix[16], const double in[3], double out[4]);
    static int invertMatrix(const double m[16], double invOut[16]);
    static void multMatrices(const double a[16], const double b[16], double r[16]);

    double m_matrixStack[32][16];
    size_t m_matrixIndex = 0;

    std::array<double, 3> m_currentColour;
    std::array<double, 3> m_currentNormal;

    std::vector<std::array<double, 3>> m_colourList;
    std::vector<std::array<double, 3>> m_normalList;
    std::vector<std::array<double, 3>> m_vertexList;
    std::vector<std::array<double, 2>> m_uvList;

    std::vector<std::array<double, 3>> m_triangleStripVertices;
    std::vector<std::array<double, 3>> m_triangleStripNormals;
    std::vector<std::array<double, 3>> m_triangleStripColours;

    std::vector<std::array<double, 3>> m_polygonVertices;
    std::vector<std::array<double, 3>> m_polygonNormals;
    std::vector<std::array<double, 3>> m_polygonColours;

    VertexState m_vertexState = idle;
};


#endif // GLEMULATOR_H
