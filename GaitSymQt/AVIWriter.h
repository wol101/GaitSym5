/*
 *  AVIWriter.h
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 08/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#ifndef AVIWRITER_H
#define AVIWRITER_H

#include <QString>

class QImage;

class AVIWriter
{
public:
    AVIWriter();
    virtual ~AVIWriter();
    int initialiseFile(const QString &aviFilename, unsigned int width, unsigned int height, unsigned int fps);
    int writeAVI(unsigned int width, unsigned int height, uint8_t *rgb, int quality);
    int writeAVI(const QImage &image, int quality);

private:
    int CloseFile();

    struct gwavi_t *m_gwavi;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_fps;
    QString m_aviFilename;
};

#endif // AVIWRITER_H
