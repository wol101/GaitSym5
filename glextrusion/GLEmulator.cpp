/*
 *  GLEmulator.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 07/07/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "GLEmulator.h"

#include <algorithm>
#include <cassert>
#include <cmath>

GLEmulator glEmulator;

GLEmulator::GLEmulator()
{
    m_matrixIndex = 0;
    makeIdentity(m_matrixStack[m_matrixIndex]);
}

void GLEmulator::clear()
{
    m_matrixIndex = 0;
    makeIdentity(m_matrixStack[m_matrixIndex]);

    m_currentColour = {0, 0, 0};
    m_currentNormal = {0, 0, 0};
    m_currentUV = {0, 0};

    m_colourList.clear();
    m_normalList.clear();
    m_vertexList.clear();
    m_uvList.clear();

    m_triangleStripVertices.clear();
    m_triangleStripNormals.clear();
    m_triangleStripColours.clear();

    m_polygonVertices.clear();
    m_polygonNormals.clear();
    m_polygonColours.clear();

    m_vertexState = idle;
}

void GLEmulator::reserve(size_t size)
{
    m_colourList.reserve(size);
    m_normalList.reserve(size);
    m_vertexList.reserve(size);
    m_uvList.reserve(size);

    m_triangleStripVertices.reserve(size);
    m_triangleStripNormals.reserve(size);
    m_triangleStripColours.reserve(size);
    m_triangleStripUVs.reserve(size);

    m_polygonVertices.reserve(size);
    m_polygonNormals.reserve(size);
    m_polygonColours.reserve(size);
    m_polygonUVs.reserve(size);
}

void GLEmulator::glMultMatrixd(const double m[16])
{
    double r[16];
    multMatrices(m, m_matrixStack[m_matrixIndex], r);
    glLoadMatrixd(r);
}

void GLEmulator::glLoadMatrixd(const double m[16])
{
    std::copy_n(m, 16, m_matrixStack[m_matrixIndex]);
}

void GLEmulator::glPopMatrix()
{
    assert(m_matrixIndex != 0);
    --m_matrixIndex;
}

void GLEmulator::glPushMatrix()
{
    assert(m_matrixIndex < 30);
    ++m_matrixIndex;
    glLoadMatrixd(m_matrixStack[m_matrixIndex - 1]);
}

void GLEmulator::glTexCoord2d(double x, double y)
{
    m_currentUV[0] = x;
    m_currentUV[1] = y;
}

void GLEmulator::glBeginTriangleStrip()
{
    assert(m_vertexState == idle);
    m_vertexState = triangleStrip;
}

void GLEmulator::glEnd()
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

void GLEmulator::glNormal3dv(const double *x)
{
    double r[4];
    multMatrixVec34(m_matrixStack[m_matrixIndex], x, r);
    normalize(r);
    m_currentNormal[0] = r[0];
    m_currentNormal[1] = r[1];
    m_currentNormal[2] = r[2];
}

void GLEmulator::glVertex3dv(const double *x)
{
    double r[3];
    multMatrixVec3(m_matrixStack[m_matrixIndex], x, r);
    switch (m_vertexState)
    {
    case idle:
        assert(m_vertexState != idle);
        break;
    case triangleStrip:
        m_triangleStripVertices.push_back({r[0], r[1], r[2]});
        m_triangleStripColours.push_back(m_currentColour);
        m_triangleStripNormals.push_back(m_currentNormal);
        m_triangleStripUVs.push_back(m_currentUV);
        break;
    case polygon:
        m_polygonVertices.push_back({r[0], r[1], r[2]});
        m_polygonColours.push_back(m_currentColour);
        m_polygonNormals.push_back(m_currentNormal);
        m_polygonUVs.push_back(m_currentUV);
        break;
    }
}

void GLEmulator::glColor3fv(const float *c)
{
    m_currentColour[0] = c[0];
    m_currentColour[1] = c[1];
    m_currentColour[2] = c[2];
    m_currentColour[3] = 1;
}

void GLEmulator::glColor4fv(const float *c)
{
    m_currentColour[0] = c[0];
    m_currentColour[1] = c[1];
    m_currentColour[2] = c[2];
    m_currentColour[3] = c[3];
}

void GLEmulator::glBeginPolygon()
{
    assert(m_vertexState == idle);
    m_vertexState = polygon;
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
        m_uvList.push_back(m_triangleStripUVs[a]);
        m_uvList.push_back(m_triangleStripUVs[b]);
        m_uvList.push_back(m_triangleStripUVs[c]);
        a = b;
        b = c;
        ++c;
        if (c >= m_triangleStripVertices.size()) break;
        m_vertexList.push_back(m_triangleStripVertices[c]); // note that the winding is reversed on the second triangle
        m_vertexList.push_back(m_triangleStripVertices[b]);
        m_vertexList.push_back(m_triangleStripVertices[a]);
        m_normalList.push_back(m_triangleStripNormals[c]); // note that the winding is reversed on the second triangle
        m_normalList.push_back(m_triangleStripNormals[b]);
        m_normalList.push_back(m_triangleStripNormals[a]);
        m_colourList.push_back(m_triangleStripColours[c]); // note that the winding is reversed on the second triangle
        m_colourList.push_back(m_triangleStripColours[b]);
        m_colourList.push_back(m_triangleStripColours[a]);
        m_uvList.push_back(m_triangleStripUVs[c]); // note that the winding is reversed on the second triangle
        m_uvList.push_back(m_triangleStripUVs[b]);
        m_uvList.push_back(m_triangleStripUVs[a]);
        a = b;
        b = c;
        ++c;
    }

    m_triangleStripColours.clear();
    m_triangleStripNormals.clear();
    m_triangleStripVertices.clear();
}

void GLEmulator::DecodePolygon() // note that this version only copes with convex polygons
{
    size_t a = 0;
    size_t b = 1;
    size_t c = 2;
    while (c < m_polygonVertices.size())
    {
        m_vertexList.push_back(m_polygonVertices[a]);
        m_vertexList.push_back(m_polygonVertices[b]);
        m_vertexList.push_back(m_polygonVertices[c]);
        m_normalList.push_back(m_polygonNormals[a]);
        m_normalList.push_back(m_polygonNormals[b]);
        m_normalList.push_back(m_polygonNormals[c]);
        m_colourList.push_back(m_polygonColours[a]);
        m_colourList.push_back(m_polygonColours[b]);
        m_colourList.push_back(m_polygonColours[c]);
        m_uvList.push_back(m_polygonUVs[a]);
        m_uvList.push_back(m_polygonUVs[b]);
        m_uvList.push_back(m_polygonUVs[c]);
        ++b;
        ++c;
    }
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

void GLEmulator::multMatrixVec3(const double matrix[16], const double in[3], double out[3])
{
    int i;
    double r[4];
    for (i = 0; i < 4; i++)
    {
        r[i] =
            in[0] * matrix[0 * 4 + i] +
            in[1] * matrix[1 * 4 + i] +
            in[2] * matrix[2 * 4 + i] +
            1.0 * matrix[3 * 4 + i];
    }
    out[0] = r[0] / r[3];
    out[1] = r[1] / r[3];
    out[2] = r[2] / r[3];
}

void GLEmulator::multMatrixVec34(const double matrix[16], const double in[3], double out[4])
{
    int i;
    for (i = 0; i < 4; i++)
    {
        out[i] =
            in[0] * matrix[0 * 4 + i] +
            in[1] * matrix[1 * 4 + i] +
            in[2] * matrix[2 * 4 + i] +
            1.0 * matrix[3 * 4 + i];
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

const std::vector<std::array<float, 4> > *GLEmulator::colourList() const
{
    return &m_colourList;
}

void C3F(float *x)         { glEmulator.glColor3fv(x); }
