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

    virtual void Draw();

    struct OBJMaterial
    {
        double Ns = {0};
        double Ka[3] = {0, 0, 0};
        double Kd[3] = {0, 0, 0};
        double Ks[3] = {0, 0, 0};
        double Ke[3] = {0, 0, 0};
        double Ni = {0};
        double d = {0};
        int illum = {0};
    };

    int ParseMeshFile(const std::string &filename);
    int ParseOBJFile(const std::string &filename);
    int ParseOBJMaterialFile(const std::string &filename, std::map<std::string, OBJMaterial> *materialMap);
    int ParsePLYFile(const std::string &filename);

    int ReadFromMemory(const char *data, size_t len, bool binary, const std::string &meshName);
    void SaveToMemory(std::vector<char> *data, bool binary);

    int ReadFromResource(const QString &resourceName);

    virtual void WritePOVRay(std::ostringstream &theString);
    virtual void WritePOVRay(std::string filename);
    virtual void WriteOBJFile(std::ostringstream &out);
    virtual void WriteOBJFile(std::string filename);
    virtual void WriteUSDFile(std::ostringstream &out, const std::string &name);

    size_t GetNumVertices() const;
    const double *GetVertex(size_t i) const;
    const double *GetNormal(size_t i) const;
    const double *GetVertexList() const;
    const double *GetNormalList() const;
    const double *GetColourList() const;
    size_t GetNumTriangles() const;
    const double *GetTriangle(size_t i) const;
    const pgd::Vector3 &GetDisplayPosition() const;
    const pgd::Matrix3x3 &GetDisplayRotation() const;
    const pgd::Vector3 &GetDisplayScale() const;

    void AddPolygon(const double *vertices, size_t nSides, const double *normals = nullptr, const double *UVs = nullptr);
    void AddTriangle(const double *vertices, const double *normals = nullptr, const double *UVs = nullptr);
    void AddPolygon(const float *floatVertices, size_t nSides, const float *floatNormals = nullptr, const float *floatUVs = nullptr);
    void AddTriangle(const float *floatVertices, const float *floatNormals = nullptr, const float *floatUVs = nullptr);
    void AddFacetedObject(const FacetedObject *object, bool useDisplayRotation, bool useDirectAccess);
    void RawAppend(const std::vector<double> *vertexList, const std::vector<double> *normalList, const std::vector<double> *colourList, const std::vector<double> *uvList);
    void RawAppend(const std::vector<std::array<double, 3>> *vertexList, const std::vector<std::array<double, 3>> *normalList, const std::vector<std::array<double, 3>> *colourList, const std::vector<std::array<double, 2>> *uvList);


    // static utilities
    static void ComputeFaceNormal(const double *v1, const double *v2, const double *v3, double normal[3]);
    static void ClearMeshStore();

    // manipulation functions
    void Move(double x, double y, double z);
    void Scale(double x, double y, double z);
    void Mirror(bool x, bool y, bool z);
    void SwapAxes(int axis1, int axis2);
    void Rotate(double x, double y, double z, double angleDegrees);

    // utility
    void ReverseWinding();
    void AllocateMemory(size_t numTriangles);
    void ApplyDisplayTransformation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec);
    void ApplyDisplayRotation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec);
    void CalculateMassProperties(double density, bool clockwise, const pgd::Vector3 &translation, double *mass, pgd::Vector3 *centreOfMass, pgd::Matrix3x3 *inertialTensor);

    pgd::Vector3 lowerBound() const;
    pgd::Vector3 upperBound() const;
    double boundingBoxVolume() const;
    pgd::Vector3 boundingBoxSize() const;

    SimulationWidget *simulationWidget() const;
    void setSimulationWidget(SimulationWidget *simulationWidget);

    void setBlendColour(const QColor &blendColour, double blendFraction);
    QColor blendColour() const;
    double blendFraction() const;

    int FindIntersection(const pgd::Vector3 &rayOrigin, const pgd::Vector3 &rayVector, std::vector<pgd::Vector3> *intersectionCoordList, std::vector<size_t> *intersectionIndexList) const;
    static bool RayIntersectsTriangle(const pgd::Vector3 &rayOrigin, const pgd::Vector3 &rayVector, const pgd::Vector3 &vertex0, const pgd::Vector3 &vertex1, const pgd::Vector3 &vertex2, pgd::Vector3 *outIntersectionPoint);
    static bool HitBoundingBox(const double minB[3], const double maxB[3], const double origin[3], const double dir[3], double coord[3]);

    void SetDisplayPosition(double x, double y, double z);
    void SetDisplayScale(double x, double y, double z);
    void SetDisplayPosition(const pgd::Vector3 &displayPosition);
    void SetDisplayScale(const pgd::Vector3 &displayScale);
    void SetDisplayRotation(const pgd::Matrix3x3 &R);
    void SetDisplayRotationFromQuaternion(const pgd::Quaternion &q);

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

private:

    std::vector<double> m_vertexList;
    std::vector<double> m_normalList;
    std::vector<double> m_colourList;
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

    std::ofstream *m_POVRayFile = nullptr;
    std::ofstream *m_OBJFile = nullptr;
    std::string m_OBJName;
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
};

#endif


