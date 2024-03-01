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
#if QT_VERSION >= 0x060000
#include <QColorSpace>
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

//    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL); // AA_UseOpenGLES not currently supported
#if QT_VERSION < 0x060000
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 3); // OpenGL 3.3
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
#if QT_VERSION < 0x060000
    fmt.setColorSpace(QSurfaceFormat::sRGBColorSpace);
#else
    fmt.setColorSpace(QColorSpace(QColorSpace::SRgb));
#endif
#if defined(GAITSYM_DEBUG_BUILD)
    fmt.setOption(QSurfaceFormat::DebugContext, true);
#endif
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication application(argc, argv);

    try
    {
        QOffscreenSurface surf;
        surf.create();
        QOpenGLContext ctx;
        ctx.create();
        ctx.makeCurrent(&surf);
        const char *p = reinterpret_cast<const char *>(ctx.functions()->glGetString(GL_VERSION));
        std::string glVersionString(p ? p : "");
        p  = reinterpret_cast<const char *>(ctx.functions()->glGetString(GL_EXTENSIONS));
        std::string glExtensionsString(p ? p : "");
        double glVersion = std::stod(glVersionString);
        qDebug () << glVersionString.c_str();
        qDebug () << glExtensionsString.c_str();
        if (glVersion <= 3.2)
        {
            QString errorMessage = QString("This application requires OpenGL 3.3 or greater.\nCurrent version is %1.\nApplication will abort.").arg(glVersionString.c_str());
            QMessageBox::critical(nullptr, "GaitSym5", errorMessage);
            exit(EXIT_FAILURE);
        }
    }
    catch (...)
    {
        QString errorMessage = QString("This application requires OpenGL 3.3 or greater.\nUnknown failure initialising OpenGL.\nApplication will abort.");
        QMessageBox::critical(nullptr, "GaitSym5", errorMessage);
        exit(EXIT_FAILURE);
    }

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
        if (styleCode == 1 && styles.contains("Windows", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("Windows"));
            break;
        }
        if (styleCode == 2 && styles.contains("windowsvista", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("windowsvista"));
            break;
        }
        if (styleCode == 2 && styles.contains("gtk", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("gtk"));

        }
        if (styleCode == 2 && styles.contains("macOS", Qt::CaseInsensitive))
        {
            application.setStyle(QStyleFactory::create("macOS"));
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
