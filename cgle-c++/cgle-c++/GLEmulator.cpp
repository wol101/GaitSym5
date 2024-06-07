/*
 *  GLEmulator.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 07/07/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "GLEmulator.h"

#include "ExtrusionLib.h"
#include "ExtrusionInternals.h"

#include <algorithm>
#include <cassert>
#include <cmath>

GLEmulator::GLEmulator()
{
    m_matrixIndex = 0;
    makeIdentity(m_matrixStack[m_matrixIndex]);
}

void GLEmulator::multMatrixd(const double m[16])
{
    double r[16];
    multMatrices(m, m_matrixStack[m_matrixIndex], r);
    loadMatrixd(r);
}

void GLEmulator::loadMatrixd(const double m[16])
{
    std::copy_n(m, 16, m_matrixStack[m_matrixIndex]);
}

void GLEmulator::popMatrix()
{
    assert(m_matrixIndex != 0);
    m_matrixIndex--;
}

void GLEmulator::pushMatrix()
{
    assert(m_matrixIndex < 31);
    ++m_matrixIndex;
    loadMatrixd(m_matrixStack[m_matrixIndex - 1]);
}

void GLEmulator::texCoord2d(double x, double y)
{
}

void GLEmulator::beginTriangleStrip(int i, int /*len*/)
{
    beginTriangleStrip(i);
}

void GLEmulator::beginTriangleStrip(int i)
{
    assert(i == GL_TRIANGLE_STRIP);
    assert(m_vertexState == idle);
    m_vertexState = triangleStrip;
}

void GLEmulator::endDraw()
{
    switch (m_vertexState)
    {
    case idle:
        assert(m_vertexState != idle);
        break;
    case triangleStrip:
        assert(m_polygonVertices.size() == 0);
        if (m_triangleStripVertices.size()) { DecodeTriangleStrip(); }
        m_vertexState = idle;
        break;
    case polygon:
        assert(m_triangleStripVertices.size() == 0);
        if (m_polygonVertices.size()) { DecodePolygon(); }
        m_vertexState = idle;
        break;
    }
}

void GLEmulator::normal3dv(double x[3])
{
    m_currentNormal[0] = x[0];
    m_currentNormal[1] = x[1];
    m_currentNormal[2] = x[2];
}

void GLEmulator::vertex3dv(double x[3], int /*j*/, int /*id*/)
{
    vertex3dv(x);
}

void GLEmulator::vertex3dv(double x[3])
{
    switch (m_vertexState)
    {
    case idle:
        assert(m_vertexState != idle);
        break;
    case triangleStrip:
        m_triangleStripVertices.push_back({x[0], x[1], x[2]});
        m_triangleStripColours.push_back(m_currentColour);
        m_triangleStripNormals.push_back(m_currentNormal);
        break;
    case polygon:
        m_polygonVertices.push_back({x[0], x[1], x[2]});
        m_polygonColours.push_back(m_currentColour);
        m_polygonNormals.push_back(m_currentNormal);
        break;
    }
}

void GLEmulator::color3fv(float c[3])
{
    m_currentColour[0] = c[0];
    m_currentColour[1] = c[1];
    m_currentColour[2] = c[2];
}

void GLEmulator::beginContour(int i)
{
    assert(m_vertexState == idle);
    m_vertexState = polygon;
}

void GLEmulator::endContour(int /*i*/)
{
    endDraw();
}

void GLEmulator::contourVertex(int /*i*/, double x[3], void */*p*/)
{
    vertex3dv(x);
}

void GLEmulator::DecodeTriangleStrip()
{
    size_t a = 0;
    size_t b = 1;
    size_t c = 2;
    while (c < m_triangleStripVertices.size())
    {
        m_vertexList.push_back(m_triangleStripVertices[a]);
        m_vertexList.push_back(m_triangleStripVertices[b]);
        m_vertexList.push_back(m_triangleStripVertices[c]);
        m_normalList.push_back(m_triangleStripNormals[a]);
        m_normalList.push_back(m_triangleStripNormals[b]);
        m_normalList.push_back(m_triangleStripNormals[c]);
        m_colourList.push_back(m_triangleStripColours[a]);
        m_colourList.push_back(m_triangleStripColours[b]);
        m_colourList.push_back(m_triangleStripColours[c]);
        m_uvList.push_back({0, 0});
        m_uvList.push_back({0, 0});
        m_uvList.push_back({0, 0});
        a = b;
        b = c;
        ++c;
        if (c >= m_triangleStripVertices.size()) break;
        m_vertexList.push_back(m_triangleStripVertices[c]); // note that the winding is reversed on the secoon triangle
        m_vertexList.push_back(m_triangleStripVertices[b]);
        m_vertexList.push_back(m_triangleStripVertices[a]);
        m_normalList.push_back(m_triangleStripNormals[c]); // note that the winding is reversed on the secoon triangle
        m_normalList.push_back(m_triangleStripNormals[b]);
        m_normalList.push_back(m_triangleStripNormals[a]);
        m_colourList.push_back(m_triangleStripColours[c]); // note that the winding is reversed on the secoon triangle
        m_colourList.push_back(m_triangleStripColours[b]);
        m_colourList.push_back(m_triangleStripColours[a]);
        m_uvList.push_back({0, 0});
        m_uvList.push_back({0, 0});
        m_uvList.push_back({0, 0});
        a = b;
        b = c;
        ++c;
    }

    m_triangleStripColours.clear();
    m_triangleStripNormals.clear();
    m_triangleStripVertices.clear();
}

void GLEmulator::DecodePolygon()
{
    m_polygonColours.clear();
    m_polygonNormals.clear();
    m_polygonVertices.clear();
}

void GLEmulator::makeIdentity(double m[16])
{
    m[0 + 4 * 0] = 1;
    m[0 + 4 * 1] = 0;
    m[0 + 4 * 2] = 0;
    m[0 + 4 * 3] = 0;
    m[1 + 4 * 0] = 0;
    m[1 + 4 * 1] = 1;
    m[1 + 4 * 2] = 0;
    m[1 + 4 * 3] = 0;
    m[2 + 4 * 0] = 0;
    m[2 + 4 * 1] = 0;
    m[2 + 4 * 2] = 1;
    m[2 + 4 * 3] = 0;
    m[3 + 4 * 0] = 0;
    m[3 + 4 * 1] = 0;
    m[3 + 4 * 2] = 0;
    m[3 + 4 * 3] = 1;
}

void GLEmulator::normalize(double v[3])
{
    double r;

    r = std::sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
    if (r == 0.0) return;

    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}

void GLEmulator::cross(double v1[3], double v2[3], double result[3])
{
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void GLEmulator::multMatrixVec(const double matrix[16], const double in[4], double out[4])
{
    int i;

    for (i = 0; i < 4; i++)
    {
        out[i] =
            in[0] * matrix[0 * 4 + i] +
            in[1] * matrix[1 * 4 + i] +
            in[2] * matrix[2 * 4 + i] +
            in[3] * matrix[3 * 4 + i];
    }
}

/*
** Invert 4x4 matrix.
** Contributed by David Moore (See Mesa bug #6748)
*/
int GLEmulator::invertMatrix(const double m[16], double invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] =   m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15]
             + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4] =  -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15]
             - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8] =   m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15]
             + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14]
              - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1] =  -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15]
             - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5] =   m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15]
             + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9] =  -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15]
             - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] =  m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14]
              + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2] =   m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15]
             + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    inv[6] =  -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15]
             - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    inv[10] =  m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15]
              + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14]
              - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    inv[3] =  -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11]
             - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    inv[7] =   m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11]
             + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11]
              - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    inv[15] =  m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10]
              + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (det == 0)
        return 0;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return 1;
}

void GLEmulator::multMatrices(const double a[16], const double b[16], double r[16])
{
    int i, j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            r[i * 4 + j] =
                a[i * 4 + 0] * b[0 * 4 + j] +
                a[i * 4 + 1] * b[1 * 4 + j] +
                a[i * 4 + 2] * b[2 * 4 + j] +
                a[i * 4 + 3] * b[3 * 4 + j];
        }
    }
}

const std::vector<std::array<double, 3> > *GLEmulator::normalList() const
{
    return &m_normalList;
}

const std::vector<std::array<double, 2> > *GLEmulator::uvList() const
{
    return &m_uvList;
}

const std::vector<std::array<double, 3> > *GLEmulator::vertexList() const
{
    return &m_vertexList;
}

const std::vector<std::array<double, 3> > *GLEmulator::colourList() const
{
    return &m_colourList;
}
