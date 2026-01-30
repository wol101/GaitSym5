/*
 *  FacetedObject.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef FacetedObject_h
#define FacetedObject_h

#include "MeshStore.h"
#include "PGDMath.h"

#include "threepp/threepp.hpp"

#include <QColor>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>

class SimulationWidget;

class FacetedObject
{
public:
    FacetedObject();
    virtual ~FacetedObject();

    virtual void draw();

    struct OBJMaterial
    {
        float Ns = {0};
        float Ka[3] = {0, 0, 0};
        float Kd[3] = {0, 0, 0};
        float Ks[3] = {0, 0, 0};
        float Ke[3] = {0, 0, 0};
        float Ni = {0};
        float d = {0};
        int illum = {0};
    };

    int parseMeshFile(const std::string &filename);
    int parseOBJFile(const std::string &filename);
    int parseOBJMaterialFile(const std::string &filename, std::map<std::string, OBJMaterial> *materialMap);
    int parsePLYFile(const std::string &filename);

    int readFromMemory(const char *data, size_t len, bool binary, const std::string &meshName);
    void saveToMemory(std::vector<char> *data, bool binary);

    int readFromResource(const QString &resourceName);

    virtual void writePOVRay(std::ostringstream &theString);
    virtual void writePOVRay(std::string filename);
    virtual void writeOBJFile(std::ostringstream &out);
    virtual void writeOBJFile(std::string filename);
    virtual void writeUSDFile(std::ostringstream &out, const std::string &name);

    const double *vertex(size_t i) const;
    const double *normal(size_t i) const;
    const float *colour(size_t i) const;
    const double *uv(size_t i) const;
    const std::vector<double> &vertexList() const;
    const std::vector<double> &normalList() const;
    const std::vector<float> &colourList() const;
    const std::vector<double> &uvList() const;

    size_t numTriangles() const;
    const double *triangle(size_t i) const;
    const pgd::Vector3 &displayPosition() const;
    const pgd::Matrix3x3 &displayRotation() const;
    const pgd::Vector3 &displayScale() const;

    void addPolygon(const double *vertices, size_t nSides, const double *normals = nullptr, const double *UVs = nullptr);
    void addTriangle(const double *vertices, const double *normals = nullptr, const double *UVs = nullptr);
    void addPolygon(const float *floatVertices, size_t nSides, const float *floatNormals = nullptr, const float *floatUVs = nullptr);
    void addTriangle(const float *floatVertices, const float *floatNormals = nullptr, const float *floatUVs = nullptr);
    void addFacetedObject(const FacetedObject *object, bool useDisplayRotation, bool useDirectAccess);
    void rawAppend(const std::vector<double> *vertexList, const std::vector<double> *normalList, const std::vector<float> *colourList, const std::vector<double> *uvList);
    void rawAppend(const std::vector<std::array<double, 3>> *vertexList, const std::vector<std::array<double, 3>> *normalList, const std::vector<std::array<float, 3>> *colourList, const std::vector<std::array<double, 2>> *uvList);
    void rawAppend(const std::vector<std::array<double, 3>> *vertexList, const std::vector<std::array<double, 3>> *normalList, const std::vector<std::array<float, 4>> *colourList, const std::vector<std::array<double, 2>> *uvList);


    // static utilities
    static void computeFaceNormal(const double *v1, const double *v2, const double *v3, double normal[3]);
    static void clearMeshStore();

    // manipulation functions
    void move(double x, double y, double z);
    void scale(double x, double y, double z);
    void mirror(bool x, bool y, bool z);
    void rotate(double x, double y, double z, double angleDegrees);

    // utility
    void reverseWinding();
    void allocateMemory(size_t numTriangles);
    void applyDisplayTransformation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec);
    void applyDisplayRotation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec);
    void calculateMassProperties(double density, bool clockwise, const pgd::Vector3 &translation, double *mass, pgd::Vector3 *centreOfMass, pgd::Matrix3x3 *inertialTensor);
    void updateBoundingBox();

    pgd::Vector3 lowerBound() const;
    pgd::Vector3 upperBound() const;
    double boundingBoxVolume() const;
    pgd::Vector3 boundingBoxSize() const;

    SimulationWidget *simulationWidget() const;
    void setSimulationWidget(SimulationWidget *simulationWidget);

    void setBlendColour(const QColor &blendColour, double blendFraction);
    QColor blendColour() const;
    double blendFraction() const;

    int findIntersection(const pgd::Vector3 &rayOrigin, const pgd::Vector3 &rayVector, std::vector<pgd::Vector3> *intersectionCoordList, std::vector<size_t> *intersectionIndexList) const;
    static bool rayIntersectsTriangle(const pgd::Vector3 &rayOrigin, const pgd::Vector3 &rayVector, const pgd::Vector3 &vertex0, const pgd::Vector3 &vertex1, const pgd::Vector3 &vertex2, pgd::Vector3 *outIntersectionPoint);
    static bool hitBoundingBox(const double minB[3], const double maxB[3], const double origin[3], const double dir[3], double coord[3]);

    void setDisplayPosition(double x, double y, double z);
    void setDisplayScale(double x, double y, double z);
    void setDisplayPosition(const pgd::Vector3 &displayPosition);
    void setDisplayScale(const pgd::Vector3 &displayScale);
    void setDisplayRotation(const pgd::Matrix3x3 &R);
    void setDisplayRotationFromQuaternion(const pgd::Quaternion &q);

    bool visible() const;
    void setVisible(bool visible);

    const QMatrix4x4 &model();
    void setModel(const QMatrix4x4 &model);

    double decal() const;
    void setDecal(double decal);

    std::string filename() const;

    std::shared_ptr<threepp::Scene> scene();
    void setScene(const std::shared_ptr<threepp::Scene> &newScene);

    bool castShadow() const;
    void setCastShadow(bool newCastShadow);

    bool receiveShadow() const;
    void setReceiveShadow(bool newReceiveShadow);

    bool wireframe() const;
    void setWireframe(bool newWireframe);

private:

    std::vector<double> m_vertexList;
    std::vector<double> m_normalList;
    std::vector<float> m_colourList;
    std::vector<double> m_uvList;
    bool m_useRelativeOBJ = false;
    bool m_badMesh = false;
    pgd::Vector3 m_lowerBound = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_upperBound = {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};

    pgd::Vector3 m_displayPosition = {0, 0, 0};
    pgd::Vector3 m_displayScale = {1, 1, 1};
    pgd::Quaternion m_displayQuaternion = {1, 0, 0, 0};
    pgd::Matrix3x3 m_displayRotation = {1, 0, 0,
                                        0, 1, 0,
                                        0, 0, 1};
    bool m_visible = true;
    QMatrix4x4 m_model;
    bool m_modelValid = false;

    std::string m_objName;
    std::string m_filename;
    size_t m_vertexOffset = 0;

    QColor m_blendColour = {255, 255, 255, 255};
    double m_blendFraction = 0;
    SimulationWidget *m_simulationWidget = nullptr;
    double m_decal = 0;

    static MeshStore m_meshStore;

    std::shared_ptr<threepp::Scene> m_scene;
    std::shared_ptr<threepp::Mesh> m_mesh;
    bool m_castShadow = true;
    bool m_receiveShadow = false;
    bool m_wireframe = false;
};

#endif


