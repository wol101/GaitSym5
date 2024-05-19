/*
 *  MainWindow.h
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 08/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

class SimulationWidget;

namespace Ui { class MainWindow; }

namespace GaitSym
{
class Body;
class Driver;
class Geom;
class Joint;
class Marker;
class Muscle;
class NamedObject;
class Simulation;
}

class QAction;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() Q_DECL_OVERRIDE;

    enum Mode { constructionMode, runMode };

    Mode mode() const;
    GaitSym::Simulation *simulation() const;
    SimulationWidget *simulationWidget() const;

    const QFileInfo &configFile() const;

public slots:
    void processOneThing();
    void handleCommandLineArguments();

    void buttonCameraBack();
    void buttonCameraBottom();
    void buttonCameraFront();
    void buttonCameraLeft();
    void buttonCameraRight();
    void buttonCameraTop();
    void enterConstructionMode();
    void enterRunMode();
    void menu1280x720();
    void menu1920x1080();
    void menu640x480();
    void menu800x600();
    void menuAbout();
    void menuClearMeshCache();
    void menuCreateAssembly();
    void menuCreateBody();
    void menuCreateDriver();
    void menuCreateGeom();
    void menuCreateJoint();
    void menuCreateMarker();
    void menuCreateMuscle();
    void menuCreateMirrorElements();
    void menuCreateTestingDrivers();
    void menuLoadDefaultView();
    void menuDeleteAssembly();
    void menuEditGlobal();
    void menuExportMarkers();
    void menuImportMeshes();
    void menuNew();
    void menuOpen();
    void menuOutputs();
    void menuPreferences();
    void menuRawXMLEditor();
    void menuRecordMovie();
    void menuRename();
    void menuResetView();
    void menuRestart();
    void menuSave();
    void menuSaveAs();
    void menuSaveDefaultView();
    void menuStartOBJSequenceSave();
    void menuStartUSDSequenceSave();
    void menuStopAVISave();
    void menuStopOBJSequenceSave();
    void menuStopUSDSequenceSave();
    void menuToggleFullScreen();

    void menuOpen(const QString &fileName, const QByteArray *fileData);
    void menuCreateEditMarker(GaitSym::Marker *marker);
    void menuCreateEditBody(GaitSym::Body *body);
    void menuCreateEditJoint(GaitSym::Joint *joint);
    void menuCreateEditGeom(GaitSym::Geom *geom);
    void menuCreateEditMuscle(GaitSym::Muscle *muscle);
    void menuCreateEditDriver(GaitSym::Driver *driver);

    void menuOpenRecent(QAction *action);

    void elementInfo(const QString &elementType, const QString &elementName);
    void elementHide(const QString &elementType, const QString &elementName);

    void usdSnapshot();
    void objSnapshot();
    void run();
    void snapshot();
    void step();

    void copy();
    void cut();
    void paste();
    void selectAll();

    void comboBoxMeshDisplayMapCurrentTextChanged(const QString &text);
    void comboBoxMuscleColourMapCurrentTextChanged(const QString &text);
    void comboBoxProjectionCurrentTextChanged(const QString &text);
    void comboBoxTrackingMarkerCurrentTextChanged(const QString &text);
    void deleteExistingBody(const QString &, bool force = false);
    void deleteExistingDriver(const QString &name, bool force = false);
    void deleteExistingGeom(const QString &, bool force = false);
    void deleteExistingJoint(const QString &, bool force = false);
    void deleteExistingMarker(const QString &, bool force = false);
    void deleteExistingMuscle(const QString &, bool force = false);
    void editExistingBody(const QString &);
    void editExistingDriver(const QString &);
    void editExistingGeom(const QString &);
    void editExistingJoint(const QString &);
    void editExistingMarker(const QString &);
    void editExistingMuscle(const QString &);
    void handleElementTreeWidgetItemChanged(QTreeWidgetItem *item, int column);

    void moveExistingMarker(const QString &s, const QVector3D &p);
    void radioButtonTracking();
    void reportOpenGLSize(int width, int height);
    void resizeAndCentre(int w, int h);
    void spinboxCOIXChanged(double v);
    void spinboxCOIYChanged(double v);
    void spinboxCOIZChanged(double v);
    void spinboxCursorNudgeChanged(double v);
    void spinboxCursorSizeChanged(double v);
    void spinboxDistanceChanged(double v);
    void spinboxFPSChanged(double v);
    void spinboxFarChanged(double v);
    void spinboxFoVChanged(double v);
    void spinboxNearChanged(double v);
    void spinboxSkip(int i);
    void spinboxTimeMax(double v);
    void spinboxTrackingOffsetChanged(double v);

    void setStatusString(const QString &s, int logLevel);
    void setUICOI(float x, float y, float z);
    void setUIFoV(float v);

    void log(const QString &text);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:

    void setInterfaceValues();
    void writeSettings();
    static QByteArray readResource(const QString &resource);
    void updateEnable();
    void resizeSimulationWindow(int openGLWidth, int openGLHeight);
    void updateComboBoxTrackingMarker();
    void handleTracking();
    void updateRecentFiles(const QString &recentFile);

    Ui::MainWindow *ui = nullptr;
    SimulationWidget *m_simulationWidget = nullptr;

    QFileInfo m_configFile; // maybe use windowFilePath() and windowTitle() instead

    bool m_movieFlag = false;
    bool m_saveOBJFileSequenceFlag = false;
    QString m_objFileSequenceFolder;
    enum objFileFormat { obj, usda };
    enum objFileFormat m_objFileFormat = obj;
    bool m_stepFlag = false;
    uint64_t m_stepCount = 0;
    int m_logLevel = 1;

    QTimer *m_timer = nullptr;
    GaitSym::Simulation *m_simulation = nullptr;

    Mode m_mode = constructionMode;
    bool m_noName = true;

    QStringList m_recentFileList;
    int m_maxRecentFiles = 20;
};

#endif // MAINWINDOW_H
