#ifndef STROKEFONT_H
#define STROKEFONT_H

class SimulationWidget;

#include "PGDMath.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class StrokeFont
{
public:
    StrokeFont();
    virtual ~StrokeFont();

    enum MarkerCode { XShape };

    void strokeString(const char *string,    /* character string */
                      int length,            /* number of characters to draw */
                      float x,               /* x coordinate of bottom left of character */
                      float y,               /* y coordinate ... */
                      float cwidth,          /* cwidth of character cell */
                      float cheight,         /* cheight of character cell */
                      int xJustification,    /* 0 - left, 1 - centre, 2 - right */
                      int yJustification,    /* 0 - bottom, 1 - centre, 2 - top */
                      const float *matrix,
                      const float *translation);

    void strokeCharacter(int ichar,            /* character code */
                         float x,              /* x coordinate of bottom left of character */
                         float y,              /* y coordinate ... */
                         float cwidth,         /* cwidth of character cell */
                         float cheight,         /* cheight of character cell */
                         const float *matrix,
                         const float *translation);

    void strokeMarker(
                             MarkerCode code,       /* marker code */
                             float x,               /* x coordinate of centre of marker */
                             float y,               /* y coordinate ... */
                             float cwidth,          /* cwidth of character cell */
                             float cheight,         /* cheight of character cell */
                             const float *matrix,
                             const float *translation);

    void addLine(float ix1, float iy1, float iz1, float ix2, float iy2, float iz2);
    void addLine(float ix1, float iy1, float iz1, float ix2, float iy2, float iz2, const float *matrix, const float *translation);
    void startLine() { m_startLineFlag = true; }
    void addPoint(float ix1, float iy1, float iz1);
    void addPoint(float ix1, float iy1, float iz1, const float *matrix, const float *translation);

    void addCircle(float cx, float cy, float cz, float r, int num_segments);
    void addArc(float cx, float cy, float cz, float r, float start_angle, float arc_angle, int num_segments);

    void setDisplayPosition(double x, double y, double z);
    void setDisplayRotation(const pgd::Matrix3x3 &R);
    void setDisplayRotationFromQuaternion(const pgd::Quaternion &q);
    const pgd::Vector3 &displayPosition()  { return m_displayPosition; }
    const pgd::Matrix3x3 &displayRotation()  { return m_displayRotation; }
    void move(double x, double y, double z);
    void scale(double x, double y, double z);

    void setZ(float z) { m_z = z; }
    void setRGBA(float r, float g, float b, float a) { m_r = r; m_g = g; m_b = b; m_a = a; }

    void zeroLineBuffer() { m_numLines = 0; }
    size_t numLines() { return m_numLines; }
    float *lineBuffer() { return m_lineBuffer; }

    SimulationWidget *glWidget() const;
    void setGlWidget(SimulationWidget *glWidget);
    QMatrix4x4 vpMatrix() const;
    void setVpMatrix(const QMatrix4x4 &vpMatrix);
    void debug();
    virtual void draw();

private:
    float m_z = 0;
    float m_r = 1;
    float m_g = 1;
    float m_b = 1;
    float m_a = 1;

    float m_lastX = 0;
    float m_lastY = 0;
    float m_lastZ = 0;
    bool m_startLineFlag = true;

    size_t m_numLines = 0;
    size_t m_maxLines = 100000;
    float *m_lineBuffer = nullptr;

    pgd::Vector3 m_displayPosition = {0, 0, 0};
    pgd::Vector3 m_displayScale = {1, 1, 1};
    pgd::Matrix3x3 m_displayRotation;

    SimulationWidget *m_glWidget = nullptr;
    QOpenGLBuffer m_VBO;
    bool m_BufferObjectsAllocated = false;
    QMatrix4x4 m_vpMatrix;
};

#endif // STROKEFONT_H
