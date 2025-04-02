/*
 *  main.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 08/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include "MainWindow.h"
#include "Preferences.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QDebug>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QMessageBox>
#include <QTimer>
#include <QColorSpace>

// this bit of code declares some symbols that hint that the NVIDIA/AMD GPU is preferred over the integrated one
#if defined(Q_OS_WIN)
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#ifdef __cplusplus
}
#endif
#endif

#if defined(GAITSYM_DEBUG_BUILD) && defined(Q_OS_WIN)
#include <crtdbg.h>
#endif

int main(int argc, char *argv[])
{
#if defined(GAITSYM_DEBUG_BUILD) && defined(Q_OS_WIN)
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
#endif

    // read in the Preferences file
    Preferences::Read();

    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    // fmt.setVersion(3, 3); // OpenGL 3.3 [if not specified then it seems to use the highest available]
    fmt.setProfile(QSurfaceFormat::CoreProfile); // only use the core functions
    fmt.setDepthBufferSize(24);
#ifndef Q_OS_MACOS // these do nothing on MacOS anyway and cause OpenGL to fail on M1
    fmt.setRedBufferSize(8);
    fmt.setGreenBufferSize(8);
    fmt.setBlueBufferSize(8);
#endif
    fmt.setAlphaBufferSize(8);
    fmt.setStencilBufferSize(8);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    fmt.setSwapInterval(1);
    fmt.setSamples(Preferences::valueInt("OpenGLMultisample", 8));
    fmt.setColorSpace(QColorSpace(QColorSpace::SRgb));
#if defined(GAITSYM_DEBUG_BUILD)
    fmt.setOption(QSurfaceFormat::DebugContext, true);
#endif
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication application(argc, argv);

    int styleCode = Preferences::valueInt("StyleCode");
    QStringList styles = QStyleFactory::keys();
    qDebug() << styles;
    while (true)
    {
        if (styleCode == 0 && styles.contains("Fusion", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("Fusion"));
            break;
        }
        if (styleCode == 1 && styles.contains("windows11", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("macOS"));
            break;
        }
        if (styleCode == 2 && styles.contains("windowsvista", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("windowsvista"));
            break;
        }
        if (styleCode == 3 && styles.contains("Windows", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("Windows"));
            break;
        }
        if (styleCode == 4 && styles.contains("gtk", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("gtk"));
            break;
        }
        if (styleCode == 5 && styles.contains("macOS", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("macOS"));
            break;
        }
        break;
    }

    MainWindow window;
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() >= 2)
    {
        QTimer::singleShot(0, &window, SLOT(handleCommandLineArguments()));
    }
    window.show();
    return application.exec();
}
