// Handy geometry classes from Physics for Game Developers
// But with lots of changes and additions

#ifndef PGDMATH
#define PGDMATH

#include <iostream>

#pragma pack(push, 1) // packing needed to allow indexed access to the elements

// wis  - namespace to avoid naming problems
namespace pgd
{

//------------------------------------------------------------------------//
// Misc. Constants
//------------------------------------------------------------------------//

//------------------------------------------------------------------------//
// Misc. Functions
//------------------------------------------------------------------------//
double   DegToRad(double deg);
double   RadToDeg(double rad);

//------------------------------------------------------------------------//
// Vector2 Class and Vector2 functions
//------------------------------------------------------------------------//
class Vector2 {
public:
    double x;
    double y;

    Vector2(void);
    Vector2(double xi, double yi);
    Vector2(const double *d);

    void Set(double xi, double yi);
    void Set(const double *xy);

    double Magnitude(void) const;
    double Magnitude2(void) const;
    void  Normalize(void);
    void  Reverse(void);

    double Dot(const Vector2 &v) const;

    Vector2& operator+=(const Vector2 &u);   // Vector2 addition
    Vector2& operator-=(const Vector2 &u);   // Vector2 subtraction
    Vector2& operator*=(double s);    // scalar multiply
    Vector2& operator/=(double s);    // scalar divide
    Vector2& operator=(double *s);    // assign from POD array
    double& operator[] (size_t i);     // index operator
    double operator[] (size_t i) const;     // index operator

    Vector2 operator-(void); // unary negate

    double *data(void);
    const double *constData(void) const;

    bool operator <(const Vector2 &rhs) const; // used for sets and maps
};

Vector2 operator+(const Vector2 &u, const Vector2 &v);
Vector2 operator-(const Vector2 &u, const Vector2 &v);
double operator*(const Vector2 &u, const Vector2 &v);
double Dot(const Vector2 &u, const Vector2 &v);
Vector2 operator*(double s, const Vector2 &u);
Vector2 operator*(const Vector2 &u, double s);
Vector2 operator/(const Vector2 &u, double s);
Vector2 Normalize(const Vector2 &u);


//------------------------------------------------------------------------//
// Vector3 Class and vector functions
//------------------------------------------------------------------------//
class Vector3 {
public:
    double x;
    double y;
    double z;

    Vector3(void);
    Vector3(double xi, double yi, double zi);
    Vector3(const double *d);

    void Set(double xi, double yi, double zi);
    void Set(const double *xyz);

    double Magnitude(void) const;
    double Magnitude2(void) const;
    void  Normalize(void);
    void  Reverse(void);

    double Dot(const Vector3 &v) const;
    Vector3 Cross(const Vector3 &v) const;

    Vector3& operator+=(const Vector3 &u);   // vector addition
    Vector3& operator-=(const Vector3 &u);   // vector subtraction
    Vector3& operator*=(double s);    // scalar multiply
    Vector3& operator/=(double s);    // scalar divide
    Vector3& operator=(double *s);    // assign from POD array
    double& operator[] (size_t i);     // index operator
    double operator[] (size_t i) const;     // index operator

    Vector3 operator-(void); // unary negate

    double *data(void);
    const double *constData(void) const;

    bool operator <(const Vector3 &rhs) const; // used for sets and maps
};

Vector3 operator+(const Vector3 &u, const Vector3 &v);
Vector3 operator-(const Vector3 &u, const Vector3 &v);
Vector3 operator^(const Vector3 &u, const Vector3 &v);
Vector3 Cross(const Vector3 &u, const Vector3 &v);
double operator*(const Vector3 &u, const Vector3 &v);
double Dot(const Vector3 &u, const Vector3 &v);
Vector3 operator*(double s, const Vector3 &u);
Vector3 operator*(const Vector3 &u, double s);
Vector3 operator/(const Vector3 &u, double s);
double TripleScalarProduct(const Vector3 &u, const Vector3 &v, const Vector3 &w);
Vector3 Normalize(const Vector3 &u);
Vector3 Projection(const pgd::Vector3 &v, const pgd::Vector3 &u);

//------------------------------------------------------------------------//
// Vector4 Class and vector functions
//------------------------------------------------------------------------//
class Vector4 {
public:
    double x;
    double y;
    double z;
    double w;

    Vector4(void);
    Vector4(double xi, double yi, double zi, double wi);
    Vector4(const double *d);

    void Set(double xi, double yi, double zi, double wi);
    void Set(const double *xyzw);

    double Magnitude(void) const;
    double Magnitude2(void) const;
    void  Normalize(void);
    void  Reverse(void);

    Vector4& operator+=(const Vector4 &u);   // vector addition
    Vector4& operator-=(const Vector4 &u);   // vector subtraction
    Vector4& operator*=(double s);    // scalar multiply
    Vector4& operator/=(double s);    // scalar divide
    Vector4& operator=(double *s);    // assign from POD array
    double& operator[] (size_t i);    // index operator
    double operator[] (size_t i) const;    // index operator

    Vector4 operator-(void); // unary negate

    double *data(void);
    const double *constData(void) const;

    bool operator <(const Vector4 &rhs) const; // used for sets and maps
};

Vector4 operator+(const Vector4 &u, const Vector4 &v);
Vector4 operator-(const Vector4 &u, const Vector4 &v);
Vector4 operator*(double s, const Vector4 &u);
Vector4 operator*(const Vector4 &u, double s);
Vector4 operator/(const Vector4 &u, double s);
Vector4 Normalize(const Vector4 &u);



//------------------------------------------------------------------------//
// Quaternion Class and Quaternion functions
//------------------------------------------------------------------------//

class Quaternion {
public:
    double n;  // number (scalar) part
    double x;    // vector part: x, y, z
    double y;
    double z;

    Quaternion(void);
    Quaternion(double nn, double x, double y, double z);
    Quaternion(const double *q_nxyz);

    void Set(double nn, double x, double y, double z);
    void Set(const double *q_nxyz);

    double Magnitude(void) const;
    Vector3 GetVector(void) const;
    double GetScalar(void) const;
    void Normalize(void);
    void Conjugate(void);
    Quaternion  operator+=(const Quaternion &q);
    Quaternion  operator-=(const Quaternion &q);
    Quaternion operator*=(double s);
    Quaternion operator/=(double s);
    Quaternion  operator~(void) const { return Quaternion(n, -x, -y, -z);}
    Quaternion  operator-(void) const { return Quaternion(-n, -x, -y, -z);}
    Quaternion& operator=(double *s);    // assign from POD array
    double& operator[] (size_t i);    // index operator
    double operator[] (size_t i) const;    // index operator


    double *data();
    const double *constData() const;

    bool operator <(const Quaternion &rhs) const; // used for sets and maps
};

Quaternion operator+(const Quaternion &q1, const Quaternion &q2);
Quaternion operator-(const Quaternion &q1, const Quaternion &q2);
Quaternion operator*(const Quaternion &q1, const Quaternion &q2);
Quaternion operator*(const Quaternion &q, double s);
Quaternion operator*(double s, const Quaternion &q);
Quaternion operator*(const Quaternion &q, const Vector3 &v);
Quaternion operator*(const Vector3 &v, const Quaternion &q);
Quaternion operator/(const Quaternion &q, double s);
Quaternion Conjugate(const Quaternion &q);
double QGetAngle(const Quaternion &q);
Vector3 QGetAxis(const Quaternion &q);
Quaternion QRotate(const Quaternion &q1, const Quaternion &q2);
Vector3  QVRotate(const Quaternion &q, const Vector3 &v);
Quaternion  MakeQFromEulerAngles(double x, double y, double z);
Quaternion  MakeQFromEulerAnglesRadian(double roll, double pitch, double yaw);
Vector3 MakeEulerAnglesFromQ(const Quaternion &q);
Vector3  MakeEulerAnglesFromQRadian(const Quaternion &q);
Quaternion MakeQFromAxisAngle(double x, double y, double z, double angle, bool fast = false);
Quaternion MakeQFromAxisAngle(const Vector3 &axis, double angle, bool fast = false);
void MakeAxisAngleFromQ(Quaternion q1, double *xa, double *ya, double *za, double *angle);
Quaternion FindRotation(const Quaternion &qa, const Quaternion &qb);
double FindAngle(const Quaternion &qa, const Quaternion &qb);
Vector3 FindAxis(const Quaternion &qa, const Quaternion &qb);
Quaternion FindRotation(const Vector3 &v1, const Vector3 &v2);
void SwingTwistDecomposition(const pgd::Quaternion &rotation, const pgd::Vector3 &direction, pgd::Quaternion *swing, pgd::Quaternion *twist);


//------------------------------------------------------------------------//
// Matrix Class and matrix functions
//------------------------------------------------------------------------//

class Matrix3x3 {
public:
    // elements eij: i -> row, j -> column
    double   e11, e12, e13, e21, e22, e23, e31, e32, e33; // in memory as row major

    Matrix3x3(void);
    Matrix3x3(bool identity); // initialises to identity, or does not intialise at all if false
    Matrix3x3(double r1c1, double r1c2, double r1c3,
              double r2c1, double r2c2, double r2c3,
              double r3c1, double r3c2, double r3c3 );
    Matrix3x3(const Quaternion &q);
    Matrix3x3(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);

    void Set(double r1c1, double r1c2, double r1c3,
             double r2c1, double r2c2, double r2c3,
             double r3c1, double r3c2, double r3c3 );
    void Set(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);
    void SetIdentity();

    double   det(void);
    Matrix3x3   Transpose(void);
    Matrix3x3   Inverse(void);

    Matrix3x3& operator+=(const Matrix3x3 &m);
    Matrix3x3& operator-=(const Matrix3x3 &m);
    Matrix3x3& operator*=(double s);
    Matrix3x3& operator/=(double s);
    Matrix3x3& operator=(double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);    // assign from POD array
    double& operator[] (size_t i);    // index operator
    double operator[] (size_t i) const;    // index operator

    double *data();
    const double *constData() const;

    void SetInertia(double ixx, double iyy, double izz, double ixy, double iyz, double izx);
    void GetInertia(double *ixx, double *iyy, double *izz, double *ixy, double *iyz, double *izx) const;
};

Matrix3x3 operator+(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator-(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator/(const Matrix3x3 &m, double s);
Matrix3x3 operator*(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator*(const Matrix3x3 &m, double s);
Matrix3x3 operator*(double s, const Matrix3x3 &m);
Vector3 operator*(const Matrix3x3 &m, const Vector3 &u);
Vector3 operator*(const Vector3 &u, const Matrix3x3 &m);

Matrix3x3 MakeMFromQ(const pgd::Quaternion &q);
Quaternion MakeQfromM (const pgd::Matrix3x3 &R);
Vector3 ClosestPoint(const pgd::Vector3 &P, const pgd::Vector3 &B, const pgd::Vector3 &M);
Quaternion slerp(pgd::Quaternion v0, pgd::Quaternion v1, double t, bool normalise = true);

std::ostream& operator<<(std::ostream &out, const Vector2 &v);
std::ostream& operator<<(std::ostream &out, const Vector3 &v);
std::ostream& operator<<(std::ostream &out, const Vector4 &v);
std::ostream& operator<<(std::ostream &out, const Quaternion &q);
std::ostream& operator<<(std::ostream &out, const Matrix3x3 &m);

}

#pragma pack(pop) // restore default packing

#endif
