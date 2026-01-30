// Handy geometry classes from Physics for Game Developers
// But with lots of changes and additions

#ifndef PGDMATH
#define PGDMATH

#include <iostream>

#pragma pack(push) // the clang warning is a known bug
#pragma pack(1) // packing needed to allow indexed access to the elements

// wis  - namespace to avoid naming problems
namespace pgd
{

// predefinitions
class Matrix3x3;

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

    void set(double xi, double yi);
    void set(const double *xy);

    double magnitude(void) const;
    double magnitude2(void) const;
    void  normalize(void);
    void  reverse(void);

    double dot(const Vector2 &v) const;

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
double dot(const Vector2 &u, const Vector2 &v);
Vector2 operator*(double s, const Vector2 &u);
Vector2 operator*(const Vector2 &u, double s);
Vector2 operator/(const Vector2 &u, double s);
Vector2 normalize(const Vector2 &u);


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

    void set(double xi, double yi, double zi);
    void set(const double *xyz);

    double magnitude(void) const;
    double magnitude2(void) const;
    void  normalize(void);
    void  reverse(void);

    double dot(const Vector3 &v) const;
    Vector3 cross(const Vector3 &v) const;

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
Vector3 cross(const Vector3 &u, const Vector3 &v);
double operator*(const Vector3 &u, const Vector3 &v);
double dot(const Vector3 &u, const Vector3 &v);
Vector3 operator*(double s, const Vector3 &u);
Vector3 operator*(const Vector3 &u, double s);
Vector3 operator/(const Vector3 &u, double s);
double tripleScalarProduct(const Vector3 &u, const Vector3 &v, const Vector3 &w);
Vector3 normalize(const Vector3 &u);
Vector3 projection(const pgd::Vector3 &v, const pgd::Vector3 &u);
pgd::Vector3 perpendicularVector(const pgd::Vector3 &v);

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

    void set(double xi, double yi, double zi, double wi);
    void set(const double *xyzw);

    double magnitude(void) const;
    double magnitude2(void) const;
    void  normalize(void);
    void  reverse(void);

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
Vector4 normalize(const Vector4 &u);



//------------------------------------------------------------------------//
// Quaternion Class and Quaternion functions
//------------------------------------------------------------------------//

class Quaternion {
public:
    double n;  // number (scalar) part
    double x;  // vector part: x, y, z
    double y;
    double z;

    Quaternion(void);
    Quaternion(double nn, double xx, double yy, double zz);
    Quaternion(const double *q_nxyz);
    Quaternion(bool identity);

    void set(double nn, double xx, double yy, double zz);
    void set(const double *q_nxyz);

    double magnitude(void) const;
    Vector3 vector(void) const;
    double scalar(void) const;
    void normalize(void);
    void conjugate(void);
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
Quaternion conjugate(const Quaternion &q);
double qGetAngle(const Quaternion &q);
Vector3 qGetAxis(const Quaternion &q);
Quaternion qRotate(const Quaternion &q1, const Quaternion &q2);
Vector3  qVRotate(const Quaternion &q, const Vector3 &v);
Quaternion  makeQFromEulerAngles(double x, double y, double z); // these are extrinsic/global/static/fixed axis Euler XYZ angles
Quaternion  makeQFromEulerAnglesRadian(double roll, double pitch, double yaw); // these are extrinsic/global/static/fixed axis Euler XYZ angles
pgd::Quaternion makeQFromEulerAnglesRadian(const pgd::Vector3 &eulerAngles, const std::string &order); // these are intrinsic/local/dynamic/moving axis Euler angles
pgd::Vector3 makeEulerAnglesFromQRadian(const pgd::Quaternion &q, const pgd::Matrix3x3 &basis);
Vector3 makeEulerAnglesFromQ(const Quaternion &q); // these are extrinsic/global/static/fixed axis Euler XYZ angles
Vector3  makeEulerAnglesFromQRadian(const Quaternion &q); // these are extrinsic/global/static/fixed axis Euler XYZ angles
Quaternion makeQFromAxisAngle(double x, double y, double z, double angle, bool fast = false);
Quaternion makeQFromAxisAngle(const Vector3 &axis, double angle, bool fast = false);
void makeAxisAngleFromQ(Quaternion q1, double *xa, double *ya, double *za, double *angle);
void makeAxisAngleFromQ(pgd::Quaternion q, pgd::Vector3 *axis, double *angle);
Quaternion findRotation(const Quaternion &qa, const Quaternion &qb);
double findAngle(const Quaternion &qa, const Quaternion &qb);
Vector3 findAxis(const Quaternion &qa, const Quaternion &qb);
Quaternion findRotation(const Vector3 &v1, const Vector3 &v2);
void qwingTwistDecomposition(const pgd::Quaternion &rotation, const pgd::Vector3 &direction, pgd::Quaternion *swing, pgd::Quaternion *twist);
void qGetBasis(const pgd::Quaternion &q, pgd::Vector3 *x, pgd::Vector3 *y, pgd::Vector3 *z);

//------------------------------------------------------------------------//
// Matrix Class and matrix functions
//------------------------------------------------------------------------//

class Matrix3x3 {
public:
    // elements eij: i -> row, j -> column
    double   e11, e12, e13,
             e21, e22, e23,
             e31, e32, e33; // in memory as row major

    Matrix3x3(void); // initialises elements to zero
    Matrix3x3(bool identity); // initialises to identity, or does not intialise at all if false
    Matrix3x3(double r1c1, double r1c2, double r1c3,
              double r2c1, double r2c2, double r2c3,
              double r3c1, double r3c2, double r3c3 );
    Matrix3x3(const Quaternion &q);
    Matrix3x3(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);

    void set(double r1c1, double r1c2, double r1c3,
             double r2c1, double r2c2, double r2c3,
             double r3c1, double r3c2, double r3c3 );
    void set(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);
    void setIdentity();
    void setCols(const Vector3 &x, const Vector3 &y, const Vector3 &z);
    void setRows(const Vector3 &x, const Vector3 &y, const Vector3 &z);

    double   determinant(void) const;
    Matrix3x3   transpose(void) const;
    Matrix3x3   inverse(void) const;

    Matrix3x3& operator+=(const Matrix3x3 &m);
    Matrix3x3& operator-=(const Matrix3x3 &m);
    Matrix3x3& operator*=(double s);
    Matrix3x3& operator/=(double s);
    Matrix3x3& operator=(double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3);    // assign from POD array
    double& operator[] (size_t i);    // index operator
    double operator[] (size_t i) const;    // index operator

    double *data();
    const double *constData() const;

    void setInertia(double ixx, double iyy, double izz, double ixy, double izx, double iyz);
    void getInertia(double *ixx, double *iyy, double *izz, double *ixy, double *izx, double *iyz) const;

    Vector3 col1();
    Vector3 col2();
    Vector3 col3();
    Vector3 row1();
    Vector3 row2();
    Vector3 row3();
};

Matrix3x3 operator+(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator-(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator/(const Matrix3x3 &m, double s);
Matrix3x3 operator*(const Matrix3x3 &m1, const Matrix3x3 &m2);
Matrix3x3 operator*(const Matrix3x3 &m, double s);
Matrix3x3 operator*(double s, const Matrix3x3 &m);
Vector3 operator*(const Matrix3x3 &m, const Vector3 &u);
Vector3 operator*(const Vector3 &u, const Matrix3x3 &m);

Matrix3x3 makeMFromQ(const pgd::Quaternion &q);
Quaternion makeQfromM (const pgd::Matrix3x3 &R);
pgd::Matrix3x3 findRotation(const pgd::Matrix3x3 &R1, const pgd::Matrix3x3 &R2);
Vector3 closestPoint(const pgd::Vector3 &P, const pgd::Vector3 &B, const pgd::Vector3 &M);
Quaternion slerp(pgd::Quaternion v0, pgd::Quaternion v1, double t, bool normalise = true);

std::ostream& operator<<(std::ostream &out, const Vector2 &v);
std::ostream& operator<<(std::ostream &out, const Vector3 &v);
std::ostream& operator<<(std::ostream &out, const Vector4 &v);
std::ostream& operator<<(std::ostream &out, const Quaternion &q);
std::ostream& operator<<(std::ostream &out, const Matrix3x3 &m);

}

#pragma pack(pop) // restore default packing

#endif
