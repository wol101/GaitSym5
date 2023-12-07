// Handy geometry classes from Physics for Game Developers
// But with lots of changes and additions

#include "PGDMath.h"

#include <cmath>
#include <cfloat>
#include <iostream>
#include <tuple>

#ifndef M_PI
#define M_PI       3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676
#endif

double pgd::DegToRad(double deg)
{
    return deg * (M_PI / 180.0);
}

double pgd::RadToDeg(double rad)
{
    return rad * (180.0 / M_PI);
}

pgd::Vector2::Vector2(void)
{
    x = 0;
    y = 0;
}

pgd::Vector2::Vector2(double xi, double yi)
{
    x = xi;
    y = yi;
}

pgd::Vector2::Vector2(const double *d)
{
    x = d[0];
    y = d[1];
}

void pgd::Vector2::Set(double xi, double yi)
{
    x = xi;
    y = yi;
}

void pgd::Vector2::Set(const double *xy)
{
    x = xy[0];
    y = xy[1];
}

double pgd::Vector2::Magnitude(void) const
{
    return std::sqrt(x*x + y*y);
}

double pgd::Vector2::Magnitude2(void) const
{
    return (x*x + y*y);
}

void  pgd::Vector2::Normalize(void)
{
    // wis - to cope with very small vectors (quite common) we need to divide by the largest magnitude element
    // to minimise rounding errors. This will make it less good with larger vectors but that's
    // much less common in this application

    double xx = std::abs(x);
    double yy = std::abs(y);
    double m;
    if (yy >= xx)
    {
        m = yy;
    }
    else
    {
        m = xx;
    }
    if (m <= DBL_EPSILON) // too small, need to fix up
    {
        x = 1;
        y = 0;
        return;
    }

    // divide by maximum element to get all numbers to a sensible size for squaring
    x /= m;
    y /= m;

    // now do the standard normalisation calculation
    m = std::sqrt(x*x + y*y);
    x /= m;
    y /= m;
}

void  pgd::Vector2::Reverse(void)
{
    x = -x;
    y = -y;
}

pgd::Vector2& pgd::Vector2::operator+=(const Vector2 &u)
{
    x += u.x;
    y += u.y;
    return *this;
}

pgd::Vector2& pgd::Vector2::operator-=(const Vector2 &u)
{
    x -= u.x;
    y -= u.y;
    return *this;
}

pgd::Vector2& pgd::Vector2::operator*=(double s)
{
    x *= s;
    y *= s;
    return *this;
}

pgd::Vector2& pgd::Vector2::operator/=(double s)
{
    x /= s;
    y /= s;
    return *this;
}

pgd::Vector2& pgd::Vector2::operator=(double *s)
{
    x = s[0];
    y = s[1];
    return *this;
}

double& pgd::Vector2::operator[](size_t i)
{
    return reinterpret_cast<double *>(&x)[i];
}

double pgd::Vector2::operator[](size_t i) const
{
    return reinterpret_cast<const double *>(&x)[i];
}

pgd::Vector2 pgd::Vector2::operator-(void)
{
    return pgd::Vector2(-x, -y);
}

double *pgd::Vector2::data(void)
{
    return &x;
}

const double *pgd::Vector2::constData(void) const
{
    return &x;
}


pgd::Vector2 pgd::operator+(const pgd::Vector2 &u, const pgd::Vector2 &v)
{
    return pgd::Vector2(u.x + v.x, u.y + v.y);
}

pgd::Vector2 pgd::operator-(const pgd::Vector2 &u, const pgd::Vector2 &v)
{
    return pgd::Vector2(u.x - v.x, u.y - v.y);
}

// dot product
double pgd::operator*(const pgd::Vector2 &u, const pgd::Vector2 &v)
{
    return (u.x*v.x + u.y*v.y);
}

double pgd::Dot(const pgd::Vector2 &u, const pgd::Vector2 &v)
{
    return (u.x*v.x + u.y*v.y);
}

double pgd::Vector2::Dot(const Vector2 &v) const
{
    return (x*v.x + y*v.y);
}

pgd::Vector2 pgd::operator*(double s, const pgd::Vector2 &u)
{
    return pgd::Vector2(u.x*s, u.y*s);
}

pgd::Vector2 pgd::operator*(const pgd::Vector2 &u, double s)
{
    return pgd::Vector2(u.x*s, u.y*s);
}

pgd::Vector2 pgd::operator/(const pgd::Vector2 &u, double s)
{
    return pgd::Vector2(u.x/s, u.y/s);
}

pgd::Vector2 pgd::Normalize(const pgd::Vector2 &u)
{
    return u / u.Magnitude();
}

// less than used for sorting in maps and sets
bool pgd::Vector2::operator<(const pgd::Vector2 &rhs) const
{
    return std::tie(x, y) < std::tie(rhs.x, rhs.y);
}


pgd::Vector3::Vector3(void)
{
    x = 0;
    y = 0;
    z = 0;
}

pgd::Vector3::Vector3(double xi, double yi, double zi)
{
    x = xi;
    y = yi;
    z = zi;
}

pgd::Vector3::Vector3(const double *d)
{
    x = d[0];
    y = d[1];
    z = d[2];
}

void pgd::Vector3::Set(double xi, double yi, double zi)
{
    x = xi;
    y = yi;
    z = zi;
}

void pgd::Vector3::Set(const double *xyz)
{
    x = xyz[0];
    y = xyz[1];
    z = xyz[2];
}

double pgd::Vector3::Magnitude(void) const
{
    return std::sqrt(x*x + y*y + z*z);
}

double pgd::Vector3::Magnitude2(void) const
{
    return (x*x + y*y + z*z);
}

void  pgd::Vector3::Normalize(void)
{
    // wis - to cope with very small vectors (quite common) we need to divide by the largest magnitude element
    // to minimise rounding errors. This will make it less good with larger vectors but that's
    // much less common in this application

    double xx = std::abs(x);
    double yy = std::abs(y);
    double zz = std::abs(z);
    double m = xx;
    if (yy >= m) m = yy;
    if (zz >= m) m = zz;
    if (m <= DBL_EPSILON) // too small, need to fix up
    {
        x = 1;
        y = 0;
        z = 0;
        return;
    }

    // divide by maximum element to get all numbers to a sensible size for squaring
    x /= m;
    y /= m;
    z /= m;

    // now do the standard normalisation calculation
    m = std::sqrt(x*x + y*y + z*z);
    x /= m;
    y /= m;
    z /= m;
}

void  pgd::Vector3::Reverse(void)
{
    x = -x;
    y = -y;
    z = -z;
}

pgd::Vector3& pgd::Vector3::operator+=(const pgd::Vector3 &u)
{
    x += u.x;
    y += u.y;
    z += u.z;
    return *this;
}

pgd::Vector3& pgd::Vector3::operator-=(const pgd::Vector3 &u)
{
    x -= u.x;
    y -= u.y;
    z -= u.z;
    return *this;
}

pgd::Vector3& pgd::Vector3::operator*=(double s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

pgd::Vector3& pgd::Vector3::operator/=(double s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

pgd::Vector3& pgd::Vector3::operator=(double *s)
{
    x = s[0];
    y = s[1];
    z = s[2];
    return *this;
}

double& pgd::Vector3::operator [](size_t i)
{
    return reinterpret_cast<double *>(&x)[i];
}

double pgd::Vector3::operator [](size_t i) const
{
    return reinterpret_cast<const double *>(&x)[i];
}


pgd::Vector3 pgd::Vector3::operator-(void)
{
    return pgd::Vector3(-x, -y, -z);
}

double *pgd::Vector3::data(void)
{
    return &x;
}

const double *pgd::Vector3::constData(void) const
{
    return &x;
}


pgd::Vector3 pgd::operator+(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return pgd::Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
}

pgd::Vector3 pgd::operator-(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return pgd::Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
}

// pgd::Vector3 cross product (u cross v)
pgd::Vector3 pgd::operator^(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return pgd::Vector3(  u.y*v.z - u.z*v.y,
                        -u.x*v.z + u.z*v.x,
                        u.x*v.y - u.y*v.x );
}

pgd::Vector3 pgd::Cross(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return pgd::Vector3(  u.y*v.z - u.z*v.y,
                        -u.x*v.z + u.z*v.x,
                        u.x*v.y - u.y*v.x );
}

pgd::Vector3 pgd::Vector3::Cross(const pgd::Vector3 &v) const
{
    return pgd::Vector3(  y*v.z - z*v.y,
                        -x*v.z + z*v.x,
                        x*v.y - y*v.x );
}

// dot product
double pgd::operator*(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return (u.x*v.x + u.y*v.y + u.z*v.z);
}

double pgd::Dot(const pgd::Vector3 &u, const pgd::Vector3 &v)
{
    return (u.x*v.x + u.y*v.y + u.z*v.z);
}

double pgd::Vector3::Dot(const pgd::Vector3 &v) const
{
    return (x*v.x + y*v.y + z*v.z);
}

pgd::Vector3 pgd::operator*(double s, const pgd::Vector3 &u)
{
    return pgd::Vector3(u.x*s, u.y*s, u.z*s);
}

pgd::Vector3 pgd::operator*(const pgd::Vector3 &u, double s)
{
    return pgd::Vector3(u.x*s, u.y*s, u.z*s);
}

pgd::Vector3 pgd::operator/(const pgd::Vector3 &u, double s)
{
    return pgd::Vector3(u.x/s, u.y/s, u.z/s);
}

// triple scalar product (u dot (v cross w))
double pgd::TripleScalarProduct(const pgd::Vector3 &u, const pgd::Vector3 &v, const pgd::Vector3 &w)
{
    return double(  (u.x * ( v.y*w.z - v.z*w.y)) +
                  (u.y * (-v.x*w.z + v.z*w.x)) +
                  (u.z * ( v.x*w.y - v.y*w.x)) );
    //return u*(v^w);

}

pgd::Vector3 pgd::Normalize(const pgd::Vector3 &u)
{
    return u / u.Magnitude();
}

// less than used for sorting in maps and sets
bool pgd::Vector3::operator<(const pgd::Vector3 &rhs) const
{
    return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
}

pgd::Vector4::Vector4(void)
{
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

pgd::Vector4::Vector4(double xi, double yi, double zi, double wi)
{
    x = xi;
    y = yi;
    z = zi;
    w = wi;
}

pgd::Vector4::Vector4(const double *d)
{
    x = d[0];
    y = d[1];
    z = d[2];
    w = d[3];
}

void pgd::Vector4::Set(double xi, double yi, double zi, double wi)
{
    x = xi;
    y = yi;
    z = zi;
    w = wi;
}

void pgd::Vector4::Set(const double *xyzw)
{
    x = xyzw[0];
    y = xyzw[1];
    z = xyzw[2];
    w = xyzw[3];
}

double pgd::Vector4::Magnitude(void) const
{
    return std::sqrt(x*x + y*y + z*z + w*w);
}

double pgd::Vector4::Magnitude2(void) const
{
    return (x*x + y*y + z*z + w*w);
}

void  pgd::Vector4::Normalize(void)
{
    // wis - to cope with very small vectors (quite common) we need to divide by the largest magnitude element
    // to minimise rounding errors. This will make it less good with larger vectors but that's
    // much less common in this application

    double xx = std::abs(x);
    double yy = std::abs(y);
    double zz = std::abs(z);
    double ww = std::abs(w);
    double m = xx;
    if (yy > m) m = yy;
    if (zz > m) m = zz;
    if (ww > m) m = ww;
    if (m <= DBL_EPSILON) // too small, need to fix up
    {
        x = 1;
        y = 0;
        z = 0;
        w = 0;
        return;
    }

    // divide by maximum element to get all numbers to a sensible size for squaring
    x /= m;
    y /= m;
    z /= m;
    w /= m;

    // now do the standard normalisation calculation
    m = std::sqrt(x*x + y*y + z*z + w*w);
    x /= m;
    y /= m;
    z /= m;
    w /= w;
}

void  pgd::Vector4::Reverse(void)
{
    x = -x;
    y = -y;
    z = -z;
    w = -w;
}

pgd::Vector4& pgd::Vector4::operator+=(const pgd::Vector4 &u)
{
    x += u.x;
    y += u.y;
    z += u.z;
    w += u.w;
    return *this;
}

pgd::Vector4& pgd::Vector4::operator-=(const pgd::Vector4 &u)
{
    x -= u.x;
    y -= u.y;
    z -= u.z;
    w -= u.w;
    return *this;
}

pgd::Vector4& pgd::Vector4::operator*=(double s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= w;
    return *this;
}

pgd::Vector4& pgd::Vector4::operator/=(double s)
{
    x /= s;
    y /= s;
    z /= s;
    w /= w;
    return *this;
}

pgd::Vector4& pgd::Vector4::operator=(double *s)
{
    x = s[0];
    y = s[1];
    z = s[2];
    w = s[3];
    return *this;
}

double& pgd::Vector4::operator[](size_t i)
{
    return reinterpret_cast<double *>(&x)[i];
}

double pgd::Vector4::operator[](size_t i) const
{
    return reinterpret_cast<const double *>(&x)[i];
}


pgd::Vector4 pgd::Vector4::operator-(void)
{
    return pgd::Vector4(-x, -y, -z, -w);
}

double *pgd::Vector4::data(void)
{
    return &x;
}

const double *pgd::Vector4::constData(void) const
{
    return &x;
}


pgd::Vector4 pgd::operator+(const pgd::Vector4 &u, const pgd::Vector4 &v)
{
    return pgd::Vector4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + u.w);
}

pgd::Vector4 pgd::operator-(const pgd::Vector4 &u, const pgd::Vector4 &v)
{
    return pgd::Vector4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - u.w);
}

pgd::Vector4 pgd::operator*(double s, const pgd::Vector4 &u)
{
    return pgd::Vector4(u.x*s, u.y*s, u.z*s, u.w*s);
}

pgd::Vector4 pgd::operator*(const pgd::Vector4 &u, double s)
{
    return pgd::Vector4(u.x*s, u.y*s, u.z*s, u.w * s);
}

pgd::Vector4 pgd::operator/(const pgd::Vector4 &u, double s)
{
    return pgd::Vector4(u.x/s, u.y/s, u.z/s, u.w/s);
}

pgd::Vector4 pgd::Normalize(const pgd::Vector4 &u)
{
    return u / u.Magnitude();
}

// less than used for sorting in maps and sets
bool pgd::Vector4::operator<(const pgd::Vector4 &rhs) const
{
    return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}


pgd::Quaternion::Quaternion(void)
{
    n = 0;
    x = 0;
    y = 0;
    z = 0;
}

pgd::Quaternion::Quaternion(double nn, double x, double y, double z)
{
    this->n = nn;
    this->x = x;
    this->y = y;
    this->z = z;
}

pgd::Quaternion::Quaternion(const double *q_nxyz)
{
    n = q_nxyz[0];
    x = q_nxyz[1];
    y = q_nxyz[2];
    z = q_nxyz[3];
}

void pgd::Quaternion::Set(double nn, double x, double y, double z)
{
    this->n = nn;
    this->x = x;
    this->y = y;
    this->z = z;
}

void pgd::Quaternion::Set(const double *q_nxyz)
{
    n = q_nxyz[0];
    x = q_nxyz[1];
    y = q_nxyz[2];
    z = q_nxyz[3];
}

double pgd::Quaternion::Magnitude(void) const
{
    return std::sqrt(n*n + x*x + y*y + z*z);
}

pgd::Vector3 pgd::Quaternion::GetVector(void) const
{
    return pgd::Vector3(x, y, z);
}

double pgd::Quaternion::GetScalar(void) const
{
    return n;
}

pgd::Quaternion pgd::Quaternion::operator+=(const pgd::Quaternion &q)
{
    n += q.n;
    x += q.x;
    y += q.y;
    z += q.z;
    return *this;
}

pgd::Quaternion pgd::Quaternion::operator-=(const pgd::Quaternion &q)
{
    n -= q.n;
    x -= q.x;
    y -= q.y;
    z -= q.z;
    return *this;
}

pgd::Quaternion pgd::Quaternion::operator*=(double s)
{
    n *= s;
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

pgd::Quaternion pgd::Quaternion::operator/=(double s)
{
    n /= s;
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

pgd::Quaternion& pgd::Quaternion::operator=(double *s)
{
    n = s[0];
    x = s[1];
    y = s[2];
    z = s[3];
    return *this;
}

double& pgd::Quaternion::operator[](size_t i)
{
    return reinterpret_cast<double *>(&n)[i];
}

double pgd::Quaternion::operator[](size_t i) const
{
    return reinterpret_cast<const double *>(&n)[i];
}

double *pgd::Quaternion::data()
{
    return &n;
}

const double *pgd::Quaternion::constData() const
{
    return &n;
}

pgd::Quaternion pgd::operator+(const pgd::Quaternion &q1, const pgd::Quaternion &q2)
{
    return  pgd::Quaternion(q1.n + q2.n,
                            q1.x + q2.x,
                            q1.y + q2.y,
                            q1.z + q2.z);
}

pgd::Quaternion pgd::operator-(const pgd::Quaternion &q1, const pgd::Quaternion &q2)
{
    return  pgd::Quaternion(q1.n - q2.n,
                            q1.x - q2.x,
                            q1.y - q2.y,
                            q1.z - q2.z);
}

pgd::Quaternion pgd::operator*(const pgd::Quaternion &q1, const pgd::Quaternion &q2)
{
    return  pgd::Quaternion(q1.n*q2.n - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
                            q1.n*q2.x + q1.x*q2.n + q1.y*q2.z - q1.z*q2.y,
                            q1.n*q2.y + q1.y*q2.n + q1.z*q2.x - q1.x*q2.z,
                            q1.n*q2.z + q1.z*q2.n + q1.x*q2.y - q1.y*q2.x);
}

pgd::Quaternion pgd::operator*(const pgd::Quaternion &q, double s)
{
    return pgd::Quaternion(q.n*s, q.x*s, q.y*s, q.z*s);
}

pgd::Quaternion pgd::operator*(double s, const pgd::Quaternion &q)
{
    return pgd::Quaternion(q.n*s, q.x*s, q.y*s, q.z*s);
}

pgd::Quaternion pgd::operator*(const pgd::Quaternion &q, const pgd::Vector3 &v)
{
    return pgd::Quaternion(-(q.x*v.x + q.y*v.y + q.z*v.z),
                           q.n*v.x + q.y*v.z - q.z*v.y,
                           q.n*v.y + q.z*v.x - q.x*v.z,
                           q.n*v.z + q.x*v.y - q.y*v.x);
}

pgd::Quaternion pgd::operator*(const pgd::Vector3 &v, const pgd::Quaternion &q)
{
    return  pgd::Quaternion( -(q.x*v.x + q.y*v.y + q.z*v.z),
                           q.n*v.x + q.z*v.y - q.y*v.z,
                           q.n*v.y + q.x*v.z - q.z*v.x,
                           q.n*v.z + q.y*v.x - q.x*v.y);
}

pgd::Quaternion pgd::operator/(const pgd::Quaternion &q, double s)
{
    return  pgd::Quaternion(q.n/s, q.x/s, q.y/s, q.z/s);
}

// less than used for sorting in maps and sets
bool pgd::Quaternion::operator<(const pgd::Quaternion &rhs) const
{
    return std::tie(n, x, y, z) < std::tie(rhs.n, rhs.x, rhs.y, rhs.z);
}

pgd::Quaternion pgd::Conjugate(const pgd::Quaternion &q)
{
    return pgd::Quaternion(q.n, -q.x, -q.y, -q.z);
}

double pgd::QGetAngle(const pgd::Quaternion &q)
{
    if (q.n <= -1) return 0; // 2 * M_PI
    if (q.n >= 1) return 0; // 2 * 0
    return  (2*std::acos(q.n));
}

pgd::Vector3 pgd::QGetAxis(const pgd::Quaternion &q)
{
    pgd::Vector3 v = q.GetVector();
    v.Normalize();
    return v;
}

pgd::Quaternion pgd::QRotate(const pgd::Quaternion &q1, const pgd::Quaternion &q2) // so called 'sandwich product'
{
    return  q1*q2*(~q1);
}

pgd::Vector3 pgd::QVRotate(const pgd::Quaternion &q, const pgd::Vector3 &v) // so called 'sandwich product'
{
#ifdef EASY_TO_READ
    pgd::Quaternion t;
    t = q*v*(~q);
    return  t.GetVector();
#else
    // optimisation based on OpenSG code
    double rx,ry,rz;
    double QwQx, QwQy, QwQz, QxQy, QxQz, QyQz;

    QwQx = q.n * q.x;
    QwQy = q.n * q.y;
    QwQz = q.n * q.z;
    QxQy = q.x * q.y;
    QxQz = q.x * q.z;
    QyQz = q.y * q.z;

    rx = 2* (v.y * (-QwQz + QxQy) + v.z *( QwQy + QxQz));
    ry = 2* (v.x * ( QwQz + QxQy) + v.z *(-QwQx + QyQz));
    rz = 2* (v.x * (-QwQy + QxQz) + v.y *( QwQx + QyQz));

    double QwQw, QxQx, QyQy, QzQz;

    QwQw = q.n * q.n;
    QxQx = q.x * q.x;
    QyQy = q.y * q.y;
    QzQz = q.z * q.z;

    rx+= v.x * (QwQw + QxQx - QyQy - QzQz);
    ry+= v.y * (QwQw - QxQx + QyQy - QzQz);
    rz+= v.z * (QwQw - QxQx - QyQy + QzQz);

    return pgd::Vector3(rx,ry,rz);

#endif
}

// these are intrinsic Euler XYZ angles (or fixed axis ZYX)
pgd::Quaternion pgd::MakeQFromEulerAngles(double x, double y, double z)
{
    pgd::Quaternion  q;
    double   roll = pgd::DegToRad(x);
    double   pitch = pgd::DegToRad(y);
    double   yaw = pgd::DegToRad(z);

    double   cyaw, cpitch, croll, syaw, spitch, sroll;
    double   cyawcpitch, syawspitch, cyawspitch, syawcpitch;

    cyaw = std::cos(0.5 * yaw);
    cpitch = std::cos(0.5 * pitch);
    croll = std::cos(0.5 * roll);
    syaw = std::sin(0.5 * yaw);
    spitch = std::sin(0.5 * pitch);
    sroll = std::sin(0.5 * roll);

    cyawcpitch = cyaw*cpitch;
    syawspitch = syaw*spitch;
    cyawspitch = cyaw*spitch;
    syawcpitch = syaw*cpitch;

    q.n = (cyawcpitch * croll + syawspitch * sroll);
    q.x = (cyawcpitch * sroll - syawspitch * croll);
    q.y = (cyawspitch * croll + syawcpitch * sroll);
    q.z = (syawcpitch * croll - cyawspitch * sroll);

    return q;
}

// these are intrinsic Euler XYZ angles (or fixed axis ZYX)
pgd::Vector3 pgd::MakeEulerAnglesFromQ(const pgd::Quaternion &q)
{
    double   r11, r21, r31, r32, r33, r12, r13;
    double   q00, q11, q22, q33;
    double   tmp;
    pgd::Vector3  u;

    q00 = q.n * q.n;
    q11 = q.x * q.x;
    q22 = q.y * q.y;
    q33 = q.z * q.z;

    r11 = q00 + q11 - q22 - q33;
    r21 = 2 * (q.x*q.y + q.n*q.z);
    r31 = 2 * (q.x*q.z - q.n*q.y);
    r32 = 2 * (q.y*q.z + q.n*q.x);
    r33 = q00 - q11 - q22 + q33;

    tmp = std::abs(r31);
    if(tmp > (1 - DBL_EPSILON))
    {
        r12 = 2 * (q.x*q.y - q.n*q.z);
        r13 = 2 * (q.x*q.z + q.n*q.y);

        u.x = pgd::RadToDeg(0.0); //roll
        u.y = pgd::RadToDeg((-(M_PI/2) * r31/tmp)); // pitch
        u.z = pgd::RadToDeg(std::atan2(-r12, -r31*r13)); // yaw
        return u;
    }

    u.x = pgd::RadToDeg(std::atan2(r32, r33)); // roll
    u.y = pgd::RadToDeg(std::asin(-r31));      // pitch
    u.z = pgd::RadToDeg(std::atan2(r21, r11)); // yaw
    return u;

}

// these are intrinsic Euler XYZ angles (or fixed axis ZYX)
pgd::Quaternion pgd::MakeQFromEulerAnglesRadian(double roll, double pitch, double yaw)
{
    pgd::Quaternion  q;

    double   cyaw, cpitch, croll, syaw, spitch, sroll;
    double   cyawcpitch, syawspitch, cyawspitch, syawcpitch;

    cyaw = std::cos(0.5 * yaw);
    cpitch = std::cos(0.5 * pitch);
    croll = std::cos(0.5 * roll);
    syaw = std::sin(0.5 * yaw);
    spitch = std::sin(0.5 * pitch);
    sroll = std::sin(0.5 * roll);

    cyawcpitch = cyaw*cpitch;
    syawspitch = syaw*spitch;
    cyawspitch = cyaw*spitch;
    syawcpitch = syaw*cpitch;

    q.n = (cyawcpitch * croll + syawspitch * sroll);
    q.x = (cyawcpitch * sroll - syawspitch * croll);
    q.y = (cyawspitch * croll + syawcpitch * sroll);
    q.z = (syawcpitch * croll - cyawspitch * sroll);

    return q;
}


// these are intrinsic Euler XYZ angles (or fixed axis ZYX)
pgd::Vector3 pgd::MakeEulerAnglesFromQRadian(const pgd::Quaternion &q)
{
    double   r11, r21, r31, r32, r33, r12, r13;
    double   q00, q11, q22, q33;
    double   tmp;
    pgd::Vector3  u;

    q00 = q.n * q.n;
    q11 = q.x * q.x;
    q22 = q.y * q.y;
    q33 = q.z * q.z;

    r11 = q00 + q11 - q22 - q33;
    r21 = 2 * (q.x*q.y + q.n*q.z);
    r31 = 2 * (q.x*q.z - q.n*q.y);
    r32 = 2 * (q.y*q.z + q.n*q.x);
    r33 = q00 - q11 - q22 + q33;

    tmp = std::abs(r31);
    if(tmp > (1 - DBL_EPSILON))
    {
        r12 = 2 * (q.x*q.y - q.n*q.z);
        r13 = 2 * (q.x*q.z + q.n*q.y);

        u.x = 0.0; //roll
        u.y = (-(M_PI/2) * r31/tmp); // pitch
        u.z = std::atan2(-r12, -r31*r13); // yaw
        return u;
    }

    u.x = std::atan2(r32, r33); // roll
    u.y = std::asin(-r31);      // pitch
    u.z = std::atan2(r21, r11); // yaw
    return u;

}

// wis  - added routine to make a pgd::Quaternion from an axis and a rotation angle in radians
pgd::Quaternion pgd::MakeQFromAxisAngle(double x, double y, double z, double angle, bool fast)
{
    if (fast)
    {
        // could use cos(x)^2 = 1 - sin(x)^2
        // cos_a = sqrt(1.0 - sin_a * sin_a);
        double halfAngle = angle / 2;
        double sin_a = std::sin( halfAngle );
        double cos_a = std::cos( halfAngle );
        return pgd::Quaternion(cos_a, x * sin_a, y * sin_a, z * sin_a);
    }
    else
    {
        pgd::Quaternion  q;
        pgd::Vector3 v(x, y, z);
        v.Normalize();
        double sin_a = std::sin( angle / 2 );
        double cos_a = std::cos( angle / 2 );
        q.x    = v.x * sin_a;
        q.y    = v.y * sin_a;
        q.z    = v.z * sin_a;
        q.n    = cos_a;
        return q;
    }
}

// wis  - added routine to make a pgd::Quaternion from an axis and a rotation angle in radians
pgd::Quaternion pgd::MakeQFromAxisAngle(const pgd::Vector3 &axis, double angle, bool fast)
{
    if (fast)
    {
        // could use cos(x)^2 = 1 - sin(x)^2
        // cos_a = sqrt(1.0 - sin_a * sin_a);
        double halfAngle = angle / 2;
        double sin_a = std::sin( halfAngle );
        double cos_a = std::cos( halfAngle );
        return pgd::Quaternion(cos_a, axis.x * sin_a, axis.y * sin_a, axis.z * sin_a);
    }
    else
    {
        pgd::Quaternion  q;
        pgd::Vector3 v(axis / axis.Magnitude());
        double sin_a = std::sin( angle / 2 );
        double cos_a = std::cos( angle / 2 );
        q.x    = v.x * sin_a;
        q.y    = v.y * sin_a;
        q.z    = v.z * sin_a;
        q.n    = cos_a;
        return q;
    }
}

void pgd::MakeAxisAngleFromQ(pgd::Quaternion q, double *xa, double *ya, double *za, double *angle)
{
    if (q.n > 1) q.Normalize();           // if w > 1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
    *angle = 2 * std::acos(q.n);
    double s = std::sqrt(1 - q.n * q.n);  // assuming quaternion normalised then w is less than 1, so term always positive.
    if (s < DBL_EPSILON)                  // test to avoid divide by zero, s is always positive due to sqrt
    {
        // if s close to zero then direction of axis not important
        *angle = 0;
        *xa = 1;
        *ya = 0;
        *za = 0;
    }
    else
    {
        *xa = q.x / s; // normalise axis
        *ya = q.y / s;
        *za = q.z / s;
    }
}

// wis - added routines to calculate the quaternion which rotates qa to qb
// PS. I'm slightly unconvinced that [qb = qa * q]. It may depend on convention but I think [qb = q * qa] which changes things.
//
// based on http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
// if q is the quaternion which rotates from qa to qb then:
// qb = qa * q
//
// multiplying both sides by conj(qa) gives:
// q = conj(qa) * qb
//
// The real part of a multiplication is:
// real((qa.w + i qa.x + j qa.y + k qa.z)*(qb.w + i qb.x + j qb.y + k qb.z)) = qa.w * qb.w - qa.x*qb.x - qa.y*qb.y- qa.z*qb.z
//
// So using the conjugate of qa gives:
// real((qa.w - i qa.x - j qa.y - k qa.z)*(qb.w + i qb.x + j qb.y + k qb.z)) = qa.w*qb.w + qa.x*qb.x + qa.y*qb.y+ qa.z*qb.z
//
// real(q) = std::cos(t/2)
//
// Therefore
// std::cos(theta/2) = qa.w*qb.w + qa.x*qb.x + qa.y*qb.y+ qa.z*qb.z

pgd::Quaternion pgd::FindRotation(const pgd::Quaternion &qa, const pgd::Quaternion &qb)
{
    return ((~qa) * qb);
}

double pgd::FindAngle(const pgd::Quaternion &qa, const pgd::Quaternion &qb)
{
    double v = qa.n*qb.n + qa.x*qb.x + qa.y*qb.y+ qa.z*qb.z;
    if (v <= -1) return 0; // 2 * M_PI
    if (v >= 1) return 0; // 2 * 0
    double angle = 2 * std::acos(v);
    if (angle < -M_PI) angle += (2 * M_PI);
    else if (angle > M_PI) angle -= (2 * M_PI);
    return angle;
}

pgd::Vector3 pgd::FindAxis(const pgd::Quaternion &qa, const pgd::Quaternion &qb)
{
    return QGetAxis((~qa) * qb);
}

// this is useful when reading in quaternions since there is inevitable precision loss
// which means the quaternion will be slightly denormal
void pgd::Quaternion::Normalize(void)
{
    double l = std::sqrt(n*n + x*x + y*y + z*z);
    n /= l;
    x /= l;
    y /= l;
    z /= l;
}

void pgd::Quaternion::Conjugate(void)
{
    x = -x;
    y = -y;
    z = -z;
}

// this routine returns the quaternion that rotates v1 to v2 via the shortest path
// note the vectors do not have to be the same size
pgd::Quaternion pgd::FindRotation(const pgd::Vector3 &v1, const pgd::Vector3 &v2)
{
    // classic solutions
    pgd::Quaternion q;
    q.n = std::sqrt((v1.Magnitude2()) * (v2.Magnitude2())) + Dot(v1, v2); // this effectively halves the rotation angle but denormalises the quaternion
    if (q.n < DBL_EPSILON) // this only occurs if a 180 degree rotation is needed
    {
        pgd::Vector3 perp; // this is a perpendicular vector (v1 dot perp = 0)
        if (fabs(v1.z) > DBL_EPSILON) perp = pgd::Vector3(0, -v1.z, v1.y);
        else perp = pgd::Vector3(-v1.y, v1.x, 0);
        q = MakeQFromAxisAngle(perp.x, perp.y, perp.z, M_PI);
    }
    else
    {
        pgd::Vector3 c = Cross(v1, v2);
        q.x = c.x;
        q.y = c.y;
        q.z = c.z;
        q.Normalize();
    }
    return q;
}

// project vector v onto vector u
pgd::Vector3 pgd::Projection(const pgd::Vector3 &v, const pgd::Vector3 &u)
{
    // (v . norm(u) ) norm(u)
    pgd::Vector3 normU = pgd::Normalize(u);
    return v.Dot(normU) * normU;
}

//       Decompose the rotation on to 2 parts.
//       1. Twist - rotation around the "direction" vector
//       2. Swing - rotation around axis that is perpendicular to "direction" vector
//       The rotation can be composed back by
//       rotation = swing * twist

//       has singularity in case of swing_rotation close to 180 degrees rotation.
//       if the input quaternion is of non-unit length, the outputs are non-unit as well
//       otherwise, outputs are both unit
void pgd::SwingTwistDecomposition(const pgd::Quaternion &rotation, const pgd::Vector3 &direction, pgd::Quaternion *swing, pgd::Quaternion *twist)
{
    pgd::Vector3 ra(rotation.x, rotation.y, rotation.z); // rotation axis
    pgd::Vector3 p = Projection(ra, direction); // return projection v1 on to v2  (parallel component)
    twist->Set(rotation.n, p.x, p.y, p.z);
    twist->Normalize();
    *swing = rotation * pgd::Conjugate(*twist);
}

pgd::Matrix3x3::Matrix3x3(void)
{
    e11 = 0;
    e12 = 0;
    e13 = 0;
    e21 = 0;
    e22 = 0;
    e23 = 0;
    e31 = 0;
    e32 = 0;
    e33 = 0;
}

pgd::Matrix3x3::Matrix3x3(bool identity)
{
    if (identity)
    {
        e11 = 1;
        e12 = 0;
        e13 = 0;
        e21 = 0;
        e22 = 1;
        e23 = 0;
        e31 = 0;
        e32 = 0;
        e33 = 1;
    }
}

pgd::Matrix3x3::Matrix3x3(double r1c1, double r1c2, double r1c3,
                          double r2c1, double r2c2, double r2c3,
                          double r3c1, double r3c2, double r3c3)
{
    e11 = r1c1;
    e12 = r1c2;
    e13 = r1c3;
    e21 = r2c1;
    e22 = r2c2;
    e23 = r2c3;
    e31 = r3c1;
    e32 = r3c2;
    e33 = r3c3;
}

pgd::Matrix3x3::Matrix3x3(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3)
{
    e11 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[0];
    e12 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[1];
    e13 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[2];
    e21 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[3];
    e22 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[4];
    e23 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[5];
    e31 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[6];
    e32 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[7];
    e33 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[8];
}

void pgd::Matrix3x3::Set(double r1c1, double r1c2, double r1c3,
                         double r2c1, double r2c2, double r2c3,
                         double r3c1, double r3c2, double r3c3)
{
    e11 = r1c1;
    e12 = r1c2;
    e13 = r1c3;
    e21 = r2c1;
    e22 = r2c2;
    e23 = r2c3;
    e31 = r3c1;
    e32 = r3c2;
    e33 = r3c3;
}

void pgd::Matrix3x3::Set(const double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3)
{
    e11 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[0];
    e12 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[1];
    e13 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[2];
    e21 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[3];
    e22 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[4];
    e23 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[5];
    e31 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[6];
    e32 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[7];
    e33 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[8];
}

void pgd::Matrix3x3::SetIdentity()
{
    e11 = 1;
    e12 = 0;
    e13 = 0;
    e21 = 0;
    e22 = 1;
    e23 = 0;
    e31 = 0;
    e32 = 0;
    e33 = 1;
}

double *pgd::Matrix3x3::data()
{
    return &e11;
}

const double *pgd::Matrix3x3::constData() const
{
    return &e11;
}

// initialise a matrix from a quaternion
// based on code from
// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
// untested!
pgd::Matrix3x3::Matrix3x3(const pgd::Quaternion &q)
{
    double sqw = q.n*q.n;
    double sqx = q.x*q.x;
    double sqy = q.y*q.y;
    double sqz = q.z*q.z;

    // invs (inverse square length) is only required if quaternion is not already normalised
    double invs = 1 / (sqx + sqy + sqz + sqw);
    e11 = ( sqx - sqy - sqz + sqw)*invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
    e22 = (-sqx + sqy - sqz + sqw)*invs ;
    e33 = (-sqx - sqy + sqz + sqw)*invs ;

    double tmp1 = q.x*q.y;
    double tmp2 = q.z*q.n;
    e21 = 2.0 * (tmp1 + tmp2)*invs ;
    e12 = 2.0 * (tmp1 - tmp2)*invs ;

    tmp1 = q.x*q.z;
    tmp2 = q.y*q.n;
    e31 = 2.0 * (tmp1 - tmp2)*invs ;
    e13 = 2.0 * (tmp1 + tmp2)*invs ;
    tmp1 = q.y*q.z;
    tmp2 = q.x*q.n;
    e32 = 2.0 * (tmp1 + tmp2)*invs ;
    e23 = 2.0 * (tmp1 - tmp2)*invs ;
}

double pgd::Matrix3x3::det(void)
{
    return  e11*e22*e33 -
           e11*e32*e23 +
           e21*e32*e13 -
           e21*e12*e33 +
           e31*e12*e23 -
           e31*e22*e13;
}

pgd::Matrix3x3 pgd::Matrix3x3::Transpose(void)
{
    return pgd::Matrix3x3(e11,e21,e31,e12,e22,e32,e13,e23,e33);
}

pgd::Matrix3x3 pgd::Matrix3x3::Inverse(void)
{
    double d = e11*e22*e33 -
               e11*e32*e23 +
               e21*e32*e13 -
               e21*e12*e33 +
               e31*e12*e23 -
               e31*e22*e13;

    if (std::fabs(d) < DBL_EPSILON) d = 1;

    return  pgd::Matrix3x3( (e22*e33-e23*e32)/d,
                           -(e12*e33-e13*e32)/d,
                            (e12*e23-e13*e22)/d,
                           -(e21*e33-e23*e31)/d,
                            (e11*e33-e13*e31)/d,
                           -(e11*e23-e13*e21)/d,
                            (e21*e32-e22*e31)/d,
                           -(e11*e32-e12*e31)/d,
                            (e11*e22-e12*e21)/d );
}

pgd::Matrix3x3& pgd::Matrix3x3::operator+=(const pgd::Matrix3x3 &m)
{
    e11 += m.e11;
    e12 += m.e12;
    e13 += m.e13;
    e21 += m.e21;
    e22 += m.e22;
    e23 += m.e23;
    e31 += m.e31;
    e32 += m.e32;
    e33 += m.e33;
    return *this;
}

pgd::Matrix3x3& pgd::Matrix3x3::operator-=(const pgd::Matrix3x3 &m)
{
    e11 -= m.e11;
    e12 -= m.e12;
    e13 -= m.e13;
    e21 -= m.e21;
    e22 -= m.e22;
    e23 -= m.e23;
    e31 -= m.e31;
    e32 -= m.e32;
    e33 -= m.e33;
    return *this;
}

pgd::Matrix3x3& pgd::Matrix3x3::operator*=(double s)
{
    e11 *= s;
    e12 *= s;
    e13 *= s;
    e21 *= s;
    e22 *= s;
    e23 *= s;
    e31 *= s;
    e32 *= s;
    e33 *= s;
    return *this;
}

pgd::Matrix3x3& pgd::Matrix3x3::operator/=(double s)
{
    e11 /= s;
    e12 /= s;
    e13 /= s;
    e21 /= s;
    e22 /= s;
    e23 /= s;
    e31 /= s;
    e32 /= s;
    e33 /= s;
    return *this;
}

pgd::Matrix3x3& pgd::Matrix3x3::operator=(double *mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3)
{
    e11 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[0];
    e12 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[1];
    e13 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[2];
    e21 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[3];
    e22 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[4];
    e23 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[5];
    e31 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[6];
    e32 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[7];
    e33 = mat_r1c1r1c2r1c3_r2c1r2c2r2c3_r3c1r3c2r3c3[8];
    return *this;
}

double& pgd::Matrix3x3::operator[](size_t i)
{
    return reinterpret_cast<double *>(&e11)[i];
}

double pgd::Matrix3x3::operator[](size_t i) const
{
    return reinterpret_cast<const double *>(&e11)[i];
}

void pgd::Matrix3x3::SetInertia(double ixx, double iyy, double izz, double ixy, double iyz, double izx)
{
    e11 = ixx; e12 = ixy; e13 = izx;
    e21 = ixy; e22 = iyy; e23 = iyz;
    e31 = izx; e32 = iyz; e33 = izz;
}

void pgd::Matrix3x3::GetInertia(double *ixx, double *iyy, double *izz, double *ixy, double *iyz, double *izx) const
{
    *ixx = e11;       *ixy = e12; *izx = e13;
    /* *ixy = e21; */ *iyy = e22; *iyz = e23;
    /* *izx = e31; *iyz = e32; */ *izz = e33;
}


pgd::Matrix3x3 pgd::operator+(const pgd::Matrix3x3 &m1, const pgd::Matrix3x3 &m2)
{
    return  pgd::Matrix3x3(m1.e11+m2.e11,
                           m1.e12+m2.e12,
                           m1.e13+m2.e13,
                           m1.e21+m2.e21,
                           m1.e22+m2.e22,
                           m1.e23+m2.e23,
                           m1.e31+m2.e31,
                           m1.e32+m2.e32,
                           m1.e33+m2.e33);
}

pgd::Matrix3x3 pgd::operator-(const pgd::Matrix3x3 &m1, const pgd::Matrix3x3 &m2)
{
    return  pgd::Matrix3x3(m1.e11-m2.e11,
                           m1.e12-m2.e12,
                           m1.e13-m2.e13,
                           m1.e21-m2.e21,
                           m1.e22-m2.e22,
                           m1.e23-m2.e23,
                           m1.e31-m2.e31,
                           m1.e32-m2.e32,
                           m1.e33-m2.e33);
}

pgd::Matrix3x3 pgd::operator/(const pgd::Matrix3x3 &m, double s)
{
    return  pgd::Matrix3x3(m.e11/s,
                           m.e12/s,
                           m.e13/s,
                           m.e21/s,
                           m.e22/s,
                           m.e23/s,
                           m.e31/s,
                           m.e32/s,
                           m.e33/s);
}

pgd::Matrix3x3 pgd::operator*(const pgd::Matrix3x3 &m1, const pgd::Matrix3x3 &m2)
{
    return pgd::Matrix3x3(m1.e11*m2.e11 + m1.e12*m2.e21 + m1.e13*m2.e31,
                          m1.e11*m2.e12 + m1.e12*m2.e22 + m1.e13*m2.e32,
                          m1.e11*m2.e13 + m1.e12*m2.e23 + m1.e13*m2.e33,
                          m1.e21*m2.e11 + m1.e22*m2.e21 + m1.e23*m2.e31,
                          m1.e21*m2.e12 + m1.e22*m2.e22 + m1.e23*m2.e32,
                          m1.e21*m2.e13 + m1.e22*m2.e23 + m1.e23*m2.e33,
                          m1.e31*m2.e11 + m1.e32*m2.e21 + m1.e33*m2.e31,
                          m1.e31*m2.e12 + m1.e32*m2.e22 + m1.e33*m2.e32,
                          m1.e31*m2.e13 + m1.e32*m2.e23 + m1.e33*m2.e33);
}

pgd::Matrix3x3 pgd::operator*(const pgd::Matrix3x3 &m, double s)
{
    return  pgd::Matrix3x3(m.e11*s,
                           m.e12*s,
                           m.e13*s,
                           m.e21*s,
                           m.e22*s,
                           m.e23*s,
                           m.e31*s,
                           m.e32*s,
                           m.e33*s);
}

pgd::Matrix3x3 pgd::operator*(double s, const pgd::Matrix3x3 &m)
{
    return  pgd::Matrix3x3(m.e11*s,
                           m.e12*s,
                           m.e13*s,
                           m.e21*s,
                           m.e22*s,
                           m.e23*s,
                           m.e31*s,
                           m.e32*s,
                           m.e33*s);
}

pgd::Vector3 pgd::operator*(const pgd::Matrix3x3 &m, const pgd::Vector3 &u)
{
    return pgd::Vector3(  m.e11*u.x + m.e12*u.y + m.e13*u.z,
                        m.e21*u.x + m.e22*u.y + m.e23*u.z,
                        m.e31*u.x + m.e32*u.y + m.e33*u.z);
}

pgd::Vector3 pgd::operator*(const pgd::Vector3 &u, const pgd::Matrix3x3 &m)
{
    return pgd::Vector3(u.x*m.e11 + u.y*m.e21 + u.z*m.e31,
                        u.x*m.e12 + u.y*m.e22 + u.z*m.e32,
                        u.x*m.e13 + u.y*m.e23 + u.z*m.e33);
}

pgd::Matrix3x3 pgd::MakeMFromQ(const pgd::Quaternion &q)
{
    pgd::Matrix3x3 m;

    double qq1 = 2*q.x*q.x;
    double qq2 = 2*q.y*q.y;
    double qq3 = 2*q.z*q.z;
    m.e11 = 1 - qq2 - qq3;
    m.e12 = 2*(q.x*q.y - q.n*q.z);
    m.e13 = 2*(q.x*q.z + q.n*q.y);
    m.e21 = 2*(q.x*q.y + q.n*q.z);
    m.e22 = 1 - qq1 - qq3;
    m.e23 = 2*(q.y*q.z - q.n*q.x);
    m.e31 = 2*(q.x*q.z - q.n*q.y);
    m.e32 = 2*(q.y*q.z + q.n*q.x);
    m.e33 = 1 - qq1 - qq2;
    return m;
}

pgd::Quaternion pgd::MakeQfromM (const pgd::Matrix3x3 &R)
{
    pgd::Quaternion q;
    double tr,s;
    tr = R.e11 + R.e22 + R.e33;
    if (tr >= 0)
    {
        s = std::sqrt (tr + 1);
        q.n = 0.5 * s;
        s = 0.5 * (1.0/s);
        q.x = (R.e32 - R.e23) * s;
        q.y = (R.e13 - R.e31) * s;
        q.z = (R.e21 - R.e12) * s;
    }
    else
    {
        // find the largest diagonal element and jump to the appropriate case
        if (R.e22 > R.e11)
        {
            if (R.e33 > R.e22) goto case_2;
            goto case_1;
        }
        if (R.e33 > R.e11) goto case_2;
        goto case_0;

    case_0:
        s = std::sqrt((R.e11 - (R.e22 + R.e33)) + 1);
        q.x = 0.5 * s;
        s = 0.5 * (1.0/s);
        q.y = (R.e12 + R.e21) * s;
        q.z = (R.e31 + R.e13) * s;
        q.n = (R.e32 - R.e23) * s;
        return q;

    case_1:
        s = std::sqrt((R.e22 - (R.e33 + R.e11)) + 1);
        q.y = 0.5 * s;
        s = 0.5 * (1.0/s);
        q.z = (R.e23 + R.e32) * s;
        q.x = (R.e12 + R.e21) * s;
        q.n = (R.e13 - R.e31) * s;
        return q;

    case_2:
        s = std::sqrt((R.e33 - (R.e11 + R.e22)) + 1);
        q.z = 0.5 * s;
        s = 0.5 * (1.0/s);
        q.x = (R.e31 + R.e13) * s;
        q.y = (R.e23 + R.e32) * s;
        q.n = (R.e21 - R.e12) * s;
        return q;
    }
    return q;
}

// find the closest point to point P on a line defined as origin B and direction M
// using formulae from www.geometrictools.com
pgd::Vector3 pgd::ClosestPoint(const pgd::Vector3 &P, const pgd::Vector3 &B, const pgd::Vector3 &M)
{
    double t0 = (M * (P - B)) / (M * M);
    pgd::Vector3 Q = B + (t0 * M);
    return Q;
}

//  generates a quaternion between two given quaternions in proportion to the variable t
// if t=0 then qm=qa, if t=1 then qm=qb, if t is between them then qm will interpolate between them
pgd::Quaternion pgd::slerp(pgd::Quaternion v0, pgd::Quaternion v1, double t, bool normalise)
{
    if (normalise)
    {
        // Only unit quaternions are valid rotations.
        // Normalize to avoid undefined behavior.
        v0.Normalize();
        v1.Normalize();
    }

    // Compute the cosine of the angle between the two vectors.
    double dot = v0.GetVector() * v1.GetVector(); // dot product

    const double DOT_THRESHOLD = 0.9995;
    if (dot > DOT_THRESHOLD)
    {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.

        pgd::Quaternion result = v0 + t * (v1 - v0);
        result.Normalize();
        return result;
    }

    // If the dot product is negative, the quaternions
    // have opposite handed-ness and slerp won't take
    // the shorter path. Fix by reversing one quaternion.
    if (dot < 0.0)
    {
        v1 = -v1;
        dot = -dot;
    }

    if (dot < -1) dot = -1;
    else if (dot > 1) dot = 1;   // Robustness: Stay within domain of std::acos()
    double theta_0 = std::acos(dot);  // theta_0 = angle between input vectors
    double theta = theta_0*t;    // theta = angle between v0 and result

    pgd::Quaternion v2 = v1 - v0*dot;
    v2.Normalize();              // { v0, v2 } is now an orthonormal basis

    return v0*std::cos(theta) + v2*std::sin(theta);
}

std::ostream& pgd::operator<<(std::ostream &out, const pgd::Vector2 &v)
{
    out << v.x << " " << v.y;
    return out;
}

std::ostream& pgd::operator<<(std::ostream &out, const pgd::Vector3 &v)
{
    out << v.x << " " << v.y << " " << v.z;
    return out;
}
std::ostream& pgd::operator<<(std::ostream &out, const pgd::Vector4 &v)
{
    out << v.x << " " << v.y << " " << v.z << " " << v.w;
    return out;
}

std::ostream& pgd::operator<<(std::ostream &out, const pgd::Quaternion &q)
{
    out << q.n << " " << q.x << " " << q.y << " " << q.z;
    return out;
}

std::ostream& pgd::operator<<(std::ostream &out, const pgd::Matrix3x3 &m)
{
    out << m.e11 << " " << m.e12 << " " << m.e13 << "\n";
    out << m.e21 << " " << m.e22 << " " << m.e23 << "\n";
    out << m.e31 << " " << m.e32 << " " << m.e33;
    return out;
}

