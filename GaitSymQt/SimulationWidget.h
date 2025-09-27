#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include "DrawDataTarget.h"
#include "IntersectionHits.h"
#include "DrawBody.h"
#include "DrawJoint.h"
#include "DrawMuscle.h"
#include "DrawMarker.h"
#include "DrawFluidSac.h"
#include "DrawGeom.h"
#include "FacetedSphere.h"
#include "AVIWriter.h"
#include "TrackBall.h"

#include "threepp/threepp.hpp"

#include <QOpenGLWindow>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QMouseEvent>

#include <memory>
#include <map>

namespace GaitSym { class Simulation; }
class MainWindow;

class SimulationWidget : public QOpenGLWindow
{
    Q_OBJECT

public:
    SimulationWidget();

    GaitSym::Simulation *simulation() const;
    void setSimulation(GaitSym::Simulation *simulation);

    bool wireframe() const;
    void setWireframe(bool wireframe);

    bool boundingBox() const;
    void setBoundingBox(bool boundingBox);

    bool orthographicProjection() const;
    void setOrthographicProjection(bool orthographicProjection);

    float cameraDistance() const;
    void setCameraDistance(float cameraDistance);

    float fieldOfView() const;
    void setFieldOfView(float fieldOfView);

    float cameraVecX() const;
    void setCameraVecX(float cameraVecX);

    float cameraVecY() const;
    void setCameraVecY(float cameraVecY);

    float cameraVecZ() const;
    void setCameraVecZ(float cameraVecZ);

    float centreOfInterestX() const;
    void setCentreOfInterestX(float centreOfInterestX);

    float centreOfInterestY() const;
    void setCentreOfInterestY(float centreOfInterestY);

    float centreOfInterestZ() const;
    void setCentreOfInterestZ(float centreOfInterestZ);

    float frontClip() const;
    void setFrontClip(float frontClip);

    float backClip() const;
    void setBackClip(float backClip);

    float upX() const;
    void setUpX(float upX);

    float upY() const;
    void setUpY(float upY);

    float upZ() const;
    void setUpZ(float upZ);

    float cursorRadius() const;
    void setCursorRadius(float cursorRadius);

    float cursor3DNudge() const;
    void setCursor3DNudge(float cursor3DNudge);

    QVector3D cursor3DPosition() const;
    void setCursor3DPosition(const QVector3D &cursor3DPosition);

    QColor cursorColour() const;
    void setCursorColour(const QColor &cursorColour);

    bool normals() const;
    void setNormals(bool normals);

    bool halfTransparency() const;
    void setHalfTransparency(bool halfTransparency);

    int writeStillFrame(const QString &filename);
    int writeMovieFrame();
    int writeCADFrame(const QString &pathname);
    int writeUSDFrame(const QString &pathname);
    int startAVISave(const QString &filename);
    int stopAVISave();

//    void AddExtraObjectToDraw(const std::string &name, std::shared_ptr<FacetedObject> object);
//    size_t DeleteExtraObjectToDraw(const std::string &name);
//    std::shared_ptr<FacetedObject> GetExtraObjectToDraw(const std::string &name);

    bool deleteDrawBody(const std::string &bodyName);

    AVIWriter *aviWriter() const;

    int aviQuality() const;
    void setAviQuality(int aviQuality);

    QColor backgroundColour() const;
    void setBackgroundColour(const QColor &backgroundColour);

    float axesScale() const;
    void setAxesScale(float axesScale);

    MainWindow *getMainWindow() const;
    void setMainWindow(MainWindow *mainWindow);

    QMatrix4x4 proj() const;
    QMatrix4x4 view() const;

    bool drawBodyMesh1() const;
    void setDrawBodyMesh1(bool drawBodyMesh1);

    bool drawBodyMesh2() const;
    void setDrawBodyMesh2(bool drawBodyMesh2);

    bool drawBodyMesh3() const;
    void setDrawBodyMesh3(bool drawBodyMesh3);

    const IntersectionHits *closestHit() const;

    QString lastMenuItem() const;

    std::map<std::string, std::unique_ptr<DrawBody>> *drawBodyMap();
    std::map<std::string, std::unique_ptr<DrawJoint>> *drawJointMap();
    std::map<std::string, std::unique_ptr<DrawGeom>> *drawGeomMap();
    std::map<std::string, std::unique_ptr<DrawMuscle>> *drawMuscleMap();
    std::map<std::string, std::unique_ptr<DrawFluidSac>> *drawFluidSacMap();
    std::map<std::string, std::unique_ptr<DrawMarker>> *drawMarkerMap();
    std::map<std::string, std::unique_ptr<DrawDataTarget>> *drawDataTargetMap();

    bool shadows() const;
    void setShadows(bool newShadows);

public slots:
    void setCameraVec(float x, float y, float z);
    void setCameraVec(double x, double y, double z);
    void menuRequest(const QPoint &pos);

signals:
    void emitStatusString(const QString &s, int logLevel);
    void emitCOI(float x, float y, float z);
    void emitFoV(float v);
    void emitCreateMarkerRequest();
    void emitEditMarkerRequest(const QString &s);
    void emitMoveMarkerRequest(const QString &s, const QVector3D &p);
    void emitEditBodyRequest(const QString &s);
    void emitEditGeomRequest(const QString &s);
    void emitEditJointRequest(const QString &s);
    void emitEditMuscleRequest(const QString &s);
//    void emitEditFluidSacRequest(const QString &s);
    void emitDeleteMarkerRequest(const QString &s);
    void emitDeleteBodyRequest(const QString &s);
    void emitDeleteGeomRequest(const QString &s);
    void emitDeleteJointRequest(const QString &s);
    void emitDeleteMuscleRequest(const QString &s);
//    void emitDeleteFluidSacRequest(const QString &s);
    void emitInfoRequest(const QString &elementType, const QString &elementName);
    void emitHideRequest(const QString &elementType, const QString &elementName);
    void emitResize(int width, int height);

protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    void setupLights();
    void drawModel();
    bool intersectModel(float winX, float winY);

    GaitSym::Simulation *m_simulation = nullptr;
    MainWindow *m_mainWindow = nullptr;

    bool m_shadows = false;
    bool m_wireframe = false;
    bool m_boundingBox = false;
    bool m_boundingBoxBuffers = false;
    bool m_normals = false;
    bool m_halfTransparency = false;

    bool m_orthographicProjection = true;
    float m_cameraDistance = 50;
    float m_fieldOfView = 5;
    float m_cameraVecX = 0, m_cameraVecY = 1, m_cameraVecZ = 0;
    float m_centreOfInterestX = 0, m_centreOfInterestY = 0, m_centreOfInterestZ = 0;
    float m_frontClip = 1;
    float m_backClip = 1000;
    float m_upX = 0, m_upY = 0, m_upZ = 1;

    std::unique_ptr<Trackball> m_trackball;
    bool m_trackballFlag = false;
    QVector3D m_trackballStartCameraVec;
    QVector3D m_trackballStartUp;

    bool m_panFlag = false;
    QMatrix4x4 m_projectPanMatrix;
    QMatrix4x4 m_unprojectPanMatrix;
    QVector3D m_panStartPoint;
    QVector3D m_panStartCOI;
    QVector3D m_panStartScreenPoint;

    float m_zoomDistance = 0;
    float m_zoomStartFOV = 0;

    std::unique_ptr<FacetedObject> m_cursor3D;
    float m_cursorRadius = 0.1f;
    float m_cursor3DNudge = 0.1f;
    QVector3D m_cursor3DPosition;
    QColor m_cursorColour;
    size_t m_cursorLevel = 4;
    QColor m_backgroundColour;
    float m_axesScale = 1;
    std::unique_ptr<FacetedObject> m_globalAxes;

//    std::map<std::string, std::shared_ptr<FacetedObject>> m_extraObjectsToDrawMap;

    std::unique_ptr<AVIWriter> m_aviWriter;
    int m_aviQuality = 80;
    unsigned int m_fps = 25;

    std::map<std::string, std::unique_ptr<DrawBody>> m_drawBodyMap;
    std::map<std::string, std::unique_ptr<DrawJoint>> m_drawJointMap;
    std::map<std::string, std::unique_ptr<DrawGeom>> m_drawGeomMap;
    std::map<std::string, std::unique_ptr<DrawMuscle>> m_drawMuscleMap;
    std::map<std::string, std::unique_ptr<DrawFluidSac>> m_drawFluidSacMap;
    std::map<std::string, std::unique_ptr<DrawMarker>> m_drawMarkerMap;
    std::map<std::string, std::unique_ptr<DrawDataTarget>> m_drawDataTargetMap;
    std::vector<Drawable *> m_drawables;
    bool m_drawBodyMesh1 = true;
    bool m_drawBodyMesh2 = false;
    bool m_drawBodyMesh3 = false;

    bool m_moveMarkerMode = false;
    std::string m_moveMarkerName;
    std::vector<std::unique_ptr<IntersectionHits>> m_hits;
    std::vector<size_t> m_hitsIndexByZ;
    QString m_lastMenuItem;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_view;

    std::unique_ptr<threepp::GLRenderer> m_renderer;
    std::shared_ptr<threepp::Scene> m_scene;
    std::shared_ptr<threepp::OrthographicCamera> m_orthographicCamera;
    std::shared_ptr<threepp::PerspectiveCamera> m_perspectiveCamera;
    std::shared_ptr<threepp::Group> m_lightGroup;
    std::shared_ptr<threepp::MeshBasicMaterial> m_wireframeMaterial;

};

#endif // SIMULATIONWIDGET_H
