#pragma once

#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace PolylineSweep {

// ============================================================
// Vec3 and free-function math (double precision)
// ============================================================

struct Vec3 {
    double x = 0.0, y = 0.0, z = 0.0;
    Vec3() = default;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline Vec3 operator-(const Vec3& v)                { return {-v.x, -v.y, -v.z}; }
inline Vec3 operator*(const Vec3& v, double s)      { return {v.x*s, v.y*s, v.z*s}; }
inline Vec3 operator*(double s, const Vec3& v)      { return v * s; }
inline Vec3 operator/(const Vec3& v, double s)      { return {v.x/s, v.y/s, v.z/s}; }
inline Vec3& operator+=(Vec3& a, const Vec3& b)     { a.x+=b.x; a.y+=b.y; a.z+=b.z; return a; }

inline double dot(const Vec3& a, const Vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}

inline double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
    double len = length(v);
    return (len > 1e-15) ? v / len : Vec3{0.0, 0.0, 0.0};
}

// Rodrigues rotation: rotate v around a unit-length axis by angle radians
inline Vec3 rotateAround(const Vec3& v, const Vec3& axis, double angle) {
    double c = std::cos(angle), s = std::sin(angle);
    return v * c + cross(axis, v) * s + axis * (dot(axis, v) * (1.0 - c));
}

// ============================================================
// Mesh types
// ============================================================

struct Triangle { int a, b, c; };

struct Mesh {
    std::vector<Vec3>     vertices;
    std::vector<Triangle> triangles;
};

// ============================================================
// Sweep options
// ============================================================

struct SweepOptions {
    bool closeEnds = true;
    // Number of rotation steps inserted at each bend node.
    // More steps produce a smoother outer arc at sharp bends.
    int bendSteps = 4;
};

// ============================================================
// Polygon helpers
// ============================================================

// Regular n-gon in XY plane, wound CCW, radius r
std::vector<Vec3> makeNgon(int n, double r = 1.0) {
    std::vector<Vec3> poly;
    poly.reserve(n);
    for (int i = 0; i < n; i++) {
        double a = 2.0 * M_PI * i / n;
        poly.push_back({ r * std::cos(a), r * std::sin(a), 0.0 });
    }
    return poly;
}

// ============================================================
// PolylineSweep
// ============================================================
//
// Sweeps a planar polygon along a 3-D polyline to produce a
// triangular mesh.
//
// At each interior node the polygon is rotated around the inner-
// most polygon vertex (the vertex on the concave side of the
// bend) so that:
//   - the inner edge stays sharp and free from self-intersection
//   - the outer edge fans out smoothly across bendSteps triangles
//
// The polygon is expected in local XY coordinates (Z is ignored).
// scales[i] uniformly scales the polygon at polyline point i.
// ============================================================

class PolylineSweep {
public:
    Mesh sweep(const std::vector<Vec3>&    polyline,
               const std::vector<Vec3>&    polygon,
               const std::vector<double>&  scales,
               const SweepOptions&         opts = {}) const;

private:
    // Local orthonormal frame: zAxis is the sweep tangent direction
    struct Frame {
        Vec3 origin;
        Vec3 xAxis, yAxis, zAxis;
    };

    Frame initialFrame(const Vec3& p0, const Vec3& tangent) const;

    // Parallel-transport f so that zAxis aligns with newTangent,
    // minimising torsion (no unnecessary roll).
    Frame transportFrame(const Frame& f,
                         const Vec3&  newOrigin,
                         const Vec3&  newTangent) const;

    // Place the polygon in world space using f and scale;
    // appends vertices to mesh and returns their indices.
    std::vector<int> addRing(Mesh&                     mesh,
                             const Frame&               f,
                             const std::vector<Vec3>&   polygon,
                             double                     scale) const;

    // Add two triangles forming a quad strip cell.
    // a0,b0 are adjacent polygon vertices in ring r;
    // a1,b1 are the same two vertices in ring r+1.
    void addQuad(Mesh& mesh, int a0, int b0, int a1, int b1) const;

    // Return the index of the polygon vertex most in direction dir2D
    // (using only the x,y components of dir2D for projection).
    int findInnerVertex(const std::vector<Vec3>& polygon,
                        const Vec3&              dir2D) const;
};

// ============================================================
// Implementation
// ============================================================

inline PolylineSweep::Frame
PolylineSweep::initialFrame(const Vec3& p0, const Vec3& tangent) const {
    Frame f;
    f.origin = p0;
    f.zAxis  = normalize(tangent);
    // Pick a world-up vector that is not parallel to the tangent
    Vec3 up  = (std::abs(f.zAxis.z) < 0.9) ? Vec3{0.0, 0.0, 1.0}
                                             : Vec3{1.0, 0.0, 0.0};
    f.xAxis  = normalize(cross(up, f.zAxis));
    f.yAxis  = normalize(cross(f.zAxis, f.xAxis));
    return f;
}

inline PolylineSweep::Frame
PolylineSweep::transportFrame(const Frame& f,
                               const Vec3&  newOrigin,
                               const Vec3&  newTangent) const {
    Frame result;
    result.origin = newOrigin;
    result.zAxis  = normalize(newTangent);

    Vec3 rotAxis = cross(f.zAxis, result.zAxis);
    double axisLen = length(rotAxis);

    if (axisLen < 1e-10) {
        // Tangents are (nearly) parallel or anti-parallel
        if (dot(f.zAxis, result.zAxis) >= 0.0) {
            result.xAxis = f.xAxis;
            result.yAxis = f.yAxis;
        } else {
            // 180-degree flip
            result.xAxis = -f.xAxis;
            result.yAxis = -f.yAxis;
        }
    } else {
        rotAxis = rotAxis / axisLen;
        double cosA = std::max(-1.0, std::min(1.0, dot(f.zAxis, result.zAxis)));
        double angle = std::acos(cosA);
        result.xAxis = rotateAround(f.xAxis, rotAxis, angle);
        result.yAxis = rotateAround(f.yAxis, rotAxis, angle);
    }
    return result;
}

inline std::vector<int>
PolylineSweep::addRing(Mesh&                   mesh,
                        const Frame&             f,
                        const std::vector<Vec3>& polygon,
                        double                   scale) const {
    std::vector<int> indices;
    indices.reserve(polygon.size());
    for (const Vec3& p : polygon) {
        Vec3 world = f.origin
                   + f.xAxis * (p.x * scale)
                   + f.yAxis * (p.y * scale);
        indices.push_back(static_cast<int>(mesh.vertices.size()));
        mesh.vertices.push_back(world);
    }
    return indices;
}

inline void PolylineSweep::addQuad(Mesh& mesh,
                                    int a0, int b0,
                                    int a1, int b1) const {
    // Two triangles with outward-facing normals (polygon CCW from inside,
    // i.e. CW when viewed from outside along the sweep tangent).
    mesh.triangles.push_back({a0, b0, a1});
    mesh.triangles.push_back({b0, b1, a1});
}

inline int PolylineSweep::findInnerVertex(const std::vector<Vec3>& polygon,
                                           const Vec3&              dir2D) const {
    int    best    = 0;
    double bestVal = polygon[0].x * dir2D.x + polygon[0].y * dir2D.y;
    for (int i = 1; i < static_cast<int>(polygon.size()); i++) {
        double val = polygon[i].x * dir2D.x + polygon[i].y * dir2D.y;
        if (val > bestVal) { bestVal = val; best = i; }
    }
    return best;
}

inline Mesh PolylineSweep::sweep(const std::vector<Vec3>&   polyline,
                                  const std::vector<Vec3>&   polygon,
                                  const std::vector<double>& scales,
                                  const SweepOptions&        opts) const {
    const int N = static_cast<int>(polyline.size());
    const int P = static_cast<int>(polygon.size());

    if (N < 2)
        throw std::runtime_error("polyline must have at least 2 points");
    if (P < 3)
        throw std::runtime_error("polygon must have at least 3 vertices");
    if (static_cast<int>(scales.size()) != N)
        throw std::runtime_error("scales.size() must equal polyline.size()");
    if (opts.bendSteps < 1)
        throw std::runtime_error("bendSteps must be >= 1");

    Mesh mesh;
    // Ordered list of polygon rings (each ring is a list of vertex indices)
    std::vector<std::vector<int>> rings;

    // ---------- First ring ----------
    Frame frame = initialFrame(polyline[0], polyline[1] - polyline[0]);
    rings.push_back(addRing(mesh, frame, polygon, scales[0]));

    // ---------- Walk the polyline ----------
    for (int i = 1; i < N; i++) {
        Vec3 t_in = normalize(polyline[i] - polyline[i - 1]);

        // Transport frame to current node
        frame = transportFrame(frame, polyline[i], t_in);

        if (i < N - 1) {
            // Interior node: may have a bend
            Vec3   t_out     = normalize(polyline[i + 1] - polyline[i]);
            double cosA      = std::max(-1.0, std::min(1.0, dot(t_in, t_out)));
            double bendAngle = std::acos(cosA);

            if (bendAngle > 1e-8) {
                Vec3 rotAxis = normalize(cross(t_in, t_out));

                // Outgoing frame axes (incoming axes rotated by bendAngle).
                Vec3 xAxis_out = rotateAround(frame.xAxis, rotAxis, bendAngle);
                Vec3 yAxis_out = rotateAround(frame.yAxis, rotAxis, bendAngle);

                // Find the inner (concave-side) polygon vertex.
                // +(t_in + t_out) bisects toward the inside of the curve.
                int innerIdx = findInnerVertex(polygon, {
                    dot(t_in + t_out, frame.xAxis),
                    dot(t_in + t_out, frame.yAxis),
                    0.0
                });

                // Inner-vertex offset in the incoming and outgoing frames.
                double px = polygon[innerIdx].x * scales[i];
                double py = polygon[innerIdx].y * scales[i];
                Vec3 offset_in  = frame.xAxis * px + frame.yAxis  * py;
                Vec3 offset_out = xAxis_out   * px + yAxis_out    * py;

                // The bend must start where the incoming inner edge terminates
                // and end where the outgoing inner edge begins — both at the
                // same fixed pivot.  Solve:
                //   s1·t_in − s2·t_out = offset_out − offset_in
                // s1 (≤ 0): pull back along t_in from polyline[i]
                // s2 (≥ 0): push forward along t_out from polyline[i]
                Vec3   delta   = offset_out - offset_in;
                double sinA_sq = 1.0 - cosA * cosA;
                double s1 = 0.0, s2 = 0.0;
                if (sinA_sq > 1e-10) {
                    s2 = (dot(delta, t_in) * cosA - dot(delta, t_out)) / sinA_sq;
                    s1 = dot(delta, t_in) + s2 * cosA;
                }

                Vec3 incoming_ctr = polyline[i] + t_in  * s1;  // s1 ≤ 0
                Vec3 outgoing_ctr = polyline[i] + t_out * s2;  // s2 ≥ 0
                Vec3 pivot        = incoming_ctr + offset_in;   // fixed point

                // Incoming ring: end of the segment arriving at this bend
                Frame incoming_frame = frame;
                incoming_frame.origin = incoming_ctr;
                rings.push_back(addRing(mesh, incoming_frame, polygon, scales[i]));

                // Bend fan: rotate each vertex around the pivot
                for (int k = 1; k <= opts.bendSteps; k++) {
                    double angle = bendAngle * static_cast<double>(k) / opts.bendSteps;
                    std::vector<int> ring;
                    ring.reserve(P);
                    for (int j = 0; j < P; j++) {
                        Vec3 vIn = incoming_ctr
                                 + frame.xAxis * (polygon[j].x * scales[i])
                                 + frame.yAxis * (polygon[j].y * scales[i]);
                        Vec3 vRot = pivot + rotateAround(vIn - pivot, rotAxis, angle);
                        ring.push_back(static_cast<int>(mesh.vertices.size()));
                        mesh.vertices.push_back(vRot);
                    }
                    rings.push_back(std::move(ring));
                }

                // Advance frame to the outgoing segment start
                frame.origin = outgoing_ctr;
                frame.xAxis  = xAxis_out;
                frame.yAxis  = yAxis_out;
                frame.zAxis  = t_out;

            } else {
                // Straight segment: single ring at this node
                rings.push_back(addRing(mesh, frame, polygon, scales[i]));
            }

        } else {
            // Last node: just add the final ring
            rings.push_back(addRing(mesh, frame, polygon, scales[i]));
        }
    }

    // ---------- Connect consecutive rings with quad strips ----------
    const int R = static_cast<int>(rings.size());
    for (int r = 0; r + 1 < R; r++) {
        const auto& r0 = rings[r];
        const auto& r1 = rings[r + 1];
        for (int j = 0; j < P; j++) {
            int jn = (j + 1) % P;
            addQuad(mesh, r0[j], r0[jn], r1[j], r1[jn]);
        }
    }

    // ---------- End caps ----------
    if (opts.closeEnds) {
        auto addCap = [&](const std::vector<int>& ring, bool flipWinding) {
            Vec3 center = {0.0, 0.0, 0.0};
            for (int idx : ring) center += mesh.vertices[idx];
            center = center / static_cast<double>(P);
            int ci = static_cast<int>(mesh.vertices.size());
            mesh.vertices.push_back(center);
            for (int j = 0; j < P; j++) {
                int jn = (j + 1) % P;
                if (flipWinding)
                    mesh.triangles.push_back({ring[j], ci, ring[jn]});
                else
                    mesh.triangles.push_back({ring[j], ring[jn], ci});
            }
        };
        addCap(rings.front(), true);   // start cap faces backward
        addCap(rings.back(),  false);  // end cap faces forward
    }

    return mesh;
}

} // namespace PolylineSweep
