/*
 *  MainWindow.cpp
 *  GaitSymODE2019
 *
 *  Created by Bill Sellers on 08/10/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DialogAbout.h"
#include "Body.h"
#include "Colour.h"
#include "DataFile.h"
#include "DialogAssembly.h"
#include "DialogBodyBuilder.h"
#include "DialogConvertFile.h"
#include "DialogCreateMirrorElements.h"
#include "DialogCreateTestingDrivers.h"
#include "DialogDrivers.h"
#include "DialogGeoms.h"
#include "DialogGlobal.h"
#include "DialogInfo.h"
#include "DialogJoints.h"
#include "DialogMarkerExport.h"
#include "DialogMarkerImport.h"
#include "DialogMarkers.h"
#include "DialogMuscles.h"
#include "DialogOutputSelect.h"
#include "DialogPreferences.h"
#include "DialogRename.h"
#include "DialogStringOfPearlsBuilder.h"
#include "Driver.h"
#include "FacetedObject.h"
#include "FluidSac.h"
#include "Geom.h"
#include "Joint.h"
#include "Marker.h"
#include "MarkerEllipseDriver.h"
#include "MarkerPositionDriver.h"
#include "Muscle.h"
#include "Preferences.h"
#include "Simulation.h"
#include "SimulationWidget.h"
#include "TegotaeDriver.h"
#include "DialogRawXMLEdit.h"
#include "ThreeHingeJointDriver.h"
#include "TwoHingeJointDriver.h"
#include "OpenSimExporter.h"
#include "DrawMuscle.h"
#include "MuJoCoPhysicsEngine.h"

#include "pystring.h"
#include "pocketpy.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QScreen>
#include <QSizePolicy>
#include <QStringList>
#include <QTemporaryFile>
#include <QThread>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWindow>
#include <QtGlobal>

#include <sstream>

using namespace std::literals::string_literals;

// simple guard class for std::cerr stream capture
class cerrRedirect
{
public:
    cerrRedirect(std::streambuf *newBuffer)
    {
        oldBuffer = std::cerr.rdbuf(newBuffer);
    }
    ~cerrRedirect()
    {
        std::cerr.rdbuf(oldBuffer);
    }
private:
    std::streambuf *oldBuffer;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Initialize pocketpy and the default VM.
    py_initialize();

    // create the window elements
    ui->setupUi(this);

    // fill the recent file lists
    m_recentFileList = Preferences::valueQStringList("RecentFileList");
    m_maxRecentFiles = Preferences::valueInt("MaxRecentFiles", 20);
    updateRecentFiles("");

    // interface related connections
    connect(ui->action1280x720, SIGNAL(triggered()), this, SLOT(menu1280x720()));
    connect(ui->action1920x1080, SIGNAL(triggered()), this, SLOT(menu1920x1080()));
    connect(ui->action640x480, SIGNAL(triggered()), this, SLOT(menu640x480()));
    connect(ui->action800x600, SIGNAL(triggered()), this, SLOT(menu800x600()));
    connect(ui->actionAboutGaitSymQt, SIGNAL(triggered()), this, SLOT(menuAbout()));
    connect(ui->actionClearMeshCache, SIGNAL(triggered()), this, SLOT(menuClearMeshCache()));
    connect(ui->actionClearKinematics, SIGNAL(triggered()), this, SLOT(menuClearKinematics()));
    connect(ui->actionConstructionMode, SIGNAL(triggered()), this, SLOT(enterConstructionMode()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->actionCreateAssembly, SIGNAL(triggered()), this, SLOT(menuCreateAssembly()));
    connect(ui->actionCreateBody, SIGNAL(triggered()), this, SLOT(menuCreateBody()));
    connect(ui->actionCreateDriver, SIGNAL(triggered()), this, SLOT(menuCreateDriver()));
    connect(ui->actionCreateGeom, SIGNAL(triggered()), this, SLOT(menuCreateGeom()));
    connect(ui->actionCreateJoint, SIGNAL(triggered()), this, SLOT(menuCreateJoint()));
    connect(ui->actionCreateMarker, SIGNAL(triggered()), this, SLOT(menuCreateMarker()));
    connect(ui->actionCreateMuscle, SIGNAL(triggered()), this, SLOT(menuCreateMuscle()));
    connect(ui->actionCreateMirrorElements, SIGNAL(triggered()), this, SLOT(menuCreateMirrorElements()));
    connect(ui->actionCreateStringOfPearls, SIGNAL(triggered()), this, SLOT(menuCreateStringOfPearls()));
    connect(ui->actionCreateTestingDrivers, SIGNAL(triggered()), this, SLOT(menuCreateTestingDrivers()));
    connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(ui->actionLoadDefaultView, SIGNAL(triggered()), this, SLOT(menuLoadDefaultView()));
    connect(ui->actionDeleteAssembly, SIGNAL(triggered()), this, SLOT(menuDeleteAssembly()));
    connect(ui->actionEditGlobal, SIGNAL(triggered()), this, SLOT(menuEditGlobal()));
    connect(ui->actionExportMarkers, SIGNAL(triggered()), this, SLOT(menuExportMarkers()));
    connect(ui->actionExportOpenSim, SIGNAL(triggered()), this, SLOT(menuExportOpenSim()));
    connect(ui->actionExportMuJoCo, SIGNAL(triggered()), this, SLOT(menuExportMuJoCo()));
    connect(ui->actionImportMarkers, SIGNAL(triggered()), this, SLOT(menuImportMarkers()));
    connect(ui->actionImportMeshesAsBodies, SIGNAL(triggered()), this, SLOT(menuImportMeshes()));
    connect(ui->actionConvertFile, SIGNAL(triggered()), this, SLOT(menuConvertFile()));
    connect(ui->actionPlaybackOpenSimBodyKinematics, SIGNAL(triggered()), this, SLOT(menuImportOpenSimBodyKinematics()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(menuNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(menuOpen()));
    connect(ui->actionOutput, SIGNAL(triggered()), this, SLOT(menuOutputs()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(menuPreferences()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionRawXMLEditor, SIGNAL(triggered()), this, SLOT(menuRawXMLEditor()));
    connect(ui->actionRecordMovie, SIGNAL(triggered()), this, SLOT(menuRecordMovie()));
    connect(ui->actionRenameElement, SIGNAL(triggered()), this, SLOT(menuRename()));
    connect(ui->actionResetView, SIGNAL(triggered()), this, SLOT(menuResetView()));
    connect(ui->actionRestart, SIGNAL(triggered()), this, SLOT(menuRestart()));
    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(run()));
    connect(ui->actionRunMode, SIGNAL(triggered()), this, SLOT(enterRunMode()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(menuSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(menuSaveAs()));
    connect(ui->actionSaveDefaultView, SIGNAL(triggered()), this, SLOT(menuSaveDefaultView()));
    connect(ui->actionSaveOBJSnapshot, SIGNAL(triggered()), this, SLOT(objSnapshot()));
    connect(ui->actionSaveUSDSnapshot, SIGNAL(triggered()), this, SLOT(usdSnapshot()));
    connect(ui->actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));
    connect(ui->actionSnapshot, SIGNAL(triggered()), this, SLOT(snapshot()));
    connect(ui->actionStartOBJSequence, SIGNAL(triggered()), this, SLOT(menuStartOBJSequenceSave()));
    connect(ui->actionStartUSDSequence, SIGNAL(triggered()), this, SLOT(menuStartUSDSequenceSave()));
    connect(ui->actionStep, SIGNAL(triggered()), this, SLOT(step()));
    connect(ui->actionStopOBJSequence, SIGNAL(triggered()), this, SLOT(menuStopOBJSequenceSave()));
    connect(ui->actionStopUSDSequence, SIGNAL(triggered()), this, SLOT(menuStopUSDSequenceSave()));
    connect(ui->actionToggleFullscreen, SIGNAL(triggered()), this, SLOT(menuToggleFullScreen()));
    connect(ui->actionViewBack, SIGNAL(triggered()), this, SLOT(buttonCameraBack()));
    connect(ui->actionViewBottom, SIGNAL(triggered()), this, SLOT(buttonCameraBottom()));
    connect(ui->actionViewFront, SIGNAL(triggered()), this, SLOT(buttonCameraFront()));
    connect(ui->actionViewLeft, SIGNAL(triggered()), this, SLOT(buttonCameraLeft()));
    connect(ui->actionViewRight, SIGNAL(triggered()), this, SLOT(buttonCameraRight()));
    connect(ui->actionViewTop, SIGNAL(triggered()), this, SLOT(buttonCameraTop()));
    connect(ui->radioButtonTrackingNone, SIGNAL(clicked()), this, SLOT(radioButtonTracking()));
    connect(ui->radioButtonTrackingX, SIGNAL(clicked()), this, SLOT(radioButtonTracking()));
    connect(ui->radioButtonTrackingY, SIGNAL(clicked()), this, SLOT(radioButtonTracking()));
    connect(ui->radioButtonTrackingZ, SIGNAL(clicked()), this, SLOT(radioButtonTracking()));
    connect(ui->comboBoxMeshDisplay, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxMeshDisplayMapCurrentTextChanged(const QString &)));
    connect(ui->comboBoxMuscleColourMap, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxMuscleColourMapCurrentTextChanged(const QString &)));
    connect(ui->comboBoxTrackingMarker, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxTrackingMarkerCurrentTextChanged(const QString &)));
    connect(ui->comboBoxProjection, SIGNAL(currentTextChanged(const QString &)), this, SLOT(comboBoxProjectionCurrentTextChanged(const QString &)));
    connect(ui->doubleSpinBoxCOIX, SIGNAL(valueChanged(double)), this, SLOT(spinboxCOIXChanged(double)));
    connect(ui->doubleSpinBoxCOIY, SIGNAL(valueChanged(double)), this, SLOT(spinboxCOIYChanged(double)));
    connect(ui->doubleSpinBoxCOIZ, SIGNAL(valueChanged(double)), this, SLOT(spinboxCOIZChanged(double)));
    connect(ui->doubleSpinBoxCursorNudge, SIGNAL(valueChanged(double)), this, SLOT(spinboxCursorNudgeChanged(double)));
    connect(ui->doubleSpinBoxCursorSize, SIGNAL(valueChanged(double)), this, SLOT(spinboxCursorSizeChanged(double)));
    connect(ui->doubleSpinBoxDistance, SIGNAL(valueChanged(double)), this, SLOT(spinboxDistanceChanged(double)));
    connect(ui->doubleSpinBoxFPS, SIGNAL(valueChanged(double)), this, SLOT(spinboxFPSChanged(double)));
    connect(ui->doubleSpinBoxFar, SIGNAL(valueChanged(double)), this, SLOT(spinboxFarChanged(double)));
    connect(ui->doubleSpinBoxFoV, SIGNAL(valueChanged(double)), this, SLOT(spinboxFoVChanged(double)));
    connect(ui->doubleSpinBoxNear, SIGNAL(valueChanged(double)), this, SLOT(spinboxNearChanged(double)));
    connect(ui->doubleSpinBoxTimeMax, SIGNAL(valueChanged(double)), this, SLOT(spinboxTimeMax(double)));
    connect(ui->doubleSpinBoxTrackingOffset, SIGNAL(valueChanged(double)), this, SLOT(spinboxTrackingOffsetChanged(double)));
    connect(ui->spinBoxSkip, SIGNAL(valueChanged(int)), this, SLOT(spinboxSkip(int)));
    connect(ui->treeWidgetElements, SIGNAL(createNewBody()), this, SLOT(menuCreateBody()));
    connect(ui->treeWidgetElements, SIGNAL(createNewDriver()), this, SLOT(menuCreateDriver()));
    connect(ui->treeWidgetElements, SIGNAL(createNewGeom()), this, SLOT(menuCreateGeom()));
    connect(ui->treeWidgetElements, SIGNAL(createNewJoint()), this, SLOT(menuCreateJoint()));
    connect(ui->treeWidgetElements, SIGNAL(createNewMarker()), this, SLOT(menuCreateMarker()));
    connect(ui->treeWidgetElements, SIGNAL(createNewMuscle()), this, SLOT(menuCreateMuscle()));
    connect(ui->treeWidgetElements, SIGNAL(deleteBody(const QString &)), this, SLOT(deleteExistingBody(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(deleteDriver(const QString &)), this, SLOT(deleteExistingDriver(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(deleteGeom(const QString &)), this, SLOT(deleteExistingGeom(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(deleteJoint(const QString &)), this, SLOT(deleteExistingJoint(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(deleteMarker(const QString &)), this, SLOT(deleteExistingMarker(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(deleteMuscle(const QString &)), this, SLOT(deleteExistingMuscle(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editBody(const QString &)), this, SLOT(editExistingBody(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editDriver(const QString &)), this, SLOT(editExistingDriver(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editGeom(const QString &)), this, SLOT(editExistingGeom(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editJoint(const QString &)), this, SLOT(editExistingJoint(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editMarker(const QString &)), this, SLOT(editExistingMarker(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(editMuscle(const QString &)), this, SLOT(editExistingMuscle(const QString &)));
    connect(ui->treeWidgetElements, SIGNAL(elementTreeWidgetItemChanged(QTreeWidgetItem *, int)), this, SLOT(handleElementTreeWidgetItemChanged(QTreeWidgetItem *, int)));
    connect(ui->treeWidgetElements, SIGNAL(infoRequest(const QString &, const QString &)), this, SLOT(elementInfo(const QString &, const QString &)));

    connect(ui->actionDisplayShadows, &QAction::triggered, this, &MainWindow::buttonDisplayShadows);
    connect(ui->actionDisplayAsWireframe, &QAction::triggered, this, &MainWindow::buttonDisplayAsWireframe);

    // put SimulationWindow into existing widgetGLWidget
    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::LeftToRight, ui->widgetSimulationPlaceholder);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->setSpacing(0);
    m_simulationWidget = new SimulationWidget();
    QWidget *container = QWidget::createWindowContainer(m_simulationWidget);
    boxLayout->addWidget(container);
    m_simulationWidget->setMainWindow(this);

    layoutSpacing(this);

    // connect the ViewControlWidget to the GLWidget
    connect(ui->widgetViewFrame, SIGNAL(EmitCameraVec(double, double, double)), m_simulationWidget, SLOT(SetCameraVec(double, double, double)));

    // connect the SimulationWindow to the MainWindow
    connect(m_simulationWidget, SIGNAL(EmitStatusString(const QString &, int)), this, SLOT(setStatusString(const QString &, int)));
    connect(m_simulationWidget, SIGNAL(EmitCOI(float, float, float)), this, SLOT(setUICOI(float, float, float)));
    connect(m_simulationWidget, SIGNAL(EmitFoV(float)), this, SLOT(setUIFoV(float)));
    connect(m_simulationWidget, SIGNAL(EmitCreateMarkerRequest()), this, SLOT(menuCreateMarker()));
    connect(m_simulationWidget, SIGNAL(EmitEditMarkerRequest(const QString &)), this, SLOT(editExistingMarker(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitMoveMarkerRequest(const QString &, const QVector3D &)), this, SLOT(moveExistingMarker(const QString &, const QVector3D &)));
    connect(m_simulationWidget, SIGNAL(EmitEditBodyRequest(const QString &)), this, SLOT(editExistingBody(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitEditGeomRequest(const QString &)), this, SLOT(editExistingGeom(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitEditJointRequest(const QString &)), this, SLOT(editExistingJoint(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitEditMuscleRequest(const QString &)), this, SLOT(editExistingMuscle(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitDeleteBodyRequest(const QString &)), this, SLOT(deleteExistingBody(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitDeleteGeomRequest(const QString &)), this, SLOT(deleteExistingGeom(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitDeleteJointRequest(const QString &)), this, SLOT(deleteExistingJoint(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitDeleteMarkerRequest(const QString &)), this, SLOT(deleteExistingMarker(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitDeleteMuscleRequest(const QString &)), this, SLOT(deleteExistingMuscle(const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitInfoRequest(const QString &, const QString &)), this, SLOT(elementInfo(const QString &, const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitHideRequest(const QString &, const QString &)), this, SLOT(elementHide(const QString &, const QString &)));
    connect(m_simulationWidget, SIGNAL(EmitResize(int, int)), this, SLOT(reportOpenGLSize(int, int)));

    // the treeWidgetElements needs to know about this window
    ui->treeWidgetElements->setMainWindow(this);

    // add the muscle drawing options to the toolbar
    for (size_t i = 0; i < DrawMuscle::muscleDrawStyleCount; i++) { ui->comboBoxMuscleDraw->addItem(DrawMuscle::muscleDrawStyleStrings(i)); }
    connect(ui->comboBoxMuscleDraw, &QComboBox::currentIndexChanged, this, &MainWindow::comboBoxMuscleDraw);

    // set up the timer
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(processOneThing()));

    // zero the timer display
    QString time = QString("%1").arg(double(0), 0, 'f', 5);
    ui->lcdNumberTime->display(time);

    // intialise parts of the interface
    setInterfaceValues();
    setStatusString(tr("Ready"), 2);
    updateEnable();

    // finally remember the geometry etc.
    restoreGeometry(Preferences::valueQByteArray("MainWindowGeometry"));
    restoreState(Preferences::valueQByteArray("MainWindowState"));
    ui->splitter1->restoreState(Preferences::valueQByteArray("MainWindowSplitter1State"));
    ui->splitter2->restoreState(Preferences::valueQByteArray("MainWindowSplitter2State"));
    Preferences::insert("ElementTreeHeaderState", ui->treeWidgetElements->header()->saveState());
    log(QString("Window state restored from \"%1\"").arg(Preferences::fileName()));

    int toolBarIconSize = Preferences::valueInt("ToolBarIconSize", 32);
    ui->toolBar->setIconSize(QSize(toolBarIconSize, toolBarIconSize));

    // Full screen does not work sensibly yet so remove
    if (isFullScreen()) this->menuToggleFullScreen();
    ui->actionToggleFullscreen->setVisible(false);
}

MainWindow::~MainWindow()
{
    // Finalize pocketpy and free all VMs.
    PK_API void py_finalize();

    m_timer->stop();

    if (m_simulation) delete m_simulation;
    delete ui;

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isWindowModified())
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Click OK to quit and lose any modifications, or Cancel to return to the current document");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (m_movieFlag) { m_simulationWidget->StopAVISave(); }
            writeSettings();
            QMainWindow::closeEvent(event);
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            // should never be reached
            event->ignore();
            break;
        }
    }
    else
    {
        if (m_movieFlag) { m_simulationWidget->StopAVISave(); }
        writeSettings();
        QMainWindow::closeEvent(event);
    }
}



void MainWindow::processOneThing()
{
    if (m_simulation)
    {
        std::stringstream capturedCerr;
        cerrRedirect redirect(capturedCerr.rdbuf());
        if (m_simulation->ShouldQuit() || m_simulation->TestForCatastrophy())
        {
            log(QString::fromStdString(capturedCerr.str()));
            setStatusString(tr("Unable to start simulation"), 1);
            ui->actionRun->setChecked(false);
            this->run();
            return;
        }

        m_simulation->UpdateSimulation();
        m_stepCount++;

        if ((m_stepCount % size_t(Preferences::valueInt("MovieSkip"))) == 0)
        {
            handleTracking();
            if (m_stepFlag)
            {
                m_stepFlag = false;
                m_timer->stop();
            }
            m_simulationWidget->getDrawMuscleMap()->clear(); // force a redraw of all muscles
            m_simulationWidget->getDrawFluidSacMap()->clear(); // force a redraw of all fluid sacs
            m_simulationWidget->update();
            if (m_movieFlag)
            {
                m_simulationWidget->WriteMovieFrame();
            }
            if (m_saveOBJFileSequenceFlag)
            {
                QString filename = QString("%1%2").arg("Frame").arg(m_simulation->GetTime(), 12, 'f', 7, QChar('0'));
                QString path = QDir(m_objFileSequenceFolder).filePath(filename);
                if (m_objFileFormat == usda) { path.append(".usda"); }
                log(QString("Writing \"%1\"\n").arg(path));
                switch (m_objFileFormat)
                {
                case obj:
                    m_simulationWidget->WriteCADFrame(path);
                    break;
                case usda:
                    m_simulationWidget->WriteUSDFrame(path);
                    break;
                }
            }
            QString time = QString("%1").arg(m_simulation->GetTime(), 0, 'f', 5);
            ui->lcdNumberTime->display(time);
//            qDebug() << m_simulation->CalculateInstantaneousFitness() << "\n";
        }

        if (m_simulation->ShouldQuit())
        {
            log(QString::fromStdString(capturedCerr.str()));
            setStatusString(tr("Simulation ended normally"), 1);
            log(QString("Fitness = %1\n").arg(m_simulation->CalculateInstantaneousFitness(), 0, 'f', 5));
            log(QString("Time = %1\n").arg(m_simulation->GetTime(), 0, 'f', 5));
            log(QString("Metabolic Energy = %1\n").arg(m_simulation->GetMetabolicEnergy(), 0, 'f', 5));
            log(QString("Mechanical Energy = %1\n").arg(m_simulation->GetMechanicalEnergy(), 0, 'f', 5));
            m_simulationWidget->update();
            QString time = QString("%1").arg(m_simulation->GetTime(), 0, 'f', 5);
            ui->lcdNumberTime->display(time);
            ui->actionRun->setChecked(false);
            this->run();
            return;
        }
        if (m_simulation->TestForCatastrophy())
        {
            log(QString::fromStdString(capturedCerr.str()));
            setStatusString(tr("Simulation aborted"), 1);
            ui->textEditLog->append(QString("Fitness = %1\n").arg(m_simulation->CalculateInstantaneousFitness(), 0, 'f', 5));
            m_simulationWidget->update();
            QString time = QString("%1").arg(m_simulation->GetTime(), 0, 'f', 5);
            ui->lcdNumberTime->display(time);
            ui->actionRun->setChecked(false);
            this->run();
            return;
        }
        log(QString::fromStdString(capturedCerr.str()));
    }
    updateEnable();
}

void MainWindow::handleCommandLineArguments()
{
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() >= 2)
    {
        QFileInfo fileInfo(arguments.at(1));
        if (fileInfo.isFile())
            this->menuOpen(arguments.at(1), nullptr);
    }
}

void MainWindow::handleTracking()
{
    if (!m_simulation) return;
    GaitSym::Marker *marker = m_simulation->GetMarker(ui->comboBoxTrackingMarker->currentText().toStdString());
    if (marker)
    {
        pgd::Vector3 position = marker->GetWorldPosition();
        if (ui->radioButtonTrackingX->isChecked())
        {
            m_simulationWidget->setCOIx(float(position.x + ui->doubleSpinBoxTrackingOffset->value()));
            ui->doubleSpinBoxCOIX->setValue(position.x + ui->doubleSpinBoxTrackingOffset->value());
        }
        if (ui->radioButtonTrackingY->isChecked())
        {
            m_simulationWidget->setCOIy(float(position.y + ui->doubleSpinBoxTrackingOffset->value()));
            ui->doubleSpinBoxCOIY->setValue(position.y + ui->doubleSpinBoxTrackingOffset->value());
        }
        if (ui->radioButtonTrackingZ->isChecked())
        {
            m_simulationWidget->setCOIz(float(position.z + ui->doubleSpinBoxTrackingOffset->value()));
            ui->doubleSpinBoxCOIZ->setValue(position.z + ui->doubleSpinBoxTrackingOffset->value());
        }
        m_simulationWidget->update();
    }
}

const QFileInfo &MainWindow::configFile() const
{
    return m_configFile;
}


void MainWindow::spinboxDistanceChanged(double v)
{
    Preferences::insert("CameraDistance", v);
    m_simulationWidget->setCameraDistance(float(v));
    m_simulationWidget->update();
}


void MainWindow::spinboxFoVChanged(double v)
{
    Preferences::insert("CameraFoV", v);
    m_simulationWidget->setFOV(float(v));
    m_simulationWidget->update();
}


void MainWindow::spinboxCOIXChanged(double v)
{
    Preferences::insert("CameraCOIX", v);
    m_simulationWidget->setCOIx(float(v));
    m_simulationWidget->update();
}


void MainWindow::spinboxCOIYChanged(double v)
{
    Preferences::insert("CameraCOIY", v);
    m_simulationWidget->setCOIy(float(v));
    m_simulationWidget->update();
}


void MainWindow::spinboxCOIZChanged(double v)
{
    Preferences::insert("CameraCOIZ", v);
    m_simulationWidget->setCOIz(float(v));
    m_simulationWidget->update();
}

void MainWindow::spinboxNearChanged(double v)
{
    Preferences::insert("CameraFrontClip", v);
    m_simulationWidget->setFrontClip(float(v));
    m_simulationWidget->update();
}

void MainWindow::spinboxFarChanged(double v)
{
    Preferences::insert("CameraBackClip", v);
    m_simulationWidget->setBackClip(float(v));
    m_simulationWidget->update();
}

void MainWindow::spinboxTrackingOffsetChanged(double v)
{
    Preferences::insert("TrackingOffset", v);
    handleTracking();
}

void MainWindow::radioButtonTracking()
{
    Preferences::insert("TrackingFlagNone", ui->radioButtonTrackingNone->isChecked());
    Preferences::insert("TrackingFlagX", ui->radioButtonTrackingX->isChecked());
    Preferences::insert("TrackingFlagY", ui->radioButtonTrackingY->isChecked());
    Preferences::insert("TrackingFlagZ", ui->radioButtonTrackingZ->isChecked());
    handleTracking();
}

void MainWindow::spinboxCursorSizeChanged(double v)
{
    Preferences::insert("CursorRadius", v);
    m_simulationWidget->setCursorRadius(float(v));
    m_simulationWidget->update();
}

void MainWindow::spinboxCursorNudgeChanged(double v)
{
    Preferences::insert("CursorNudge", v);
    m_simulationWidget->setCursor3DNudge(float(v));
}

void MainWindow::comboBoxMuscleColourMapCurrentTextChanged(const QString &text)
{
    GaitSym::Muscle::StrapColourControl colourControl = GaitSym::Muscle::fixedColour;
    if (text == "Strap Colour") colourControl = GaitSym::Muscle::fixedColour;
    else if (text == "Activation Colour") colourControl = GaitSym::Muscle::activationMap;
    else if (text == "Strain Colour") colourControl = GaitSym::Muscle::strainMap;
    else if (text == "Force Colour") colourControl = GaitSym::Muscle::forceMap;
    Preferences::insert("StrapColourControl", static_cast<int>(colourControl));
    if (m_simulation)
    {
        for (auto &&iter : *m_simulation->GetMuscleList()) iter.second->setStrapColourControl(colourControl);
    }
    m_simulationWidget->update();
}

void MainWindow::comboBoxMuscleDraw(int index)
{
    Preferences::insert("MuscleDrawStyle", index);
    if (m_simulation)
    {
        for (auto &&iter : *m_simulation->GetMuscleList()) iter.second->setRedraw(true);
    }
    m_simulationWidget->update();}

void MainWindow::spinboxSkip(int v)
{
    Preferences::insert("MovieSkip", v);
}

void MainWindow::spinboxTimeMax(double v)
{
    m_simulation->SetTimeLimit(v);
}

void MainWindow::spinboxFPSChanged(double v)
{
    Preferences::insert("MovieFramerate", v);
}

void MainWindow::setInterfaceValues()
{
    m_simulationWidget->setCameraDistance(float(Preferences::valueDouble("CameraDistance")));
    m_simulationWidget->setFOV(float(Preferences::valueDouble("CameraFoV")));
    m_simulationWidget->setCameraVecX(float(Preferences::valueDouble("CameraVecX")));
    m_simulationWidget->setCameraVecY(float(Preferences::valueDouble("CameraVecY")));
    m_simulationWidget->setCameraVecZ(float(Preferences::valueDouble("CameraVecZ")));
    m_simulationWidget->setCOIx(float(Preferences::valueDouble("CameraCOIX")));
    m_simulationWidget->setCOIy(float(Preferences::valueDouble("CameraCOIY")));
    m_simulationWidget->setCOIz(float(Preferences::valueDouble("CameraCOIZ")));
    m_simulationWidget->setFrontClip(float(Preferences::valueDouble("CameraFrontClip")));
    m_simulationWidget->setBackClip(float(Preferences::valueDouble("CameraBackClip")));
    m_simulationWidget->setUpX(float(Preferences::valueDouble("CameraUpX")));
    m_simulationWidget->setUpY(float(Preferences::valueDouble("CameraUpY")));
    m_simulationWidget->setUpZ(float(Preferences::valueDouble("CameraUpZ")));
    m_simulationWidget->setOrthographicProjection(Preferences::valueBool("OrthographicFlag"));

    QColor cursorColour = Preferences::valueQColor("CursorColour");
    m_simulationWidget->setCursorColour(QColor(cursorColour.red(), cursorColour.green(), cursorColour.blue(), cursorColour.alpha()));
    m_simulationWidget->setCursorRadius(float(Preferences::valueDouble("CursorRadius")));
    m_simulationWidget->setCursor3DNudge(float(Preferences::valueDouble("CursorNudge")));

    m_simulationWidget->setWireframe(Preferences::valueBool("DisplayAsWireframe"));
    m_simulationWidget->setShadows(Preferences::valueBool("DisplayShadows"));

    ui->doubleSpinBoxDistance->setValue(Preferences::valueDouble("CameraDistance"));
    ui->doubleSpinBoxFoV->setValue(Preferences::valueDouble("CameraFoV"));
    ui->doubleSpinBoxCOIX->setValue(Preferences::valueDouble("CameraCOIX"));
    ui->doubleSpinBoxCOIY->setValue(Preferences::valueDouble("CameraCOIY"));
    ui->doubleSpinBoxCOIZ->setValue(Preferences::valueDouble("CameraCOIZ"));
    ui->doubleSpinBoxFar->setValue(Preferences::valueDouble("CameraBackClip"));
    ui->doubleSpinBoxNear->setValue(Preferences::valueDouble("CameraFrontClip"));
    ui->doubleSpinBoxTrackingOffset->setValue(Preferences::valueDouble("TrackingOffset"));

    ui->radioButtonTrackingNone->setChecked(Preferences::valueBool("TrackingFlagNone"));
    ui->radioButtonTrackingX->setChecked(Preferences::valueBool("TrackingFlagX"));
    ui->radioButtonTrackingY->setChecked(Preferences::valueBool("TrackingFlagY"));
    ui->radioButtonTrackingZ->setChecked(Preferences::valueBool("TrackingFlagZ"));

    ui->comboBoxMuscleColourMap->setCurrentIndex(Preferences::valueInt("StrapColourControl"));
    ui->comboBoxMuscleDraw->setCurrentIndex(Preferences::valueInt("MuscleDrawStyle"));

    ui->spinBoxSkip->setValue(Preferences::valueInt("MovieSkip"));
    ui->doubleSpinBoxFPS->setValue(Preferences::valueDouble("MovieFramerate"));

    ui->doubleSpinBoxCursorNudge->setValue(Preferences::valueDouble("CursorNudge"));
    ui->doubleSpinBoxCursorSize->setValue(Preferences::valueDouble("CursorRadius"));

}

void MainWindow::writeSettings()
{
    Preferences::insert("CameraVecX", double(m_simulationWidget->cameraVecX()));
    Preferences::insert("CameraVecY", double(m_simulationWidget->cameraVecY()));
    Preferences::insert("CameraVecZ", double(m_simulationWidget->cameraVecZ()));

    Preferences::insert("MainWindowGeometry", saveGeometry());
    Preferences::insert("MainWindowState", saveState());
    Preferences::insert("MainWindowSplitter1State", ui->splitter1->saveState());
    Preferences::insert("MainWindowSplitter2State", ui->splitter2->saveState());
    Preferences::insert("ElementTreeHeaderState", ui->treeWidgetElements->header()->saveState());
    Preferences::Write();
}


void MainWindow::setStatusString(const QString &s, int logLevel)
{
    QStringList lines = s.split("\n", Qt::SkipEmptyParts);
    if (lines.size() == 0) { statusBar()->showMessage(s); }
    else { statusBar()->showMessage(lines[0]); }
    statusBar()->repaint();
    if (logLevel <= m_logLevel) log(s);
}

void MainWindow::setUICOI(float x, float y, float z)
{
    ui->doubleSpinBoxCOIX->setValue(double(x));
    ui->doubleSpinBoxCOIY->setValue(double(y));
    ui->doubleSpinBoxCOIZ->setValue(double(z));
}

void MainWindow::setUIFoV(float v)
{
    ui->doubleSpinBoxFoV->setValue(double(v));
}

void MainWindow::resizeSimulationWindow(int openGLWidth, int openGLHeight)
{
    showNormal();
    QScreen *screen = this->screen();
    if (!screen)
    {
        setStatusString("Error: Unable to access screen for resize", 0);
        return;
    }
    qreal openGLScale = devicePixelRatio();

    QRect available = screen->availableGeometry();
    if (available.width() * openGLScale < openGLWidth || available.height() * openGLScale < openGLHeight)
    {
        setStatusString(QString("Error: max screen for resize width = %1 height = %2").arg(available.width() * openGLScale).arg(available.height() * openGLScale), 0);
        return;
    }
    move(available.left(), available.top());

    int targetWidth = std::ceil(openGLWidth / openGLScale);
    int targetHeight = std::ceil(openGLHeight / openGLScale);
    int repeatCount = 0;
    const int maxRepeat = 16;
    while ((m_simulationWidget->width() * openGLScale != openGLWidth || m_simulationWidget->height() * openGLScale != openGLHeight))
    {
        qreal deltaOpenGLWidth = openGLWidth - m_simulationWidget->width() * openGLScale;
        qreal deltaOpenGLHeight = openGLHeight - m_simulationWidget->height() * openGLScale;
        int deltaW = deltaOpenGLWidth == 0 ? deltaOpenGLWidth : (deltaOpenGLWidth < 0 ? std::floor(deltaOpenGLWidth / openGLScale) : std::ceil(deltaOpenGLWidth / openGLScale));
        int deltaH = deltaOpenGLHeight == 0 ? deltaOpenGLHeight : (deltaOpenGLHeight < 0 ? std::floor(deltaOpenGLHeight / openGLScale) : std::ceil(deltaOpenGLHeight / openGLScale));
        resize(width() + deltaW, height() + deltaH);
        ++repeatCount;
        if (repeatCount >= maxRepeat)
        {
            m_simulationWidget->resize(targetWidth, targetHeight);
            break;
        }
    }
    m_simulationWidget->update();
    if (m_simulationWidget->width() * openGLScale != openGLWidth || m_simulationWidget->height() * openGLScale != openGLHeight)
    {
        setStatusString(QString("Error: unable to achieve requested size: width = %1 height = %2 magnification = %3").arg(m_simulationWidget->width() * openGLScale).arg(m_simulationWidget->height() * openGLScale).arg(openGLScale), 0);
        return;
    }
    setStatusString(QString("Simulation widget width = %1 height = %2").arg(m_simulationWidget->width() * openGLScale).arg(m_simulationWidget->height() * openGLScale), 1);
}

SimulationWidget *MainWindow::simulationWidget() const
{
    return m_simulationWidget;
}

GaitSym::Simulation *MainWindow::simulation() const
{
    return m_simulation;
}

void MainWindow::resizeAndCentre(int w, int h)
{
    QRect available = screen()->availableGeometry();

    // Need to find how big the central widget is compared to the window
    int heightDiff = height() - m_simulationWidget->height();
    int widthDiff = width() - m_simulationWidget->width();
    int newWidth = w + widthDiff;
    int newHeight = h + heightDiff;

    // centre window
    int topLeftX = available.left() + (available.width() / 2) - (newWidth / 2);
    int topLeftY = available.top() + (available.height() / 2) - (newHeight / 2);
    // but don't start off screen
    if (topLeftX < available.left()) topLeftX = available.left();
    if (topLeftY < available.top()) topLeftY = available.top();

    move(topLeftX, topLeftY);
    resize(newWidth, newHeight);
    m_simulationWidget->update();
}

void MainWindow::reportOpenGLSize(int width, int height)
{
    setStatusString(QString("OpenGL width = %1 height = %2").arg(width).arg(height), 2);
}

void MainWindow::log(const QString &text)
{
    if (text.trimmed().size()) // only log strings with content
    {
        ui->textEditLog->append(text);
        ui->textEditLog->repaint();
    }
}


QByteArray MainWindow::readResource(const QString &resource)
{
    QFile file(resource);
    bool ok = file.open(QIODevice::ReadOnly);
    Q_ASSERT_X(ok, "MainWindow::readResource", "resource not found");
    return file.readAll();
}


void MainWindow::updateEnable()
{
    qDebug() << "void MainWindow::updateEnable()";
    qDebug() << "m_simulation = " << m_simulation;
    qDebug() << "m_mode = " << m_mode;
    qDebug() << "m_noName = " << m_noName;
    qDebug() << "isWindowModified() = " << isWindowModified();
    qDebug() << "m_stepCount = " << m_stepCount;
    if (m_simulation)
    {
        qDebug() << "m_simulation->GetBodyList()->size() = " << m_simulation->GetBodyList()->size();
        qDebug() << "m_simulation->GetMuscleList()->size() = " << m_simulation->GetMuscleList()->size();
        qDebug() << "m_simulation->GetMarkerList()->size() = " << m_simulation->GetMarkerList()->size();
        qDebug() << "m_simulation->GetControllerList()->size() = " << m_simulation->GetControllerList()->size();
        qDebug() << "m_simulation->HasAssembly() = " << m_simulation->HasAssembly();
    }
    ui->actionOutput->setEnabled(m_simulation != nullptr);
    ui->actionRestart->setEnabled(m_simulation != nullptr && m_mode == runMode && m_noName == false && isWindowModified() == false);
    ui->actionSave->setEnabled(m_simulation != nullptr && m_noName == false && isWindowModified() == true);
    ui->actionSaveAs->setEnabled(m_simulation != nullptr);
    ui->actionRawXMLEditor->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionRenameElement->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionCreateMirrorElements->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 0);
    ui->actionCreateStringOfPearls->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 0);
    ui->actionCreateTestingDrivers->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetMuscleList()->size() > 0);
    ui->actionExportMarkers->setEnabled(m_simulation != nullptr);
    ui->actionExportOpenSim->setEnabled(m_simulation != nullptr);
    ui->actionExportMuJoCo->setEnabled(m_simulation != nullptr);
    ui->actionRecordMovie->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false);
    ui->actionRun->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false);
    ui->actionStep->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false);
    ui->actionSnapshot->setEnabled(m_simulation != nullptr);
    ui->actionSaveOBJSnapshot->setEnabled(m_simulation != nullptr && m_mode == runMode);
    ui->actionStartOBJSequence->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false && m_saveOBJFileSequenceFlag == false);
    ui->actionStopOBJSequence->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false && m_saveOBJFileSequenceFlag == true && m_objFileFormat == obj);
    ui->actionSaveUSDSnapshot->setEnabled(m_simulation != nullptr && m_mode == runMode);
    ui->actionStartUSDSequence->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false && m_saveOBJFileSequenceFlag == false);
    ui->actionStopUSDSequence->setEnabled(m_simulation != nullptr && m_mode == runMode && isWindowModified() == false && m_saveOBJFileSequenceFlag == true && m_objFileFormat == usda);
    ui->actionImportMarkers->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionImportMeshesAsBodies->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionPlaybackOpenSimBodyKinematics->setEnabled(m_simulation != nullptr && m_mode == runMode && m_noName == false && isWindowModified() == false && m_stepCount == 0);
    ui->actionClearKinematics->setEnabled(m_simulation != nullptr && m_mode == runMode && m_simulation->kinematicsFile().size() > 0);
    ui->actionCreateBody->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionCreateMarker->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 0);
    ui->actionCreateJoint->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 1 && m_simulation->GetMarkerList()->size() > 0);
    ui->actionCreateMuscle->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 1 && m_simulation->GetMarkerList()->size() > 0);
    ui->actionCreateGeom->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 0 && m_simulation->GetMarkerList()->size() > 0);
    ui->actionCreateDriver->setEnabled(m_simulation != nullptr && m_mode == constructionMode && (m_simulation->GetMuscleList()->size() > 0 || m_simulation->GetControllerList()->size() > 0));
    ui->actionEditGlobal->setEnabled(m_simulation != nullptr && m_mode == constructionMode);
    ui->actionCreateAssembly->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->GetBodyList()->size() > 0);
    ui->actionDeleteAssembly->setEnabled(m_simulation != nullptr && m_mode == constructionMode && m_simulation->HasAssembly());
    ui->actionConstructionMode->setEnabled(m_simulation != nullptr && m_mode == runMode && m_stepCount == 0);
    ui->actionRunMode->setEnabled(m_simulation != nullptr && m_mode == constructionMode && isWindowModified() == false && m_simulation->GetBodyList()->size() > 0);
    ui->actionDisplayAsWireframe->setChecked(m_simulationWidget->wireframe());
    ui->actionDisplayShadows->setChecked(Preferences::valueBool("DisplayShadows")); // m_simulationWidget->shadows() is never updated so this is the value after restart
}


MainWindow::Mode MainWindow::mode() const
{
    return m_mode;
}



void MainWindow::deleteExistingBody(const QString &name, bool force)
{
    GaitSym::Body *body = m_simulation->GetBody(name.toStdString());
    if (!body)
    {
        QMessageBox::warning(this, tr("Delete Body %1").arg(name), tr("Body cannot be found. Aborting delete."));
        return;
    }

    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(body))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Body %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the body itself
        ui->treeWidgetElements->removeBody(QString::fromStdString(body->name()));
        m_simulation->DeleteNamedObject(body->name());
        updateComboBoxTrackingMarker();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }
}

void MainWindow::deleteExistingMarker(const QString &name, bool force)
{
    GaitSym::Marker *marker = m_simulation->GetMarker(name.toStdString());
    if (!marker)
    {
        QMessageBox::warning(this, tr("Delete Marker %1").arg(name), tr("Marker cannot be found. Aborting delete."));
        return;
    }
    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(marker))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Marker %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the marker itself
        ui->treeWidgetElements->removeMarker(QString::fromStdString(marker->name()));
        m_simulation->DeleteNamedObject(marker->name());
        updateComboBoxTrackingMarker();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }
}

void MainWindow::deleteExistingJoint(const QString &name, bool force)
{
    GaitSym::Joint *joint = m_simulation->GetJoint(name.toStdString());
    if (!joint)
    {
        QMessageBox::warning(this, tr("Delete Joint %1").arg(name), tr("Joint cannot be found. Aborting delete."));
        return;
    }
    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(joint))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Joint %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the marker itself
        ui->treeWidgetElements->removeJoint(QString::fromStdString(joint->name()));
        m_simulation->DeleteNamedObject(joint->name());
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }
}

void MainWindow::deleteExistingMuscle(const QString &name, bool force)
{
    GaitSym::Muscle *muscle = m_simulation->GetMuscle(name.toStdString());
    if (!muscle)
    {
        QMessageBox::warning(this, tr("Delete Muscle %1").arg(name), tr("Muscle cannot be found. Aborting delete."));
        return;
    }
    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(muscle))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Muscle %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the marker itself
        ui->treeWidgetElements->removeMuscle(QString::fromStdString(muscle->name()));
        m_simulation->DeleteNamedObject(muscle->GetStrap()->name());
        m_simulation->DeleteNamedObject(muscle->name());
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }
}

void MainWindow::deleteExistingDriver(const QString &name, bool force)
{
    GaitSym::Driver *driver = m_simulation->GetDriver(name.toStdString());
    if (!driver)
    {
        QMessageBox::warning(this, tr("Delete Driver %1").arg(name), tr("Driver cannot be found. Aborting delete."));
        return;
    }
    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(driver))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Driver %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the marker itself
        ui->treeWidgetElements->removeDriver(QString::fromStdString(driver->name()));
        m_simulation->DeleteNamedObject(driver->name());
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }}

void MainWindow::deleteExistingGeom(const QString &name, bool force)
{
    GaitSym::Geom *geom = m_simulation->GetGeom(name.toStdString());
    if (!geom)
    {
        QMessageBox::warning(this, tr("Delete Geom %1").arg(name), tr("Geom cannot be found. Aborting delete."));
        return;
    }
    // get a list of dependencies
    std::string dependencyMessage;
    std::vector<GaitSym::NamedObject *> dependencyList;
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(geom))
        {
            dependencyMessage += it->name() + " "s;
            dependencyList.push_back(it);
        }
    }
    QString message;
    if (dependencyMessage.size()) message += "The following dependencies will be deleted:\n" + QString::fromStdString(dependencyMessage) + QString("\n");
    message += "This action cannot be undone.\nAre you sure you want to continue?";
    int ret = QMessageBox::Ok;
    if (!force) ret = QMessageBox::warning(this, tr("Delete Geom %1").arg(name), message, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        // first delete dependencies
        for (auto &&it : dependencyList)
        {
            ui->treeWidgetElements->removeName(QString::fromStdString(it->name()));
            m_simulation->DeleteNamedObject(it->name());
        }
        // now delete the marker itself
        ui->treeWidgetElements->removeGeom(QString::fromStdString(geom->name()));
        m_simulation->DeleteNamedObject(geom->name());
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
        setWindowModified(true);
        updateEnable();
        m_simulationWidget->update();
    }
}

void MainWindow::editExistingBody(const QString &name)
{
    GaitSym::Body *body = m_simulation->GetBody(name.toStdString());
    this->menuCreateEditBody(body);
}

void MainWindow::editExistingMarker(const QString &name)
{
    GaitSym::Marker *marker = m_simulation->GetMarker(name.toStdString());
    this->menuCreateEditMarker(marker);
}

void MainWindow::editExistingJoint(const QString &name)
{
    GaitSym::Joint *joint = m_simulation->GetJoint(name.toStdString());
    this->menuCreateEditJoint(joint);
}

void MainWindow::editExistingMuscle(const QString &name)
{
    GaitSym::Muscle *muscle = m_simulation->GetMuscle(name.toStdString());
    this->menuCreateEditMuscle(muscle);
}

void MainWindow::editExistingGeom(const QString &name)
{
    GaitSym::Geom *geom = m_simulation->GetGeom(name.toStdString());
    this->menuCreateEditGeom(geom);
}

void MainWindow::editExistingDriver(const QString &name)
{
    GaitSym::Driver *driver = m_simulation->GetDriver(name.toStdString());
    this->menuCreateEditDriver(driver);
}


void MainWindow::comboBoxMeshDisplayMapCurrentTextChanged(const QString &text)
{
    if (text == QString("Mesh 1"))
    {
        m_simulationWidget->setDrawBodyMesh1(true);
        m_simulationWidget->setDrawBodyMesh2(false);
        m_simulationWidget->setDrawBodyMesh3(false);
    }
    if (text == QString("Mesh 2"))
    {
        m_simulationWidget->setDrawBodyMesh1(false);
        m_simulationWidget->setDrawBodyMesh2(true);
        m_simulationWidget->setDrawBodyMesh3(false);
    }
    if (text == QString("Mesh 3"))
    {
        m_simulationWidget->setDrawBodyMesh1(false);
        m_simulationWidget->setDrawBodyMesh2(false);
        m_simulationWidget->setDrawBodyMesh3(true);
    }
    if (text == QString("Mesh 1 & 2"))
    {
        m_simulationWidget->setDrawBodyMesh1(true);
        m_simulationWidget->setDrawBodyMesh2(true);
        m_simulationWidget->setDrawBodyMesh3(false);
    }
    if (text == QString("Mesh 2 & 3"))
    {
        m_simulationWidget->setDrawBodyMesh1(false);
        m_simulationWidget->setDrawBodyMesh2(true);
        m_simulationWidget->setDrawBodyMesh3(true);
    }
    if (text == QString("Mesh 1 & 3"))
    {
        m_simulationWidget->setDrawBodyMesh1(true);
        m_simulationWidget->setDrawBodyMesh2(false);
        m_simulationWidget->setDrawBodyMesh3(true);
    }
    if (text == QString("All Meshes"))
    {
        m_simulationWidget->setDrawBodyMesh1(true);
        m_simulationWidget->setDrawBodyMesh2(true);
        m_simulationWidget->setDrawBodyMesh3(true);
    }
    m_simulationWidget->update();
}

void MainWindow::comboBoxProjectionCurrentTextChanged(const QString &text)
{
    if (text == QString("Orthographic")) { m_simulationWidget->setOrthographicProjection(true); }
    if (text == QString("Perspective")) { m_simulationWidget->setOrthographicProjection(false); }
    m_simulationWidget->update();
}

void MainWindow::comboBoxTrackingMarkerCurrentTextChanged(const QString &text)
{
    if (m_simulation)
    {
        GaitSym::Marker *marker = m_simulation->GetMarker(text.toStdString());
        if (marker) Preferences::insert("TrackMarkerID", text);
    }
}


void MainWindow::handleElementTreeWidgetItemChanged(QTreeWidgetItem * /* item */, int column)
{
    if (column == 1) m_simulationWidget->update();
}



void MainWindow::moveExistingMarker(const QString &s, const QVector3D &p)
{
    auto markerIt = m_simulation->GetMarkerList()->find(s.toStdString());
    if (markerIt == m_simulation->GetMarkerList()->end()) return;
    markerIt->second->SetWorldPosition(double(p.x()), double(p.y()), double(p.z()));
    markerIt->second->setRedraw(true);
    std::vector<GaitSym::NamedObject *> objectList = m_simulation->GetObjectList();
    for (auto &&it : objectList)
    {
        if (it->isUpstreamObject(markerIt->second.get()))
        {
            it->saveToAttributes();
            it->createFromAttributes();
            it->setRedraw(true);
            // everything needs a redraw but some things also need extra work
            if (dynamic_cast<GaitSym::Strap *>(it)) dynamic_cast<GaitSym::Strap *>(it)->Calculate();
        }
    }
    setWindowModified(true);
    updateEnable();
    m_simulationWidget->update();
}

void MainWindow::updateComboBoxTrackingMarker()
{
    const QSignalBlocker blocker(ui->comboBoxTrackingMarker);
    ui->comboBoxTrackingMarker->clear();
    if (!m_simulation) return;
    QString currentTrackMarker = Preferences::valueQString("TrackMarkerID");
    int currentTrackMarkerIndex = -1;
    int count = 0;
    for (auto &&markerIt : *m_simulation->GetMarkerList())
    {
        QString currentMarker = QString::fromStdString(markerIt.first);
        ui->comboBoxTrackingMarker->addItem(currentMarker);
        if (currentMarker == currentTrackMarker) currentTrackMarkerIndex = count;
        count++;
    }
    ui->comboBoxTrackingMarker->setCurrentIndex(currentTrackMarkerIndex);
}

void MainWindow::menuOpen()
{
    if (this->isWindowModified())
    {
        int ret = QMessageBox::warning(this, tr("Current document contains unsaved changes"),
                                       tr("Opening a new document will delete the current document.\nAre you sure you want to continue?"),
                                       QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel) return;
    }

    QFileInfo info(Preferences::valueQString("LastFileOpened"));
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this, tr("Open Config File"), info.absoluteFilePath(), tr("Config Files (*.gaitsym *.xml);;Any File (*.* *)"), nullptr);
    if (fileName.isNull() == false)
    {
        Preferences::insert("LastFileOpened", fileName);
        menuOpen(fileName, nullptr);
    }
}

void MainWindow::menuOpen(const QString &fileName, const QByteArray *fileData)
{
    // dispose any simulation cleanly
    this->m_timer->stop();
    this->ui->actionRun->setChecked(false);
    if (this->m_movieFlag) { menuStopAVISave(); }
    this->m_saveOBJFileSequenceFlag = false;
    if (this->m_simulationWidget->aviWriter()) menuStopAVISave();
    std::string kinematicsFile;
    if (this->m_simulation)
    {
        kinematicsFile = m_simulation->kinematicsFile();
        delete this->m_simulation;
        this->m_simulation = nullptr;
        this->m_simulationWidget->setSimulation(this->m_simulation);
    }
    this->m_stepCount = 0;
    this->m_stepFlag = false;

    this->m_configFile.setFile(fileName);
    QDir::setCurrent(this->m_configFile.absolutePath());
    QString canonicalFilePath = this->m_configFile.canonicalFilePath();
    QDir currentDir(this->m_configFile.absolutePath());
    Preferences::insert("LastFileOpened", canonicalFilePath);

    this->setStatusString(fileName + QString(" loading"), 2);
    qApp->processEvents();

    std::string *errorMessage = nullptr;
    if (fileData)
    {
        this->m_simulation = new GaitSym::Simulation();
        errorMessage = this->m_simulation->LoadModel(fileData->constData(), fileData->size());
    }
    else
    {
        GaitSym::DataFile file;
        int err;
        err = file.ReadFile(canonicalFilePath.toStdString());
        if (err)
        {
            this->setStatusString(QString("Error reading ") + canonicalFilePath, 0);
            this->updateEnable();
            return;
        }
        this->m_simulation = new GaitSym::Simulation();
        errorMessage = this->m_simulation->LoadModel(file.GetRawData(), file.GetSize());
    }
    if (errorMessage)
    {
        this->setStatusString(QString::fromStdString(*errorMessage), 0);
        delete this->m_simulation;
        this->m_simulation = nullptr;
        this->m_simulationWidget->setSimulation(this->m_simulation);
        this->m_simulationWidget->update();
        this->updateEnable();
        return;
    }
    this->ui->treeWidgetElements->fillVisibitilityLists(this->m_simulation);

    // check we can find the meshes
    QStringList searchPath;
    for (size_t i = 0; i < this->m_simulation->GetGlobal()->MeshSearchPath()->size(); i++)
        searchPath.append(QString::fromStdString(this->m_simulation->GetGlobal()->MeshSearchPath()->at(i)));
    bool noToAll = false;
    bool meshPathChanged = false;
    for (auto &&iter : *this->m_simulation->GetBodyList())
    {
        std::vector<std::string> meshNames = {iter.second->GetGraphicFile1(), iter.second->GetGraphicFile2(), iter.second->GetGraphicFile3()};
        for (size_t nameIndex = 0;  nameIndex < meshNames.size(); nameIndex++)
        {
            auto &&meshName = meshNames[nameIndex];
            if (meshName.size() == 0) continue;
            bool fileFound = false;
            QString graphicsFile = QString::fromStdString(meshName);
            for (int i = 0; i < searchPath.size(); i++)
            {
                QDir dir(searchPath[i]);
                if (dir.exists(graphicsFile))
                {
                    fileFound = true;
                    break;
                }
            }
            if (fileFound == false)
            {
                int ret = QMessageBox::warning(this, QString("Loading ") + canonicalFilePath,
                                               QString("Unable to find \"") + graphicsFile + QString("\"\nDo you want to load a new file?\nThis will alter the Mesh Path and the file will require saving to make this change permanent."),
                                               QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll, QMessageBox::Yes);
                if (ret == QMessageBox::NoToAll) noToAll = true;
                if (ret == QMessageBox::Yes)
                {
                    QString newFile = QFileDialog::getOpenFileName(this, tr("Open Mesh File"), graphicsFile, tr("Mesh Files (*.obj *.ply)"), nullptr);
                    if (newFile.isNull() == false)
                    {
                        QFileInfo newFileInfo(newFile);
                        switch (nameIndex)
                        {
                        case 0:
                            iter.second->SetGraphicFile1(newFileInfo.fileName().toStdString());
                            break;
                        case 1:
                            iter.second->SetGraphicFile2(newFileInfo.fileName().toStdString());
                            break;
                        case 2:
                            iter.second->SetGraphicFile3(newFileInfo.fileName().toStdString());
                            break;
                        default:
                            qDebug() << "Error IN MainWindow::menuOpen(): invalid nameIndex = " << nameIndex;
                        }
                        QString newPath = currentDir.relativeFilePath(newFileInfo.absolutePath());
                        if (newPath.size() == 0) newPath = ".";
                        searchPath.append(newPath);
                        meshPathChanged = true;
                    }
                }
            }
            if (noToAll) break;
        }
        if (noToAll) break;
    }
    if (meshPathChanged)
    {
        this->m_simulation->GetGlobal()->MeshSearchPath()->clear();
        for (int i = 0; i < searchPath.size(); i++) this->m_simulation->GetGlobal()->MeshSearchPath()->push_back(searchPath[i].toStdString());
    }

    this->m_simulationWidget->setAxesScale(float(this->m_simulation->GetGlobal()->size1()));
    QString backgroundColour = QString::fromStdString(this->m_simulation->GetGlobal()->colour1().GetHexARGB());
    this->m_simulationWidget->setSimulation(this->m_simulation);
    this->m_simulationWidget->setBackgroundColour(QColor(backgroundColour));
    this->m_simulationWidget->update();
    // this->m_simulation->setDrawContactForces(Preferences::valueBool("DisplayContactForces"));
    //  this->m_simulation->Draw(this->m_simulationWidget);
    this->radioButtonTracking();

    this->ui->doubleSpinBoxTimeMax->setValue(this->m_simulation->GetTimeLimit());
    QString time = QString("%1").arg(double(0), 0, 'f', 5);
    this->ui->lcdNumberTime->display(time);

    this->setStatusString(fileName + QString(" loaded"), 1);

    this->comboBoxMuscleColourMapCurrentTextChanged(this->ui->comboBoxMuscleColourMap->currentText());
    this->comboBoxMeshDisplayMapCurrentTextChanged(this->ui->comboBoxMeshDisplay->currentText());

    // put the filename as a title
    //if (canonicalFilePath.size() <= 256) this->setWindowTitle(canonicalFilePath + "[*]");
    //else this->setWindowTitle(QString("...") + canonicalFilePath.right(256) + "[*]");
    this->setWindowTitle(canonicalFilePath + "[*]");

    updateRecentFiles(canonicalFilePath);

    // set menu activations for loaded model
    this->m_noName = false;
    if (meshPathChanged)
    {
        this->setWindowModified(true);
        enterConstructionMode();
    }
    else
    {
        this->setWindowModified(false);
        enterRunMode();
    }

    if (kinematicsFile.size() > 0) { m_simulation->setKinematicsFile(kinematicsFile); }
    this->updateComboBoxTrackingMarker();
    this->handleTracking();

    this->updateEnable();
    Preferences::Write();
}

void MainWindow::menuRestart()
{
    menuOpen(this->m_configFile.absoluteFilePath(), nullptr);
}

void MainWindow::menuSaveAs()
{
    QString fileName;
    if (this->m_configFile.absoluteFilePath().isEmpty())
    {
        QFileInfo info(Preferences::valueQString("LastFileOpened"));
        fileName = QFileDialog::getSaveFileName(this, tr("Save Model State File "), info.absoluteFilePath(), tr("Config Files (*.gaitsym);;XML files (*.xml)"), nullptr);
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Model State File"), this->m_configFile.absoluteFilePath(), tr("Config Files (*.gaitsym);;XML files (*.xml)"), nullptr);
    }

    if (fileName.isNull() == false)
    {
        if (this->m_mode == MainWindow::constructionMode) // need to put everything into run mode to save properly
        {
            for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterRunMode();
            for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
            for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
            for (auto &&it : *this->m_simulation->GetJointList()) it.second->LateInitialisation();
        }
        this->setStatusString(fileName + QString(" saving"), 2);
        this->m_configFile.setFile(fileName);
        QDir currentDir(this->m_configFile.absolutePath());
        QString meshPath, relativeMeshPath;
        for (auto &&it : *this->m_simulation->GetBodyList())
        {
            meshPath = QString::fromStdString(it.second->GetGraphicFile1());
            relativeMeshPath = currentDir.relativeFilePath(meshPath);
            it.second->SetGraphicFile1(relativeMeshPath.toStdString());
            meshPath = QString::fromStdString(it.second->GetGraphicFile2());
            relativeMeshPath = currentDir.relativeFilePath(meshPath);
            it.second->SetGraphicFile2(relativeMeshPath.toStdString());
            meshPath = QString::fromStdString(it.second->GetGraphicFile3());
            relativeMeshPath = currentDir.relativeFilePath(meshPath);
            it.second->SetGraphicFile3(relativeMeshPath.toStdString());
        }
        this->m_simulation->SetOutputModelStateFile(fileName.toStdString());
        this->m_simulation->OutputProgramState();
        this->setStatusString(fileName + QString(" saved"), 1);
        QDir::setCurrent(this->m_configFile.absolutePath());
        Preferences::insert("LastFileOpened", this->m_configFile.canonicalFilePath());
        // if (fileName.size() <= 256) this->setWindowTitle(fileName + "[*]");
        // else this->setWindowTitle(QString("...") + fileName.right(256) + "[*]");
        this->setWindowTitle(this->m_configFile.canonicalFilePath());
        updateRecentFiles(this->m_configFile.canonicalFilePath());
        this->m_noName = false;
        this->setWindowModified(false);
        if (this->m_mode == MainWindow::constructionMode)
        {
            for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterConstructionMode();
            for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
            for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
        }
        Preferences::Write();
        this->updateEnable();
    }
    else
    {
        this->ui->statusBar->showMessage(QString("Save As... cancelled"));
    }
}

void MainWindow::menuSave()
{
    if (this->m_noName) return;
    if (this->m_mode == MainWindow::constructionMode) // need to put everything into run mode to save properly
    {
        for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterRunMode();
        for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
        for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
        for (auto &&it : *this->m_simulation->GetJointList()) it.second->LateInitialisation();
    }
    QString fileName = this->m_configFile.absoluteFilePath();
    QDir currentDir(this->m_configFile.absolutePath());
    QString meshPath, relativeMeshPath;
    for (auto &&it : *this->m_simulation->GetBodyList())
    {
        meshPath = QString::fromStdString(it.second->GetGraphicFile1());
        relativeMeshPath = currentDir.relativeFilePath(meshPath);
        it.second->SetGraphicFile1(relativeMeshPath.toStdString());
        meshPath = QString::fromStdString(it.second->GetGraphicFile2());
        relativeMeshPath = currentDir.relativeFilePath(meshPath);
        it.second->SetGraphicFile2(relativeMeshPath.toStdString());
        meshPath = QString::fromStdString(it.second->GetGraphicFile3());
        relativeMeshPath = currentDir.relativeFilePath(meshPath);
        it.second->SetGraphicFile3(relativeMeshPath.toStdString());
    }
    this->setStatusString(fileName + QString(" saving"), 2);
    this->m_simulation->SetOutputModelStateFile(fileName.toStdString());
    this->m_simulation->OutputProgramState();
    this->setStatusString(fileName + QString(" saved"), 1);
    this->setWindowModified(false);
    if (this->m_mode == MainWindow::constructionMode)
    {
        for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterConstructionMode();
        for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
        for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
    }
    Preferences::Write();
    this->updateEnable();
}

void MainWindow::menuExportOpenSim()
{
    QString fileName;
    if (this->m_configFile.absoluteFilePath().isEmpty())
    {
        QFileInfo info(Preferences::valueQString("LastFileOpened"));
        fileName = QFileDialog::getSaveFileName(this, tr("Export Model as OpenSim File"), QDir(info.absolutePath()).filePath(info.completeBaseName()), tr("OpenSym Files (*.osim)"), nullptr);
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Model as OpenSim File"), QDir(this->m_configFile.absolutePath()).filePath(this->m_configFile.completeBaseName()), tr("Config Files (*.osim)"), nullptr);
    }

    if (fileName.isNull() == false)
    {
        setStatusString(QString("Exporting \"%1\"").arg(fileName), 1);
        GaitSym::OpenSimExporter openSimExporter;
        openSimExporter.setMocoExport(Preferences::valueBool("OpenSimMocoExport", false));
        for (auto &&it : *m_simulationWidget->getDrawBodyMap())
        {
            GaitSym::Body *body = m_simulation->GetBody(it.first);
            if (it.second->meshEntity1() && body)
            {
                FacetedObject temp;
                temp.AddFacetedObject(it.second->meshEntity1(), false, true); // body meshes have already been moved so their origin is the centre of mass
                QFileInfo info(fileName);
                QDir currentDir(info.absolutePath());
                QDir newDir(currentDir.absoluteFilePath("osim_meshes"));
                currentDir.mkdir("osim_meshes");
                openSimExporter.setPathToObjFiles("osim_meshes");
                temp.WriteOBJFile(newDir.absoluteFilePath(QString::fromStdString(body->GetGraphicFile1())).toStdString());
            }
        }
        openSimExporter.Process(m_simulation);
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            setStatusString(QString("Error opening \"%1\" for OpenSim export").arg(fileName), 0);
            return;
        }
        qint64 c = file.write(openSimExporter.xmlString()->data(), openSimExporter.xmlString()->size());
        if (c != openSimExporter.xmlString()->size())
        {
            setStatusString(QString("Error writing \"%1\" for OpenSim export").arg(fileName), 0);
            return;
        }
        setStatusString(QString("Exported \"%1\"").arg(fileName), 1);
    }
    else
    {
        setStatusString(QString("OpenSim export cancelled"), 1);
    }
}

void MainWindow::menuExportMuJoCo()
{
    QString fileName;
    if (this->m_configFile.absoluteFilePath().isEmpty())
    {
        QFileInfo info(Preferences::valueQString("LastFileOpened"));
        fileName = QFileDialog::getSaveFileName(this, tr("Export Model as MuJoCo File"), QDir(info.absolutePath()).filePath(info.completeBaseName()), tr("MuJoCo Files (*.xml)"), nullptr);
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save MuJoCo File"), QDir(this->m_configFile.absolutePath()).filePath(this->m_configFile.completeBaseName()), tr("Config Files (*.xml)"), nullptr);
    }

    if (fileName.isNull() == false)
    {
        setStatusString(QString("Exporting \"%1\"").arg(fileName), 1);
        GaitSym::MuJoCoPhysicsEngine muJoCoPhysicsEngine;
        std::string *err = muJoCoPhysicsEngine.Initialise(simulation());
        if (err)
        {
            setStatusString(QString("Error parsing \"%1\" for MuJoCo export").arg(fileName), 0);
            return;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            setStatusString(QString("Error opening \"%1\" for MuJoCo export").arg(fileName), 0);
            return;
        }
        std::string xmlString = muJoCoPhysicsEngine.mjXML();
        qint64 c = file.write(xmlString.data(), xmlString.size());
        if (c != xmlString.size())
        {
            setStatusString(QString("Error writing \"%1\" for MuJoCo export").arg(fileName), 0);
            return;
        }
        setStatusString(QString("Exported \"%1\"").arg(fileName), 1);
    }
    else
    {
        setStatusString(QString("MuJoCo export cancelled"), 1);
    }
}

void MainWindow::menuAbout()
{
    DialogAbout dialogAbout(this);

    int status = dialogAbout.exec();

    if (status == QDialog::Accepted)
    {
    }
}

void MainWindow::run()
{
    if (this->ui->actionRun->isChecked())
    {
        if (this->m_simulation) this->m_timer->start();
        this->setStatusString(tr("Simulation running"), 1);
    }
    else
    {
        this->m_timer->stop();
        this->setStatusString(tr("Simulation stopped"), 1);
    }
}
void MainWindow::step()
{
    this->m_stepFlag = true;
    if (this->m_simulation) this->m_timer->start();
    this->setStatusString(tr("Simulation stepped"), 2);
}

void MainWindow::snapshot()
{
    int count = 0;
    QDir dir(this->m_configFile.absolutePath());
    QStringList list = dir.entryList(QDir::Files | QDir::Dirs, QDir::Name);
    QStringList matches = list.filter(QRegularExpression(QString("^Snapshot\\d\\d\\d\\d\\d.*")));
    if (matches.size() > 0)
    {
        QString numberString = matches.last().mid(8, 5);
        count = numberString.toInt() + 1;
    }
    QString filename = dir.absoluteFilePath(QString("Snapshot%1.png").arg(count, 5, 10, QChar('0')));
    if (this->m_simulationWidget->WriteStillFrame(filename))
    {
        QMessageBox::warning(this, "Snapshot Error", QString("Could not write '%1'\n").arg(filename));
        return;
    }
    this->setStatusString(QString("\"%1\" saved").arg(filename), 1);
}

void MainWindow::objSnapshot()
{
    QFileInfo info(Preferences::valueQString("LastFileOpened"));

    QString folder = QFileDialog::getExistingDirectory(this, tr("Choose folder to save current view as OBJ files"), info.absolutePath());

    if (folder.isNull() == false)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        this->setStatusString(QString("Writing to \"%1\"").arg(folder), 1);
        if (this->m_simulationWidget->WriteCADFrame(folder))
        {
            this->setStatusString(QString("Error: Folder '%1' write fail\n").arg(folder), 0);
            return;
        }
        this->setStatusString(QString("Files written in '%1'\n").arg(folder), 1);
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::usdSnapshot()
{
    int count = 0;
    QDir dir(this->m_configFile.absolutePath());
    QStringList list = dir.entryList(QDir::Files | QDir::Dirs, QDir::Name);
    QStringList matches = list.filter(QRegularExpression(QString("^Snapshot\\d\\d\\d\\d\\d.*")));
    if (matches.size() > 0)
    {
        QString numberString = matches.last().mid(8, 5);
        count = numberString.toInt() + 1;
    }
    QString filename = dir.absoluteFilePath(QString("Snapshot%1.usda").arg(count, 5, 10, QChar('0')));
    // filename = QFileDialog::getSaveFileName(this, tr("Save current view as USD file"), filename, tr("Images (*.usd *.usda *.usdc)"));

    if (filename.isNull() == false)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        this->setStatusString(QString("Writing \"%1\"").arg(filename), 1);
        if (this->m_simulationWidget->WriteUSDFrame(filename))
        {
            QMessageBox::warning(this, "Snapshot Error", QString("Could not write '%1'\n").arg(filename));
            return;
        }
        this->setStatusString(QString("\"%1\" saved").arg(filename), 1);
        QApplication::restoreOverrideCursor();
    }
}


void MainWindow::menuRecordMovie()
{
    if (this->ui->actionRecordMovie->isChecked())
    {
        this->m_movieFlag = true;
        QFileInfo info(Preferences::valueQString("LastFileOpened"));
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save output as AVI file"), info.absolutePath(), tr("AVI Files (*.avi)"), nullptr);
        if (fileName.isNull() == false)
        {
            this->m_movieFlag = true;
            this->m_simulationWidget->StartAVISave(fileName);
        }
        else
        {
            this->m_movieFlag = false;
            this->ui->actionRecordMovie->setChecked(false);
        }
    }
    else
    {
        this->m_movieFlag = false;
        if (this->m_simulationWidget->aviWriter()) menuStopAVISave();
    }
}

void MainWindow::menuPreferences()
{
    DialogPreferences dialogPreferences(this);
    dialogPreferences.initialise();

    int status = dialogPreferences.exec();

    if (status == QDialog::Accepted)   // write the new settings
    {
        dialogPreferences.update();
        this->writeSettings();

        // these settings have immediate effect
        QColor cursorColour = Preferences::valueQColor("CursorColour");
        this->m_simulationWidget->setCursorColour(QColor(cursorColour.red(), cursorColour.green(), cursorColour.blue(), cursorColour.alpha()));
        this->m_simulationWidget->setCursorRadius(float(Preferences::valueDouble("CursorRadius")));
        this->m_simulationWidget->setCursor3DNudge(float(Preferences::valueDouble("CursorNudge")));
        this->m_simulationWidget->setFrontClip(float(Preferences::valueDouble("CameraFrontClip")));
        this->m_simulationWidget->setBackClip(float(Preferences::valueDouble("CameraBackClip")));

        this->m_simulationWidget->update();

    }

}

void MainWindow::menuOutputs()
{
    if (this->m_simulation == nullptr) return;
    DialogOutputSelect dialogOutputSelect(this);
    dialogOutputSelect.setSimulation(this->m_simulation);
    int status = dialogOutputSelect.exec();
    if (status == QDialog::Accepted)
    {
        this->ui->treeWidgetElements->fillVisibitilityLists(this->m_simulation);
        this->setStatusString(tr("Outputs set"), 1);
    }
    else
    {
        this->setStatusString(tr("Outputs setting cancelled"), 1);
    }
}

void MainWindow::menuLoadDefaultView()
{
    this->ui->doubleSpinBoxTrackingOffset->setValue(Preferences::valueDouble("DefaultTrackingOffset"));

    this->m_simulationWidget->setCameraDistance(float(Preferences::valueDouble("DefaultCameraDistance")));
    this->m_simulationWidget->setFOV(float(Preferences::valueDouble("DefaultCameraFoV")));
    this->m_simulationWidget->setCameraVecX(float(Preferences::valueDouble("DefaultCameraVecX")));
    this->m_simulationWidget->setCameraVecY(float(Preferences::valueDouble("DefaultCameraVecY")));
    this->m_simulationWidget->setCameraVecZ(float(Preferences::valueDouble("DefaultCameraVecZ")));
    this->m_simulationWidget->setCOIx(float(Preferences::valueDouble("DefaultCameraCOIX")));
    this->m_simulationWidget->setCOIy(float(Preferences::valueDouble("DefaultCameraCOIY")));
    this->m_simulationWidget->setCOIz(float(Preferences::valueDouble("DefaultCameraCOIZ")));
    this->m_simulationWidget->setUpX(float(Preferences::valueDouble("DefaultCameraUpX")));
    this->m_simulationWidget->setUpY(float(Preferences::valueDouble("DefaultCameraUpY")));
    this->m_simulationWidget->setUpZ(float(Preferences::valueDouble("DefaultCameraUpZ")));
    this->m_simulationWidget->setBackClip(float(Preferences::valueDouble("DefaultCameraBackClip")));
    this->m_simulationWidget->setFrontClip(float(Preferences::valueDouble("DefaultCameraFrontClip")));

    this->m_simulationWidget->update();
}

void MainWindow::menuSaveDefaultView()
{
    Preferences::insert("DefaultTrackingOffset", this->ui->doubleSpinBoxTrackingOffset->value());

    Preferences::insert("DefaultCameraDistance", this->m_simulationWidget->cameraDistance());
    Preferences::insert("DefaultCameraFoV", this->m_simulationWidget->FOV());
    Preferences::insert("DefaultCameraCOIX", this->m_simulationWidget->COIx());
    Preferences::insert("DefaultCameraCOIY", this->m_simulationWidget->COIy());
    Preferences::insert("DefaultCameraCOIZ", this->m_simulationWidget->COIz());
    Preferences::insert("DefaultCameraVecX", this->m_simulationWidget->cameraVecX());
    Preferences::insert("DefaultCameraVecY", this->m_simulationWidget->cameraVecY());
    Preferences::insert("DefaultCameraVecZ", this->m_simulationWidget->cameraVecZ());
    Preferences::insert("DefaultCameraUpX", this->m_simulationWidget->upX());
    Preferences::insert("DefaultCameraUpY", this->m_simulationWidget->upY());
    Preferences::insert("DefaultCameraUpZ", this->m_simulationWidget->upZ());
    Preferences::insert("DefaultCameraBackClip", this->m_simulationWidget->backClip());
    Preferences::insert("DefaultCameraFrontClip", this->m_simulationWidget->frontClip());
    Preferences::Write();
}

void MainWindow::menu640x480()
{
    this->resizeSimulationWindow(640, 480);
}

void MainWindow::menu800x600()
{
    this->resizeSimulationWindow(800, 600);
}

void MainWindow::menu1280x720()
{
    this->resizeSimulationWindow(1280, 720);
}

void MainWindow::menu1920x1080()
{
    this->resizeSimulationWindow(1920, 1080);
}

void MainWindow::buttonCameraRight()
{
    this->m_simulationWidget->setCameraVecX(0);
    this->m_simulationWidget->setCameraVecY(1);
    this->m_simulationWidget->setCameraVecZ(0);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(0);
    this->m_simulationWidget->setUpZ(1);
    this->m_simulationWidget->update();
}


void MainWindow::buttonCameraTop()
{
    this->m_simulationWidget->setCameraVecX(0);
    this->m_simulationWidget->setCameraVecY(0);
    this->m_simulationWidget->setCameraVecZ(-1);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(1);
    this->m_simulationWidget->setUpZ(0);
    this->m_simulationWidget->update();
}


void MainWindow::buttonCameraFront()
{
    this->m_simulationWidget->setCameraVecX(-1);
    this->m_simulationWidget->setCameraVecY(0);
    this->m_simulationWidget->setCameraVecZ(0);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(0);
    this->m_simulationWidget->setUpZ(1);
    this->m_simulationWidget->update();
}


void MainWindow::buttonCameraLeft()
{
    this->m_simulationWidget->setCameraVecX(0);
    this->m_simulationWidget->setCameraVecY(-1);
    this->m_simulationWidget->setCameraVecZ(0);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(0);
    this->m_simulationWidget->setUpZ(1);
    this->m_simulationWidget->update();
}


void MainWindow::buttonCameraBottom()
{
    this->m_simulationWidget->setCameraVecX(0);
    this->m_simulationWidget->setCameraVecY(0);
    this->m_simulationWidget->setCameraVecZ(1);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(1);
    this->m_simulationWidget->setUpZ(0);
    this->m_simulationWidget->update();
}


void MainWindow::buttonCameraBack()
{
    this->m_simulationWidget->setCameraVecX(1);
    this->m_simulationWidget->setCameraVecY(0);
    this->m_simulationWidget->setCameraVecZ(0);
    this->m_simulationWidget->setUpX(0);
    this->m_simulationWidget->setUpY(0);
    this->m_simulationWidget->setUpZ(1);
    this->m_simulationWidget->update();
}

void MainWindow::buttonDisplayAsWireframe()
{
    bool wireframe = !this->m_simulationWidget->wireframe();
    this->m_simulationWidget->setWireframe(wireframe);
    Preferences::insert("DisplayAsWireframe", wireframe);
    this->updateEnable();
    this->m_simulationWidget->update();
}

void MainWindow::buttonDisplayShadows()
{
    bool shadows = !Preferences::valueBool("DisplayShadows", "false"); // current version does not alter the value until restart !this->m_simulationWidget->shadows();
    Preferences::insert("DisplayShadows", shadows);
    this->updateEnable();
    // because turning shadows on and off causes instability, do not do anything other than tell the user
    // this->m_simulationWidget->setShadows(shadows);
    // this->m_simulationWidget->update();
    if (shadows != this->m_simulationWidget->shadows())
    {
        if (shadows) QMessageBox::information(this, "Shadow Display", "Shadows enabled. Restart to see effect.");
        else QMessageBox::information(this, "Shadow Display", "Shadows disabled. Restart to see effect.");
    }
    else
    {
        if (shadows) QMessageBox::information(this, "Shadow Display", "Reverting back to shadows enabled.");
        else QMessageBox::information(this, "Shadow Display", "Reverting back to shadows disabled.");
    }
}


void MainWindow::menuStopAVISave()
{
    this->m_movieFlag = false;
    this->m_simulationWidget->StopAVISave();
}

void MainWindow::menuStartOBJSequenceSave()
{
    this->m_objFileFormat = MainWindow::obj;
    QFileInfo info(Preferences::valueQString("LastFileOpened"));

    this->m_objFileSequenceFolder = QFileDialog::getExistingDirectory(this, tr("Choose folder for writing the OBJ file sequence"), info.absolutePath());

    if (this->m_objFileSequenceFolder.isNull() == false)
    {
        this->m_saveOBJFileSequenceFlag = true;    }
}

void MainWindow::menuStartUSDSequenceSave()
{
    this->m_objFileFormat = MainWindow::usda;
    QFileInfo info(Preferences::valueQString("LastFileOpened"));

    this->m_objFileSequenceFolder = QFileDialog::getExistingDirectory(this, tr("Choose folder for writing the USD file sequence"), info.absolutePath());

    if (this->m_objFileSequenceFolder.isNull() == false)
    {
        this->m_saveOBJFileSequenceFlag = true;
    }
}

void MainWindow::menuStopOBJSequenceSave()
{
    this->m_saveOBJFileSequenceFlag = false;
}

void MainWindow::menuStopUSDSequenceSave()
{
    this->m_saveOBJFileSequenceFlag = false;
}

void MainWindow::menuNew()
{
    if (this->isWindowModified())
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Click OK to create a new document, and Cancel to continue working on the current document");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            break;
        case QMessageBox::Cancel:
            return;
        default:
            // should never be reached
            return;
        }
    }

    DialogGlobal dialogGlobal(this);
    dialogGlobal.setExistingBodies(nullptr);
    dialogGlobal.lateInitialise();

    int status = dialogGlobal.exec();

    if (status == QDialog::Accepted)
    {
        if (this->m_movieFlag) { menuStopAVISave(); }
        if (this->m_simulation) delete this->m_simulation;
        this->m_simulation = nullptr;
        this->m_simulationWidget->setSimulation(this->m_simulation);
        this->m_stepCount = 0;
        this->ui->actionRun->setChecked(false);
        this->m_timer->stop();
        this->m_saveOBJFileSequenceFlag = false;
        this->m_simulation = new GaitSym::Simulation();
        std::unique_ptr<GaitSym::Global> newGlobal = dialogGlobal.outputGlobal();
        newGlobal->setSimulation(this->m_simulation);
        this->m_simulation->SetGlobal(std::move(newGlobal));
        this->m_simulationWidget->setSimulation(this->m_simulation);
        this->m_simulationWidget->update();
        this->ui->treeWidgetElements->setSimulation(this->m_simulation);
        std::unique_ptr<GaitSym::Marker> marker = std::make_unique<GaitSym::Marker>(nullptr);
        marker->setName("WorldMarker"s);
        auto markersMap = this->m_simulation->GetMarkerList();
        (*markersMap)[marker->name()] = std::move(marker);
        this->ui->treeWidgetElements->fillVisibitilityLists(this->m_simulation);
        this->updateComboBoxTrackingMarker();
        this->m_noName = true;
        this->setWindowModified(false);
        enterConstructionMode();
        this->updateEnable();
        this->setStatusString(tr("New document created"), 1);
    }
    else
    {
        this->setStatusString(tr("New document cancelled"), 1);
    }
}

void MainWindow::menuImportMeshes()
{
    if (this->m_simulation == nullptr) return;
    std::string lastFolder;
    if (this->m_noName) lastFolder = pystring::os::path::dirname(Preferences::valueQString("LastFileOpened").toStdString());
    else lastFolder = this->m_configFile.absolutePath().toStdString();

    QStringList meshFileNames = QFileDialog::getOpenFileNames(dynamic_cast<QWidget *>(parent()), tr("Select the geometry files required"), QString::fromStdString(lastFolder), "Meshes (*.obj *.ply)");
    std::vector<std::string> errorList;
    if (meshFileNames.size())
    {
        for (auto &&it : meshFileNames)
        {
            // first check that this is a valid mesh
            std::string meshFileName = it.toStdString();
            std::unique_ptr<FacetedObject> mesh = std::make_unique<FacetedObject>();
            if (mesh->ParseMeshFile(meshFileName))
            {
                errorList.push_back("Error parsing "s + meshFileName);
                continue;
            }

            // now create the body
            std::unique_ptr<GaitSym::Body> body = std::make_unique<GaitSym::Body>(/*this->m_simulation->GetWorldID()*/);
            body->setSimulation(this->m_simulation);
            body->SetConstructionDensity(Preferences::valueDouble("BodyDensity", 1000.0));
            body->SetGraphicFile1(meshFileName);

            // get a unique file name
            auto bodyList = this->m_simulation->GetBodyList();
            auto markerList = this->m_simulation->GetMarkerList();
            std::string ext, suggestedName;
            pystring::os::path::splitext(suggestedName, ext, pystring::os::path::basename(meshFileName));
            for (size_t i = 0; i < suggestedName.size(); i++)
            {
                if (std::isalnum(suggestedName[i]) || suggestedName[i] == '_') continue;
                suggestedName[i] = '_';
            }
            std::string suggestedCMMarkerName = suggestedName + "_CM_Marker"s;
            auto suggestedNameIt = bodyList->find(suggestedName);
            auto suggestedMarkerNameIt = markerList->find(suggestedCMMarkerName);
            size_t count = 0;
            while (suggestedNameIt != bodyList->end() || suggestedMarkerNameIt != markerList->end() || count > 999)
            {
                count++;
                std::string newSuggestedName = suggestedName + std::to_string(count);
                suggestedCMMarkerName = newSuggestedName + "_CM_Marker"s;
                suggestedNameIt = bodyList->find(newSuggestedName);
                suggestedMarkerNameIt = markerList->find(suggestedCMMarkerName);
                if (suggestedNameIt == bodyList->end() && suggestedMarkerNameIt == markerList->end())
                {
                    suggestedName = newSuggestedName;
                    break;
                }
            }
            if (count > 999)
            {
                errorList.push_back("Error setting name for "s + meshFileName);
                continue;
            }
            body->setName(suggestedName);

            // and set the mass
            double mass, ixx, iyy, izz, ixy, izx, iyz;
            pgd::Vector3 centreOfMass;
            pgd::Matrix3x3 inertialTensor;
            double density = body->GetConstructionDensity();
            bool clockwise = false;
            pgd::Vector3 translation;
            mesh->CalculateMassProperties(density, clockwise, translation, &mass, &centreOfMass, &inertialTensor);
            std::string massError/* = GaitSym::Body::MassCheck(&mass)*/; // FIX_ME
            if (massError.size() == 0)
            {
                body->SetConstructionPosition(centreOfMass[0], centreOfMass[1], centreOfMass[2]);
                body->SetPosition(centreOfMass[0], centreOfMass[1], centreOfMass[2]);
                // now recalculate the inertial tensor arount the centre of mass
                translation.Set(-centreOfMass[0], -centreOfMass[1], -centreOfMass[2]);
                mesh->CalculateMassProperties(density, clockwise, translation, &mass, &centreOfMass, &inertialTensor);
            }
            else
            {
                QMessageBox::warning(this, tr("Calculate Mass Properties: %1").arg(meshFileName.c_str()), tr("Calculated mass properties are invalid so using defaults:\n%1").arg(massError.c_str()));
                pgd::Vector3 boundingBoxCentre = (pgd::Vector3(mesh->upperBound()) + pgd::Vector3(mesh->lowerBound())) / 2;
                body->SetConstructionPosition(boundingBoxCentre.x, boundingBoxCentre.y, boundingBoxCentre.z);
                body->SetPosition(boundingBoxCentre.x, boundingBoxCentre.y, boundingBoxCentre.z);
                mass = 1;
                inertialTensor.SetInertia(1, 1, 1, 0, 0, 0);
            }
            inertialTensor.GetInertia(&ixx, &iyy, &izz, &ixy, &izx, &iyz);
            body->SetMass(mass, ixx, iyy, izz, ixy, izx, iyz);

            // set the default properties
            body->setSize1(Preferences::valueDouble("BodyAxesSize"));
            body->setSize2(Preferences::valueDouble("BodyBlendFraction"));
            body->setColour1(Preferences::valueQColor("BodyColour1").name(QColor::HexArgb).toStdString());
            body->setColour2(Preferences::valueQColor("BodyColour2").name(QColor::HexArgb).toStdString());
            body->setColour3(Preferences::valueQColor("BodyColour3").name(QColor::HexArgb).toStdString());

            // this is needed because there are some parts of GaitSym::Body that do not have a public interface
            body->saveToAttributes();
            body->createFromAttributes();

            // insert the new centre of mass marker
            std::unique_ptr<GaitSym::Marker> cmMarker = std::make_unique<GaitSym::Marker>(body.get());
            cmMarker->setName(suggestedCMMarkerName);
            cmMarker->setSimulation(this->m_simulation);
            cmMarker->setSize1(Preferences::valueDouble("MarkerSize", 0.01));

            // and add to the simulation
            (*bodyList)[suggestedName] = std::move(body);
            (*markerList)[suggestedCMMarkerName] = std::move(cmMarker);
        }
    }
    else
    {
        this->setStatusString("Import meshes as Bodies cancelled", 2);
        return;
    }
    this->setStatusString("Import Meshes as Bodies Successful", 1);
    for (size_t i = 0; i < errorList.size(); i++)
    {
        this->log(QString::fromStdString(errorList[i] + "\n"s));
    }
    this->setWindowModified(true);
    this->updateEnable();
    this->updateComboBoxTrackingMarker();
    this->ui->treeWidgetElements->fillVisibitilityLists(this->m_simulation);
    this->m_simulationWidget->update();
}

void MainWindow::menuImportOpenSimBodyKinematics()
{
    QFileInfo info(Preferences::valueQString("LastImportOpenSimBodyKinematics"));
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Import OpenSim BodyKinematics"), info.absoluteFilePath(), tr("STO File (*.sto);;Any File (*.* *)"), nullptr);
    if (fileName.isNull() == false)
    {
        Preferences::insert("LastImportOpenSimBodyKinematics", fileName);
        m_simulation->setKinematicsFile(fileName.toStdString());
        for (auto &&muscleIt : *m_simulation->GetMuscleList()) { muscleIt.second->setRedraw(true); }
        this->handleTracking();
        this->updateEnable();
        this->m_simulationWidget->update();
    }
}

void MainWindow::menuClearKinematics()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString("Kinematics currently loaded from \""s + m_simulation->kinematicsFile() + "\""s));
    msgBox.setInformativeText("Click OK to clear kinematics and revert to normal function, or Cancel to keep in playback mode");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if (msgBox.exec() == QMessageBox::Ok)
    {
        m_simulation->setKinematicsFile(std::string());
        menuRestart();
    }
}

void MainWindow::menuToggleFullScreen()
{
    // might want to remember things like whether the screen was maximised or minimised
    if (this->isFullScreen())
    {
        this->ui->menuBar->show();
        this->showNormal();
    }
    else
    {
        this->ui->menuBar->hide();
        this->showFullScreen();
    }
}

void MainWindow::menuCreateJoint()
{
    menuCreateEditJoint(nullptr);
}

void MainWindow::menuCreateEditJoint(GaitSym::Joint *joint)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateJoint", "this->m_simulation undefined");
    Q_ASSERT_X(this->m_simulation->GetBodyList()->size(), "MainWindow::menuCreateEditMarker", "No bodies defined");
    DialogJoints dialogJoints(this);
    dialogJoints.setSimulation(this->m_simulation);
    dialogJoints.setInputJoint(joint);
    dialogJoints.lateInitialise();
    int status = dialogJoints.exec();
    if (status == QDialog::Accepted)
    {
        if (!joint) // creating a new joint
        {
            std::unique_ptr<GaitSym::Joint> newJoint = dialogJoints.outputJoint();
            std::string newJointName = newJoint->name();
            newJoint->LateInitialisation();
            this->ui->treeWidgetElements->insertJoint(QString().fromStdString(newJointName), newJoint->visible(), newJoint->dump());
            (*this->m_simulation->GetJointList())[newJointName] = std::move(newJoint);
            this->setStatusString(QString("New joint created: %1").arg(QString::fromStdString(newJointName)), 1);
        }
        else // replacing an existing joint
        {
            std::unique_ptr<GaitSym::Joint> replacementJoint = dialogJoints.outputJoint();
            std::string replacementJointName = replacementJoint->name();
            replacementJoint->LateInitialisation();
            // the only thing that currently depends on joints is the ThreeJointDriver
            for (auto &&driverIt : *this->m_simulation->GetDriverList())
            {
                if (GaitSym::ThreeHingeJointDriver *threeHingeJointDriver = dynamic_cast<GaitSym::ThreeHingeJointDriver *>(driverIt.second.get())) threeHingeJointDriver->saveToAttributes();
            }
            (*this->m_simulation->GetJointList())[replacementJointName] = std::move(replacementJoint);
            for (auto driverIt = this->m_simulation->GetDriverList()->begin(); driverIt != this->m_simulation->GetDriverList()->end(); /* no increment */)
            {
                if (GaitSym::ThreeHingeJointDriver *threeHingeJointDriver = dynamic_cast<GaitSym::ThreeHingeJointDriver *>(driverIt->second.get()))
                {
                    std::string *lastError = threeHingeJointDriver->createFromAttributes();
                    if (lastError)
                    {
                        QMessageBox::warning(this, QString("Error creating ThreeHingeJointDriver \"%1\"").arg(QString::fromStdString(driverIt->first)),
                                             QString("Error message:\n\"%1\"").arg(QString::fromStdString(*lastError)));
                        this->setStatusString(QString("ThreeHingeJointDriver deleted: %1").arg(QString::fromStdString(driverIt->first)), 1);
                        this->deleteExistingDriver(QString::fromStdString(driverIt->first));
                        driverIt = this->m_simulation->GetDriverList()->erase(driverIt);
                    }
                    else { driverIt++; }
                }
                else { driverIt++; }
            }
            this->setStatusString(QString("Joint edited: %1").arg(QString::fromStdString(replacementJointName)), 1);
        }
        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Joint creation cancelled"), 2);
    }
}

void MainWindow::menuCreateBody()
{
    menuCreateEditBody(nullptr);
}

void MainWindow::menuCreateEditBody(GaitSym::Body *body)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateBody", "m_simulation undefined");
    DialogBodyBuilder dialogBodyBuilder(this);
    dialogBodyBuilder.setSimulation(this->m_simulation);
    dialogBodyBuilder.setInputBody(body);
    dialogBodyBuilder.lateInitialise();
    pgd::Vector3 originalContructionPosition;
    pgd::Vector3 originalPosition;
    pgd::Quaternion originalOrientation;
    if (body)
    {
        originalContructionPosition = body->GetConstructionPosition();
        originalPosition = body->GetPosition();
        originalOrientation = body->GetQuaternion();
    }
    int status = dialogBodyBuilder.exec();
    if (status == QDialog::Accepted)
    {
        if (!body) // this is the create body option so there will be no dependencies
        {
            std::unique_ptr<GaitSym::Body> newBody = dialogBodyBuilder.outputBody();
            newBody->LateInitialisation();
            std::string newBodyName = newBody->name();
            // insert the new centre of mass marker unless it already exists
            std::string cmMarkerName = newBodyName + "_CM_Marker"s;
            if (!this->m_simulation->GetMarker(cmMarkerName))
            {
                std::unique_ptr<GaitSym::Marker> cmMarker = std::make_unique<GaitSym::Marker>(newBody.get());
                cmMarker->setName(cmMarkerName);
                cmMarker->setSimulation(this->m_simulation);
                cmMarker->setSize1(Preferences::valueDouble("MarkerSize", 0.01));
                this->ui->treeWidgetElements->insertMarker(QString().fromStdString(cmMarkerName), cmMarker->visible(), cmMarker->dump());
                (*this->m_simulation->GetMarkerList())[cmMarkerName] = std::move(cmMarker);
                this->setStatusString(QString("New marker created: %1").arg(QString::fromStdString(cmMarkerName)), 1);
            }
            else
            {
                this->setStatusString(QString("Unable to create: %1. Marker already exists.").arg(QString::fromStdString(cmMarkerName)), 1);
            }
            // insert the new body
            this->ui->treeWidgetElements->insertBody(QString().fromStdString(newBodyName), newBody->visible(), newBody->dump());
            (*this->m_simulation->GetBodyList())[newBodyName] = std::move(newBody);
            this->setStatusString(QString("New body created: %1").arg(QString::fromStdString(newBodyName)), 0);
            this->updateComboBoxTrackingMarker();
        }
        else // this is an edit so things may have moved and we need to deal with that
        {
            this->setStatusString(QString("Body edited: %1").arg(QString::fromStdString(body->name())), 1);
            pgd::Vector3 deltaPosition = pgd::Vector3(body->GetConstructionPosition()) - originalContructionPosition;
            if (Preferences::valueBool("DialogBodyBuilderMoveMarkers", false) == false) // need to compensate the move in construction position
            {
                for (auto &&it : *this->m_simulation->GetMarkerList())
                {
                    if (it.second->GetBody() == body)
                        it.second->OffsetPosition(-deltaPosition.x, -deltaPosition.y, -deltaPosition.z);
                }
            }
            // and handle the CM marker if it exists
            std::string cmMarkerName = body->name() + "_CM_Marker"s;
            GaitSym::Marker *cmMarker = this->m_simulation->GetMarker(cmMarkerName);
            if (cmMarker && cmMarker->GetBody()->name() == body->name())
            {
                cmMarker->SetPosition(0, 0, 0); // this puts it back at the centre of mass
            }
            else
            {
                if (cmMarker) this->setStatusString(QString("Unable to move: %1. Marker attached to a different body.").arg(QString::fromStdString(cmMarkerName)), 0);
                else this->setStatusString(QString("Unable to move: %1. Marker does not exists.").arg(QString::fromStdString(cmMarkerName)), 0);
            }

            body->setRedraw(true);
            std::vector<GaitSym::NamedObject *> objectList = this->m_simulation->GetObjectList();
            for (auto &&it : objectList)
            {

                if (it->isUpstreamObject(body))
                {
                    it->saveToAttributes();
                    it->createFromAttributes();
                    it->setRedraw(true);
                    // everything needs a redraw but some things also need extra work
                    if (dynamic_cast<GaitSym::Strap *>(it)) dynamic_cast<GaitSym::Strap *>(it)->Calculate();
                }
            }
        }
        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Body creation cancelled"), 2);
    }
}

void MainWindow::menuCreateMarker()
{
    menuCreateEditMarker(nullptr);
}

void MainWindow::menuCreateEditMarker(GaitSym::Marker *marker)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateEditMarker", "m_simulation undefined");
    Q_ASSERT_X(this->m_simulation->GetBodyList()->size(), "MainWindow::menuCreateEditMarker", "No bodies defined");
    DialogMarkers dialogMarkers(this);
    dialogMarkers.setCursor3DPosition(this->m_simulationWidget->cursor3DPosition());
    dialogMarkers.setInputMarker(marker);
    dialogMarkers.setSimulation(this->m_simulation);
    dialogMarkers.lateInitialise();
    if (sender() == this->m_simulationWidget && this->m_simulationWidget->getLastMenuItem() != tr("Edit Marker..."))
    {
        auto closestHit = this->m_simulationWidget->getClosestHit();
        if (closestHit)
        {
            pgd::Vector3 location = closestHit->worldLocation();
            dialogMarkers.overrideStartPosition(location);
            if (closestHit->drawable()) dialogMarkers.overrideStartBody(closestHit->drawable()->name());
        }
    }
    int status = dialogMarkers.exec();
    if (status == QDialog::Accepted)
    {
        if (!marker) // create so no dependencies
        {
            std::unique_ptr<GaitSym::Marker> newMarker = dialogMarkers.outputMarker();
            std::string newMarkerName = newMarker->name();
            this->ui->treeWidgetElements->insertMarker(QString().fromStdString(newMarkerName), newMarker->visible(), newMarker->dump());
            (*this->m_simulation->GetMarkerList())[newMarkerName] = std::move(newMarker);
            this->setStatusString(QString("New marker created: %1").arg(QString::fromStdString(newMarkerName)), 1);
        }
        else // editing a marker so need to cope with dependencies
        {
            marker->setRedraw(true);
            std::vector<GaitSym::NamedObject *> objectList = this->m_simulation->GetObjectList();
            for (auto &&it : objectList)
            {
                if (it->isUpstreamObject(marker))
                {
                    it->saveToAttributes();
                    it->createFromAttributes();
                    it->setRedraw(true);
                    // everything needs a redraw but somethings also need extra work
                    if (dynamic_cast<GaitSym::Strap *>(it)) dynamic_cast<GaitSym::Strap *>(it)->Calculate();
                }
            }
            this->setStatusString(QString("Marker edited: %1").arg(QString::fromStdString(marker->name())), 1);
        }

        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Marker creation cancelled"), 2);
    }
}

void MainWindow::menuCreateMuscle()
{
    menuCreateEditMuscle(nullptr);
}

void MainWindow::menuCreateEditMuscle(GaitSym::Muscle *muscle)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateMuscle", "this->m_simulation undefined");
    Q_ASSERT_X(this->m_simulation->GetBodyList()->size(), "MainWindow::menuCreateEditMarker", "No bodies defined");
    DialogMuscles dialogMuscles(this);
    dialogMuscles.setSimulation(this->m_simulation);
    dialogMuscles.setInputMuscle(muscle);
    dialogMuscles.lateInitialise();
    int status = dialogMuscles.exec();
    if (status == QDialog::Accepted)
    {
        if (!muscle) // creating a new muscle
        {
            std::unique_ptr<GaitSym::Strap> newStrap = dialogMuscles.outputStrap();
            std::string newStrapName = newStrap->name();
            (*this->m_simulation->GetStrapList())[newStrapName] = std::move(newStrap);
            std::unique_ptr<GaitSym::Muscle> newMuscle = dialogMuscles.outputMuscle();
            muscle = newMuscle.get();
            std::string newMuscleName = newMuscle->name();
            this->ui->treeWidgetElements->insertMuscle(QString().fromStdString(newMuscleName), newMuscle->visible(), newMuscle->dump());
            (*this->m_simulation->GetMuscleList())[newMuscleName] = std::move(newMuscle);
            this->setStatusString(QString("New muscle created: %1").arg(QString::fromStdString(newMuscleName)), 1);
        }
        else // replacing an existing muscle
        {
            std::unique_ptr<GaitSym::Strap> replacementStrap = dialogMuscles.outputStrap();
            std::string replacementStrapName = replacementStrap->name();
            (*this->m_simulation->GetStrapList())[replacementStrapName] = std::move(replacementStrap);
            std::unique_ptr<GaitSym::Muscle> replacementMuscle = dialogMuscles.outputMuscle();
            muscle = replacementMuscle.get();
            std::string replacementMuscleName = replacementMuscle->name();
            //            this->ui->treeWidgetElements->insertMuscle(QString().fromStdString(replacementMuscleName), replacementMuscle->visible(), replacementMuscle->dump());
            (*this->m_simulation->GetMuscleList())[replacementMuscleName] = std::move(replacementMuscle);
            this->setStatusString(QString("Muscle edited: %1").arg(QString::fromStdString(replacementMuscleName)), 1);
        }

        this->setWindowModified(true);
        GaitSym::Muscle::StrapColourControl colourControl = GaitSym::Muscle::fixedColour;
        QString text = this->ui->comboBoxMuscleColourMap->currentText();
        if (text == "Strap Colour") colourControl = GaitSym::Muscle::fixedColour;
        else if (text == "Activation Colour") colourControl = GaitSym::Muscle::activationMap;
        else if (text == "Strain Colour") colourControl = GaitSym::Muscle::strainMap;
        else if (text == "Force Colour") colourControl = GaitSym::Muscle::forceMap;
        muscle->setStrapColourControl(colourControl);
        muscle->LateInitialisation();
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Muscle creation cancelled"), 2);
    }
}

void MainWindow::menuCreateGeom()
{
    menuCreateEditGeom(nullptr);
}

void MainWindow::menuCreateEditGeom(GaitSym::Geom *geom)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateGeom", "this->m_simulation undefined");
    Q_ASSERT_X(this->m_simulation->GetBodyList()->size(), "MainWindow::menuCreateEditMarker", "No bodies defined");
    DialogGeoms dialogGeoms(this);
    dialogGeoms.setSimulation(this->m_simulation);
    dialogGeoms.setInputGeom(geom);
    dialogGeoms.lateInitialise();
    int status = dialogGeoms.exec();
    if (status == QDialog::Accepted)
    {
        if (!geom) // creating a new geom
        {
            std::unique_ptr<GaitSym::Geom> newGeom = dialogGeoms.outputGeom();
            std::string newGeomName = newGeom->name();
            this->ui->treeWidgetElements->insertGeom(QString().fromStdString(newGeomName), newGeom->visible(), newGeom->dump());
            (*this->m_simulation->GetGeomList())[newGeomName] = std::move(newGeom);
            this->setStatusString(QString("New geom created: %1").arg(QString::fromStdString(newGeomName)), 1);
        }
        else // replacing an existing geom
        {
            std::unique_ptr<GaitSym::Geom> replacementGeom = dialogGeoms.outputGeom();
            std::string replacementGeomName = replacementGeom->name();
            (*this->m_simulation->GetGeomList())[replacementGeomName] = std::move(replacementGeom);
            // handle dependencies
            std::vector<GaitSym::NamedObject *> objectList = this->m_simulation->GetObjectList();
            for (auto &&it : objectList)
            {
                if (it->isUpstreamObject(geom))  // have to look for the old object because that's what needs to be replaced
                {
                    it->saveToAttributes();
                    it->createFromAttributes();
                    it->setRedraw(true);
                    // everything needs a redraw but somethings also need extra work
                    if (dynamic_cast<GaitSym::Strap *>(it)) dynamic_cast<GaitSym::Strap *>(it)->Calculate();
                }
            }

            this->setStatusString(QString("Geom edited: %1").arg(QString::fromStdString(replacementGeomName)), 1);
        }
        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Geom creation cancelled"), 2);
    }
}



void MainWindow::menuCreateDriver()
{
    menuCreateEditDriver(nullptr);
}

void MainWindow::menuCreateEditDriver(GaitSym::Driver *driver)
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateDriver", "this->m_simulation undefined");
    Q_ASSERT_X(this->m_simulation->GetBodyList()->size(), "MainWindow::menuCreateEditMarker", "No bodies defined");
    if (dynamic_cast<GaitSym::TegotaeDriver *>(driver) || dynamic_cast<GaitSym::ThreeHingeJointDriver *>(driver) || dynamic_cast<GaitSym::TwoHingeJointDriver *>(driver)
        || dynamic_cast<GaitSym::MarkerPositionDriver *>(driver) || dynamic_cast<GaitSym::MarkerEllipseDriver *>(driver))
    {
        QMessageBox::warning(this, "GUI Based Editing Not Implemented", QString("Driver %1 could not be edited").arg(QString::fromStdString(driver->name())));
        return;
    }
    DialogDrivers dialogDrivers(this);
    dialogDrivers.setSimulation(this->m_simulation);
    dialogDrivers.setInputDriver(driver);
    dialogDrivers.lateInitialise();
    int status = dialogDrivers.exec();
    if (status == QDialog::Accepted)
    {
        if (!driver) // creating a new driver
        {
            std::unique_ptr<GaitSym::Driver> newDriver = dialogDrivers.outputDriver();
            std::string newDriverName = newDriver->name();
            this->ui->treeWidgetElements->insertDriver(QString().fromStdString(newDriverName), newDriver->visible(), newDriver->dump());
            (*this->m_simulation->GetDriverList())[newDriverName] = std::move(newDriver);
            this->setStatusString(QString("New driver created: %1").arg(QString::fromStdString(newDriverName)), 1);
        }
        else // replacing an existing driver
        {
            std::unique_ptr<GaitSym::Driver> replacementDriver = dialogDrivers.outputDriver();
            std::string replacementDriverName = replacementDriver->name();
            (*this->m_simulation->GetDriverList())[replacementDriverName] = std::move(replacementDriver);
            this->setStatusString(QString("Driver edited: %1").arg(QString::fromStdString(replacementDriverName)), 1);
        }
        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Driver creation cancelled"), 2);
    }
}

void MainWindow::menuEditGlobal()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuEditGlobal", "this->m_simulation undefined");
    DialogGlobal dialogGlobal(this);
    dialogGlobal.setInputGlobal(this->m_simulation->GetGlobal());
    dialogGlobal.setExistingBodies(this->m_simulation->GetBodyList());
    dialogGlobal.lateInitialise();

    int status = dialogGlobal.exec();

    if (status == QDialog::Accepted)   // write the new settings
    {
        this->m_simulation->SetGlobal(dialogGlobal.outputGlobal());
        this->m_simulation->GetGlobal()->setSimulation(this->m_simulation);
        this->setStatusString(tr("Global values edited"), 1);
        this->setWindowModified(true);
        this->updateEnable();
        this->ui->doubleSpinBoxTimeMax->setValue(this->m_simulation->GetGlobal()->TimeLimit());
        this->m_simulationWidget->setAxesScale(float(this->m_simulation->GetGlobal()->size1()));
        this->m_simulationWidget->setBackgroundColour(QString::fromStdString(this->m_simulation->GetGlobal()->colour1().GetHexARGB()));
        this->m_simulationWidget->update();
    }
    else
    {
        this->setStatusString(tr("Global values unchanged"), 2);
    }
}

void MainWindow::enterRunMode()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::enterRunMode", "this->m_simulation undefined");
    this->m_mode = MainWindow::runMode;
    for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterRunMode();
    for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
    for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
    for (auto &&it : *this->m_simulation->GetJointList()) it.second->LateInitialisation();
    this->ui->actionRunMode->setChecked(true);
    this->ui->actionConstructionMode->setChecked(false);
    this->updateEnable();
    this->m_simulationWidget->getDrawMuscleMap()->clear(); // force a redraw of all muscles
    this->m_simulationWidget->getDrawFluidSacMap()->clear(); // force a redraw of all fluid sacs
    this->m_simulationWidget->update();
}

void MainWindow::enterConstructionMode()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::enterConstructionMode", "this->m_simulation undefined");
    Q_ASSERT_X(this->m_stepCount == 0, "MainWindow::enterConstructionMode", "this->m_stepCount not zero");
    this->m_mode = MainWindow::constructionMode;
    for (auto &&it : *this->m_simulation->GetBodyList()) it.second->EnterConstructionMode();
    for (auto &&it : *this->m_simulation->GetMuscleList()) it.second->LateInitialisation();
    for (auto &&it : *this->m_simulation->GetFluidSacList()) it.second->LateInitialisation();
    for (auto &&it : *this->m_simulation->GetJointList()) it.second->LateInitialisation();
    this->ui->actionRunMode->setChecked(false);
    this->ui->actionConstructionMode->setChecked(true);
    this->updateEnable();
    this->m_simulationWidget->getDrawMuscleMap()->clear(); // force a redraw of all muscles
    this->m_simulationWidget->getDrawFluidSacMap()->clear(); // force a redraw of all fluid sacs
    this->m_simulationWidget->update();
}

void MainWindow::menuCreateAssembly()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuCreateAssembly", "this->m_simulation undefined");
    DialogAssembly dialogAssembly(this);
    dialogAssembly.setSimulation(this->m_simulation);
    dialogAssembly.initialise();
    connect(&dialogAssembly, SIGNAL(jointCreated(const QString &)), this->ui->treeWidgetElements, SLOT(insertJoint(const QString &)));
    connect(&dialogAssembly, SIGNAL(markerCreated(const QString &)), this->ui->treeWidgetElements, SLOT(insertMarker(const QString &)));
    connect(&dialogAssembly, SIGNAL(jointDeleted(const QString &)), this->ui->treeWidgetElements, SLOT(deleteJoint(const QString &)));
    connect(&dialogAssembly, SIGNAL(markerDeleted(const QString &)), this->ui->treeWidgetElements, SLOT(deleteMarker(const QString &)));
    int status = dialogAssembly.exec();
    if (status == QDialog::Accepted)
    {
        this->setStatusString(tr("Assembly constraints created"), 2);
        this->setWindowModified(true);
    }
    else
    {
        this->setStatusString(tr("Assembly cancelled"), 2);
    }
    this->updateEnable();
    this->m_simulationWidget->update();
}

void MainWindow::menuDeleteAssembly()
{
    int ret = QMessageBox::warning(this, tr("Delete Assembly"), tr("This action cannot be undone.\nAre you sure you want to continue?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
    {
        for (auto iter = this->m_simulation->GetJointList()->begin(); iter != this->m_simulation->GetJointList()->end(); /* no increment */)
        {
            if (iter->second->group() == "assembly"s)
            {
                QString name = QString::fromStdString(iter->first);
                iter++;
                this->deleteExistingJoint(name, true);
            }
            else
            {
                iter++;
            }
        }
        for (auto iter = this->m_simulation->GetMarkerList()->begin(); iter != this->m_simulation->GetMarkerList()->end(); /* no increment */)
        {
            if (iter->second->group() == "assembly"s)
            {
                QString name = QString::fromStdString(iter->first);
                iter++;
                this->deleteExistingMarker(name, true);
            }
            else
            {
                iter++;
            }
        }
        this->setStatusString(tr("Assembly constraints deleted"), 2);
        this->setWindowModified(true);
        this->updateEnable();
        this->m_simulationWidget->update();
    }
}

void MainWindow::menuExportMarkers()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuExportMarkers", "this->m_simulation undefined");
    DialogMarkerExport dialogMarkerExport(this);
    dialogMarkerExport.setSimulation(this->m_simulation);
    int status = dialogMarkerExport.exec();
    if (status == QDialog::Accepted)
    {
        setStatusString(QString("Marker export returned"), 1);
    }
    else
    {
        setStatusString(QString("Marker export cancelled"), 1);
    }
}

void MainWindow::menuImportMarkers()
{
    Q_ASSERT_X(this->m_simulation, "MainWindow::menuImportMarkers", "this->m_simulation undefined");
    DialogMarkerImport dialogMarkerImport(this);
    dialogMarkerImport.setSimulation(this->m_simulation);
    std::vector<std::unique_ptr<GaitSym::Marker>> markerList;
    dialogMarkerImport.setMarkerList(&markerList);
    int status = dialogMarkerImport.exec();
    if (status == QDialog::Accepted)
    {
        if (markerList.size())
        {
            std::vector<GaitSym::NamedObject *> objectList = this->m_simulation->GetObjectList();
            for (size_t i =0; i < markerList.size(); i++)
            {
                std::unique_ptr<GaitSym::Marker> marker = std::move(markerList[i]);
                std::string markerName = marker->name();
                this->ui->treeWidgetElements->insertMarker(QString().fromStdString(markerName), marker->visible(), marker->dump());
                auto markerIt = this->m_simulation->GetMarkerList()->find(markerName);
                if (markerIt != this->m_simulation->GetMarkerList()->end()) // replacement marker
                {
                    markerIt->second->setRedraw(true);
                    for (auto &&it : objectList)
                    {
                        if (it->isUpstreamObject(markerIt->second.get()))
                        {
                            it->saveToAttributes();
                            it->createFromAttributes();
                            it->setRedraw(true);
                            // everything needs a redraw but somethings also need extra work
                            if (dynamic_cast<GaitSym::Strap *>(it)) dynamic_cast<GaitSym::Strap *>(it)->Calculate();
                        }
                    }
                }
                (*this->m_simulation->GetMarkerList())[markerName] = std::move(marker);
            }
            this->setWindowModified(true);
            this->updateComboBoxTrackingMarker();
            this->updateEnable();
            this->m_simulationWidget->update();
        }
    }
    else
    {
        setStatusString(QString("Marker import cancelled"), 1);
    }
}

void MainWindow::menuResetView()
{
    this->ui->doubleSpinBoxTrackingOffset->setValue(Preferences::valueDouble("ResetTrackingOffset"));

    this->m_simulationWidget->setCameraDistance(float(Preferences::valueDouble("ResetCameraDistance")));
    this->m_simulationWidget->setFOV(float(Preferences::valueDouble("ResetCameraFoV")));
    this->m_simulationWidget->setCameraVecX(float(Preferences::valueDouble("ResetCameraVecX")));
    this->m_simulationWidget->setCameraVecY(float(Preferences::valueDouble("ResetCameraVecY")));
    this->m_simulationWidget->setCameraVecZ(float(Preferences::valueDouble("ResetCameraVecZ")));
    this->m_simulationWidget->setCOIx(float(Preferences::valueDouble("ResetCameraCOIX")));
    this->m_simulationWidget->setCOIy(float(Preferences::valueDouble("ResetCameraCOIY")));
    this->m_simulationWidget->setCOIz(float(Preferences::valueDouble("ResetCameraCOIZ")));
    this->m_simulationWidget->setUpX(float(Preferences::valueDouble("ResetCameraUpX")));
    this->m_simulationWidget->setUpY(float(Preferences::valueDouble("ResetCameraUpY")));
    this->m_simulationWidget->setUpZ(float(Preferences::valueDouble("ResetCameraUpZ")));
    this->m_simulationWidget->setBackClip(float(Preferences::valueDouble("ResetCameraBackClip")));
    this->m_simulationWidget->setFrontClip(float(Preferences::valueDouble("ResetCameraFrontClip")));

    this->m_simulationWidget->setCursor3DPosition(QVector3D(0, 0, 0));
    this->m_simulationWidget->setCursorRadius(float(Preferences::valueDouble("ResetCursorRadius")));
    this->m_simulationWidget->setAxesScale(float(Preferences::valueDouble("ResetAxesSize")));

    this->ui->doubleSpinBoxDistance->setValue(Preferences::valueDouble("ResetCameraDistance"));
    this->ui->doubleSpinBoxFoV->setValue(Preferences::valueDouble("ResetCameraFoV"));
    this->ui->doubleSpinBoxCOIX->setValue(Preferences::valueDouble("ResetCameraCOIX"));
    this->ui->doubleSpinBoxCOIY->setValue(Preferences::valueDouble("ResetCameraCOIY"));
    this->ui->doubleSpinBoxCOIZ->setValue(Preferences::valueDouble("ResetCameraCOIZ"));
    this->ui->doubleSpinBoxFar->setValue(Preferences::valueDouble("ResetCameraBackClip"));
    this->ui->doubleSpinBoxNear->setValue(Preferences::valueDouble("ResetCameraFrontClip"));
    this->ui->doubleSpinBoxTrackingOffset->setValue(Preferences::valueDouble("ResetTrackingOffset"));

    this->ui->radioButtonTrackingNone->setChecked(true);

    this->ui->doubleSpinBoxCursorSize->setValue(Preferences::valueDouble("ResetCursorRadius"));

    this->m_simulationWidget->update();
}

void MainWindow::menuRawXMLEditor()
{
    DialogRawXMLEdit dialogRawXMLEdit(this);
    dialogRawXMLEdit.useXMLSyntaxHighlighter();
    dialogRawXMLEdit.setEditorText(QString::fromStdString(this->m_simulation->SaveToXML()));
    dialogRawXMLEdit.setModified(false);
    int status = dialogRawXMLEdit.exec();
    if (status == QDialog::Accepted) // write the new settings
    {
        if (dialogRawXMLEdit.isModified())
        {
            QByteArray editFileData = dialogRawXMLEdit.editorText().toUtf8();
            menuOpen(this->m_configFile.absoluteFilePath(), &editFileData);
            this->setWindowModified(true);
            enterConstructionMode();
            this->updateEnable();
            this->m_simulationWidget->update();
        }
        else
        {
            this->ui->statusBar->showMessage(tr("Raw XML Editor no changes made"));
        }
    }
    else
    {
        this->ui->statusBar->showMessage(tr("Raw XML Editor cancelled"));
    }
}

void MainWindow::menuCreateMirrorElements()
{
    DialogCreateMirrorElements dialog(this);
    dialog.useXMLSyntaxHighlighter();
    dialog.setEditorText(QString::fromStdString(this->m_simulation->SaveToXML()));
    dialog.setModified(false);
    int status = dialog.exec();
    if (status == QDialog::Accepted) // write the new settings
    {
        if (dialog.isModified())
        {
            QByteArray editFileData = dialog.editorText().toUtf8();
            menuOpen(this->m_configFile.absoluteFilePath(), &editFileData);
            this->setWindowModified(true);
            enterConstructionMode();
            this->updateEnable();
            this->m_simulationWidget->update();
        }
        else
        {
            this->ui->statusBar->showMessage(tr("Create Mirror Elements no changes made"));
        }
    }
    else
    {
        this->ui->statusBar->showMessage(tr("Create Mirror Elements cancelled"));
    }
}

void MainWindow::menuCreateTestingDrivers()
{
    DialogCreateTestingDrivers dialog(this);
    dialog.useXMLSyntaxHighlighter();
    dialog.setEditorText(QString::fromStdString(this->m_simulation->SaveToXML()));
    dialog.setModified(false);
    int status = dialog.exec();
    if (status == QDialog::Accepted) // write the new settings
    {
        if (dialog.isModified())
        {
            QByteArray editFileData = dialog.editorText().toUtf8();
            menuOpen(this->m_configFile.absoluteFilePath(), &editFileData);
            this->setWindowModified(true);
            enterConstructionMode();
            this->updateEnable();
            this->m_simulationWidget->update();
        }
        else
        {
            this->ui->statusBar->showMessage(tr("Create Mirror Elements no changes made"));
        }
    }
    else
    {
        this->ui->statusBar->showMessage(tr("Create Mirror Elements cancelled"));
    }
}

void MainWindow::menuCreateStringOfPearls()
{
    DialogStringOfPearlsBuilder dialog(this);
    dialog.setSimulation(this->m_simulation);
    dialog.lateInitialise();
    int status = dialog.exec();
    if (status == QDialog::Accepted)
    {
        this->ui->statusBar->showMessage(tr("Create String of Pearls finished"));
    }
    else
    {
        this->ui->statusBar->showMessage(tr("Create String of Pearls cancelled"));
    }
}

void MainWindow::menuRename()
{
    DialogRename dialog(this);
    dialog.useXMLSyntaxHighlighter();
    dialog.setEditorText(QString::fromStdString(this->m_simulation->SaveToXML()));
    auto objectList = this->m_simulation->GetObjectList();
    dialog.setNameList(&objectList);
    dialog.setModified(false);
    int status = dialog.exec();
    if (status == QDialog::Accepted) // write the new settings
    {
        if (dialog.isModified())
        {
            QByteArray editFileData = dialog.editorText().toUtf8();
            menuOpen(this->m_configFile.absoluteFilePath(), &editFileData);
            this->setWindowModified(true);
            enterConstructionMode();
            this->updateEnable();
            this->m_simulationWidget->update();
        }
        else
        {
            this->ui->statusBar->showMessage(tr("Rename: no changes made"));
        }
    }
    else
    {
        this->ui->statusBar->showMessage(tr("Rename cancelled"));
    }
}

void MainWindow::elementInfo(const QString &elementType, const QString &elementName)
{
    DialogInfo *dialog = new DialogInfo(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true); // needed so I can display this modelessly
    dialog->useXMLSyntaxHighlighter();
    GaitSym::NamedObject *element = this->m_simulation->GetNamedObject(elementName.toStdString());
    if (!element) return;
    element->saveToAttributes();
    std::vector<std::string> lines;
    lines.push_back("<"s + elementType.toUpper().toStdString());
    for (auto &&it : element->attributeMap()) lines.push_back("    "s + it.first + "=\"" + it.second + "\"");
    lines.push_back("/>"s);
    if (GaitSym::Muscle *muscle = dynamic_cast<GaitSym::Muscle *>(element))
    {
        muscle->GetStrap()->saveToAttributes();
        lines.push_back("<STRAP"s);
        for (auto &&it : muscle->GetStrap()->attributeMap()) lines.push_back("    "s + it.first + "=\"" + it.second + "\"");
        lines.push_back("/>"s);
    }
    std::string text = pystring::join("\n"s, lines);
    dialog->setEditorText(QString::fromStdString(text));
    dialog->setWindowTitle(QString("%1 ID=\"%2\" Information").arg(elementType, elementName));
    dialog->setModal(false);
    dialog->show();
}

void MainWindow::elementHide(const QString &elementType, const QString &elementName)
{
    this->ui->treeWidgetElements->setVisibleSwitch(elementType.toUpper(), elementName, false);
}

void MainWindow::menuClearMeshCache()
{
    FacetedObject::ClearMeshStore();
    this->log("Mesh cache cleared");
    this->ui->statusBar->showMessage("Mesh cache cleared");
}

void MainWindow::menuConvertFile()
{
    DialogConvertFile dialogConvertFile(this);
    dialogConvertFile.exec();
}

void MainWindow::copy()
{
    QWidget *focused = QApplication::focusWidget();
    if ( focused != nullptr )
    {
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier ));
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier ));
    }
}

void MainWindow::cut()
{
    QWidget *focused = QApplication::focusWidget();
    if ( focused != nullptr )
    {
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier ));
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyRelease, Qt::Key_X, Qt::ControlModifier ));
    }
}

void MainWindow::paste()
{
    QWidget *focused = QApplication::focusWidget();
    if ( focused != nullptr )
    {
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier ));
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier ));
    }
}

void MainWindow::selectAll()
{
    QWidget *focused = QApplication::focusWidget();
    if ( focused != nullptr )
    {
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier ));
        QApplication::postEvent( focused, new QKeyEvent( QEvent::KeyRelease, Qt::Key_A, Qt::ControlModifier ));
    }
}

void MainWindow::updateRecentFiles(const QString &recentFile)
{
    // remove any existing reference to recentFile
    for (int i = m_recentFileList.size() - 1; i >= 0; i--)
    {
        if (m_recentFileList[i] == recentFile) { m_recentFileList.removeAt(i); }
    }
    // add recentFile to the front of the list
    m_recentFileList.push_front(recentFile);
    // remove any non-existent files
    for (int i = m_recentFileList.size() - 1; i >= 0; i--)
    {
        QFileInfo checkFile(m_recentFileList[i]);
        if (checkFile.exists() && checkFile.isFile()) continue;
        m_recentFileList.removeAt(i);
    }
    // remove the last elements of the list if necessary
    for (int i = m_recentFileList.size(); i >= m_maxRecentFiles; i--) { m_recentFileList.pop_back(); }
    // delete the old recent file menu and create a new one
    QList<QMenu *>  menuList = this->menuBar()->findChildren<QMenu *>();
    for (auto && menu: menuList)
    {
        if (menu->title() == "Open Recent")
        {
            disconnect(menu, SIGNAL(triggered(QAction *)), nullptr, nullptr);
            menu->clear();
            for (auto &&file : m_recentFileList) { menu->addAction(file); }
            menu->addSeparator();
            menu->addAction("Clear Menu");
            connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(menuOpenRecent(QAction *)));
            break;
        }
    }
    Preferences::insert("RecentFileList", m_recentFileList);
}

void MainWindow::menuOpenRecent(QAction *action)
{
    if (action->text() == "Clear Menu")
    {
        m_recentFileList.clear();
        updateRecentFiles("");
        return;
    }
    menuOpen(action->text(), nullptr);
}

void MainWindow::layoutSpacing(QWidget *container)
{
    QList<QLayout *> list = container->findChildren<QLayout *>(Qt::FindChildrenRecursively);
    int left = 4, top = 4, right = 4, bottom = 4;
    int spacing = 4;
    for (auto &&item : list)
    {
        QMargins margins = item->contentsMargins();
        margins.setLeft(std::min(left, margins.left()));
        margins.setRight(std::min(right, margins.right()));
        margins.setTop(std::min(top, margins.top()));
        margins.setBottom(std::min(bottom, margins.bottom()));
        item->setContentsMargins(margins);
        if (auto w = dynamic_cast<QGridLayout *>(item))
        {
            int hSpacing = w->horizontalSpacing();
            int vSpacing = w->verticalSpacing();
            w->setHorizontalSpacing(std::min(spacing, hSpacing));
            w->setVerticalSpacing(std::min(spacing, vSpacing));
        }
        else
        {
            item->setSpacing(std::min(item->spacing(), spacing));
        }
    }
}
