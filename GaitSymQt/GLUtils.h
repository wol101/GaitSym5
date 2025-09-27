/*
 *  GLUtils.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 26/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef GLUtils_h
#define GLUtils_h

class StrokeFont;

class GLUtils
{
public:

    static void drawAxes(float x, float y, float z, float ox = 0, float oy = 0, float oz = 0);
    static void outputText(float x, float y, float z, char *text, float size, const float *matrix, const float *translation);
    static void drawLine(float ix1, float iy1, float iz1, float ix2, float iy2, float iz2);
    static void setDrawColour(float r, float g, float b, float a);

    static StrokeFont *strokeFont();
    static void setStrokeFont(StrokeFont *strokeFont);

private:
    static StrokeFont *m_strokeFont;
};


#endif
