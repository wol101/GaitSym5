/*
 *  PolylineSweep.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 22/06/2026
 *  Copyright 2026 Bill Sellers. All rights reserved.
 *
 */

#ifndef POLYLINESWEEP_H
#define POLYLINESWEEP_H

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
inline double length(const Vec3& v) { return std::sqrt(dot(v, v)); }
inline Vec3 normalize(const Vec3& v) {
    double len = length(v);
    return (len > 1e-15) ? v / len : Vec3{};
}
// Rodrigues rotation: rotate v around a unit-length axis by angle radians
inline Vec3 rotateAround(const Vec3& v, const Vec3& axis, double angle) {
    double c = std::cos(angle), s = std::sin(angle);
    return v * c + cross(axis, v) * s + axis * (dot(axis, v) * (1.0 - c));
}

// ============================================================
// Color (RGB, double precision, 0–1 range)
// ============================================================

struct Color {
    double r = 1.0, g = 1.0, b = 1.0;
    Color() = default;
    Color(double r, double g, double b) : r(r), g(g), b(b) {}
};
inline Color operator+(const Color& a, const Color& b) { return {a.r+b.r, a.g+b.g, a.b+b.b}; }
inline Color operator*(const Color& c, double s)       { return {c.r*s, c.g*s, c.b*s}; }
inline Color operator*(double s, const Color& c)       { return c * s; }
inline Color lerp(const Color& a, const Color& b, double t) { return a*(1.0-t) + b*t; }

// ============================================================
// UV coordinates
// ============================================================

struct UV {
    double u = 0.0, v = 0.0;
    UV() = default;
    UV(double u, double v) : u(u), v(v) {}
};

// ============================================================
// Mesh types
// ============================================================

// Vertex / normal / UV index triple for one triangle corner.
// When normals or UVs are absent, normalTris / uvTris are empty.
struct Triangle { int a, b, c; };

struct Mesh {
    std::vector<Vec3>     vertices;
    std::vector<Color>    vertexColors;  // optional, same count as vertices
    std::vector<Vec3>     normals;       // optional
    std::vector<UV>       uvs;           // optional
    std::vector<Triangle> triangles;     // vertex indices
    std::vector<Triangle> normalTris;    // normal indices, parallel to triangles
    std::vector<Triangle> uvTris;        // UV indices,     parallel to triangles
};

// ============================================================
// Sweep options
// ============================================================

struct SweepOptions {
    bool closeEnds       = true;
    // Number of rotation steps at each bend node.
    int  bendSteps       = 4;
    // Normals
    bool generateNormals = true;
    // true  = smooth (per-vertex) normals on the tube; caps always flat.
    // false = flat (per-face) normals everywhere.
    bool smoothNormals   = true;
    // UVs: U wraps 0→1 around the cross-section; V increases with arc length.
    bool generateUVs     = true;
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
// most polygon vertex so that the inner edge stays sharp and the
// outer edge fans smoothly across bendSteps triangles.
//
// polygon : closed polygon in local XY (Z ignored), wound so that
//           the outward face is correct when swept.
// scales  : one scale factor per polyline point.
// colors  : optional per-node colors; if non-empty must match
//           polyline.size(). Interpolated across segments.
// ============================================================

class PolylineSweep {
public:
    Mesh sweep(const std::vector<Vec3>&   polyline,
               const std::vector<Vec3>&   polygon,
               const std::vector<double>& scales,
               const SweepOptions&        opts   = {},
               const std::vector<Color>&  colors = {}) const;

private:
    struct Frame {
        Vec3 origin;
        Vec3 xAxis, yAxis, zAxis;  // zAxis = tangent direction
    };

    Frame initialFrame  (const Vec3& p0, const Vec3& tangent) const;
    Frame transportFrame(const Frame& f, const Vec3& newOrigin,
                         const Vec3& newTangent) const;

    // Append polygon vertices (+ optional color) to mesh; return indices.
    std::vector<int> addRingVerts(Mesh& mesh, const Frame& f,
                                  const std::vector<Vec3>& polygon,
                                  double scale,
                                  const Color* color) const;

    int findInnerVertex(const std::vector<Vec3>& polygon,
                        const Vec3& dir2D) const;
};

// ============================================================
// Implementation
// ============================================================

inline PolylineSweep::Frame
PolylineSweep::initialFrame(const Vec3& p0, const Vec3& tangent) const {
    Frame f;
    f.origin = p0;
    f.zAxis  = normalize(tangent);
    Vec3 up  = (std::abs(f.zAxis.z) < 0.9) ? Vec3{0,0,1} : Vec3{1,0,0};
    f.xAxis  = normalize(cross(up, f.zAxis));
    f.yAxis  = normalize(cross(f.zAxis, f.xAxis));
    return f;
}

inline PolylineSweep::Frame
PolylineSweep::transportFrame(const Frame& f, const Vec3& newOrigin,
                              const Vec3& newTangent) const {
    Frame r;
    r.origin = newOrigin;
    r.zAxis  = normalize(newTangent);
    Vec3 ax  = cross(f.zAxis, r.zAxis);
    double len = length(ax);
    if (len < 1e-10) {
        r.xAxis = (dot(f.zAxis, r.zAxis) >= 0.0) ? f.xAxis : -f.xAxis;
        r.yAxis = (dot(f.zAxis, r.zAxis) >= 0.0) ? f.yAxis : -f.yAxis;
    } else {
        ax = ax / len;
        double angle = std::acos(std::max(-1.0, std::min(1.0, dot(f.zAxis, r.zAxis))));
        r.xAxis = rotateAround(f.xAxis, ax, angle);
        r.yAxis = rotateAround(f.yAxis, ax, angle);
    }
    return r;
}

inline std::vector<int>
PolylineSweep::addRingVerts(Mesh& mesh, const Frame& f,
                            const std::vector<Vec3>& polygon,
                            double scale, const Color* color) const {
    std::vector<int> idx;
    idx.reserve(polygon.size());
    for (const Vec3& p : polygon) {
        idx.push_back(static_cast<int>(mesh.vertices.size()));
        mesh.vertices.push_back(f.origin + f.xAxis*(p.x*scale) + f.yAxis*(p.y*scale));
        if (color) mesh.vertexColors.push_back(*color);
    }
    return idx;
}

inline int PolylineSweep::findInnerVertex(const std::vector<Vec3>& polygon,
                                          const Vec3& dir2D) const {
    int best = 0;
    double bestVal = polygon[0].x*dir2D.x + polygon[0].y*dir2D.y;
    for (int i = 1; i < (int)polygon.size(); i++) {
        double val = polygon[i].x*dir2D.x + polygon[i].y*dir2D.y;
        if (val > bestVal) { bestVal = val; best = i; }
    }
    return best;
}

inline Mesh PolylineSweep::sweep(const std::vector<Vec3>&   polyline,
                                 const std::vector<Vec3>&   polygon,
                                 const std::vector<double>& scales,
                                 const SweepOptions&        opts,
                                 const std::vector<Color>&  colors) const {
    const int N = (int)polyline.size();
    const int P = (int)polygon.size();

    if (N < 2)  throw std::runtime_error("polyline must have >= 2 points");
    if (P < 3)  throw std::runtime_error("polygon must have >= 3 vertices");
    if ((int)scales.size() != N)
        throw std::runtime_error("scales.size() must equal polyline.size()");
    if (opts.bendSteps < 1)
        throw std::runtime_error("bendSteps must be >= 1");
    const bool hasColors = !colors.empty();
    if (hasColors && (int)colors.size() != N)
        throw std::runtime_error("colors.size() must equal polyline.size()");

    const bool doNormals = opts.generateNormals;
    const bool smooth    = doNormals && opts.smoothNormals;
    const bool doUVs     = opts.generateUVs;

    // Max polygon radius for cap UV normalisation
    double polyRadius = 1e-15;
    for (const Vec3& p : polygon)
        polyRadius = std::max(polyRadius, std::sqrt(p.x*p.x + p.y*p.y));

    Mesh mesh;

    // ---- Per-ring build records ----------------------------------------
    // Each ring stores vertex indices, smooth-normal indices (may be empty),
    // the base index into mesh.uvs (P+1 entries, seam-closed), and its
    // world-space centre for arc-length V tracking.
    struct RingRecord {
        std::vector<int> verts;
        std::vector<int> normIdx;  // only used for smooth shading
        int  uvBase  = 0;
        Vec3 center;
    };
    std::vector<RingRecord> rings;
    std::vector<Color>      ringColor;   // one entry per ring, for cap centres

    double vCurrent  = 0.0;
    Vec3   prevCenter = polyline[0];

    // Helper: compute outward smooth normal for polygon vertex j in frame f
    auto smoothNormal = [&](const Vec3& xA, const Vec3& yA, int j) -> Vec3 {
        return normalize(xA * polygon[j].x + yA * polygon[j].y);
    };

    // Helper: append P+1 UV entries (u = j/P, v = vCurrent) for one ring
    auto pushRingUVs = [&]() -> int {
        int base = (int)mesh.uvs.size();
        if (doUVs)
            for (int j = 0; j <= P; j++)
                mesh.uvs.push_back({ (double)j / P, vCurrent });
        return base;
    };

    // Helper: add a complete ring (verts + optional smooth normals + UVs)
    // using an already-placed frame and a per-vertex normal function.
    auto addRing = [&](const Frame& f, double scale,
                       const Color* col,
                       const Vec3& nxAxis, const Vec3& nyAxis) -> RingRecord {
        RingRecord rec;
        rec.center = f.origin;
        rec.verts  = addRingVerts(mesh, f, polygon, scale, col);
        if (smooth) {
            rec.normIdx.reserve(P);
            for (int j = 0; j < P; j++) {
                rec.normIdx.push_back((int)mesh.normals.size());
                mesh.normals.push_back(smoothNormal(nxAxis, nyAxis, j));
            }
        }
        rec.uvBase = pushRingUVs();
        return rec;
    };

    auto nodeColor = [&](int i) -> const Color* {
        return hasColors ? &colors[i] : nullptr;
    };

    // ---- First ring ----------------------------------------------------
    Frame frame = initialFrame(polyline[0], polyline[1] - polyline[0]);
    rings.push_back(addRing(frame, scales[0], nodeColor(0),
                            frame.xAxis, frame.yAxis));
    ringColor.push_back(hasColors ? colors[0] : Color{});

    // ---- Walk the polyline ---------------------------------------------
    for (int i = 1; i < N; i++) {
        Vec3 t_in = normalize(polyline[i] - polyline[i-1]);
        frame = transportFrame(frame, polyline[i], t_in);

        if (i < N - 1) {
            // Interior node: check for a bend
            Vec3   t_out     = normalize(polyline[i+1] - polyline[i]);
            double cosA      = std::max(-1.0, std::min(1.0, dot(t_in, t_out)));
            double bendAngle = std::acos(cosA);

            if (bendAngle > 1e-8) {
                Vec3 rotAxis  = normalize(cross(t_in, t_out));
                Vec3 xAxis_out = rotateAround(frame.xAxis, rotAxis, bendAngle);
                Vec3 yAxis_out = rotateAround(frame.yAxis, rotAxis, bendAngle);

                // Inner polygon vertex (concave side)
                int innerIdx = findInnerVertex(polygon, {
                                                          dot(t_in + t_out, frame.xAxis),
                                                          dot(t_in + t_out, frame.yAxis),
                                                          0.0 });

                // Segment lengths for scale interpolation.
                double segLen_in  = length(polyline[i]   - polyline[i-1]);
                double segLen_out = length(polyline[i+1] - polyline[i]);

                // Inner-vertex offset direction in each frame (⊥ to its tangent).
                Vec3 fvec_in  = frame.xAxis * polygon[innerIdx].x + frame.yAxis * polygon[innerIdx].y;
                Vec3 fvec_out = xAxis_out   * polygon[innerIdx].x + yAxis_out   * polygon[innerIdx].y;

                // Original stable s1/s2 solve: find pull-back distances so the
                // inner polygon vertex coincides in incoming and outgoing frames
                // (using scales[i] for geometry; scale variation handled below).
                Vec3   delta = (fvec_out - fvec_in) * scales[i];
                double sinA_sq = 1.0 - cosA*cosA;
                double s1 = 0.0, s2 = 0.0;
                if (sinA_sq > 1e-10) {
                    s2 = (dot(delta,t_in)*cosA - dot(delta,t_out)) / sinA_sq;
                    s1 = dot(delta,t_in) + s2*cosA;
                }

                Vec3 incoming_ctr = polyline[i] + t_in  * s1;
                Vec3 outgoing_ctr = polyline[i] + t_out * s2;

                // Interpolated scales at the pull-back positions.
                double t_inc      = (segLen_in  > 1e-15) ? std::max(0.0, std::min(1.0, 1.0 + s1/segLen_in )) : 1.0;
                double t_out_frac = (segLen_out > 1e-15) ? std::max(0.0, std::min(1.0, s2/segLen_out)) : 0.0;
                double scale_inc  = scales[i-1] + (scales[i]   - scales[i-1]) * t_inc;
                double scale_out  = scales[i]   + (scales[i+1] - scales[i])   * t_out_frac;

                // Two pivots: inner-vertex world positions on the incoming and
                // outgoing rings respectively.  Blending across the fan ensures
                // both ring boundaries match exactly, with no lateral offset on
                // either side regardless of how much the scale changes.
                Vec3 pivot_in  = incoming_ctr + fvec_in  * scale_inc;
                Vec3 pivot_out = outgoing_ctr + fvec_out * scale_out;

                // Color at incoming_ctr (also interpolated for the pull-back)
                Color incomingColor{};
                const Color* inCol = nullptr;
                if (hasColors) {
                    incomingColor = lerp(colors[i-1], colors[i], t_inc);
                    inCol = &incomingColor;
                }

                // Incoming ring — uses the interpolated scale, not scales[i]
                vCurrent += length(incoming_ctr - prevCenter);
                prevCenter = incoming_ctr;
                Frame inFrame = frame; inFrame.origin = incoming_ctr;
                rings.push_back(addRing(inFrame, scale_inc, inCol,
                                        frame.xAxis, frame.yAxis));
                ringColor.push_back(hasColors ? incomingColor : Color{});

                // Bend fan rings — blend the pivot linearly from pivot_in to pivot_out
                // so that ring 0 exactly matches the incoming ring and ring bendSteps
                // exactly matches the outgoing ring, with no off-axis translation.
                for (int k = 1; k <= opts.bendSteps; k++) {
                    double frac    = (double)k / opts.bendSteps;
                    double angle   = bendAngle * frac;
                    double scale_k = scale_inc + (scale_out - scale_inc) * frac;
                    Vec3 rotX = rotateAround(frame.xAxis, rotAxis, angle);
                    Vec3 rotY = rotateAround(frame.yAxis, rotAxis, angle);
                    Vec3 pivot_k   = pivot_in * (1.0 - frac) + pivot_out * frac;
                    Vec3 fanCtr = pivot_k
                                - rotX * (polygon[innerIdx].x * scale_k)
                                - rotY * (polygon[innerIdx].y * scale_k);

                    vCurrent += length(fanCtr - prevCenter);
                    prevCenter = fanCtr;

                    RingRecord rec;
                    rec.center = fanCtr;
                    rec.verts.reserve(P);
                    if (smooth) rec.normIdx.reserve(P);

                    for (int j = 0; j < P; j++) {
                        Vec3 vRot = pivot_k
                                  + rotX * ((polygon[j].x - polygon[innerIdx].x) * scale_k)
                                  + rotY * ((polygon[j].y - polygon[innerIdx].y) * scale_k);
                        rec.verts.push_back((int)mesh.vertices.size());
                        mesh.vertices.push_back(vRot);
                        if (inCol) mesh.vertexColors.push_back(colors[i]);
                        if (smooth) {
                            rec.normIdx.push_back((int)mesh.normals.size());
                            mesh.normals.push_back(smoothNormal(rotX, rotY, j));
                        }
                    }
                    rec.uvBase = pushRingUVs();
                    rings.push_back(std::move(rec));
                    ringColor.push_back(hasColors ? colors[i] : Color{});
                }

                frame.origin = outgoing_ctr;
                frame.xAxis  = xAxis_out;
                frame.yAxis  = yAxis_out;
                frame.zAxis  = t_out;

            } else {
                // Straight: one ring at this node
                vCurrent += length(polyline[i] - prevCenter);
                prevCenter = polyline[i];
                rings.push_back(addRing(frame, scales[i], nodeColor(i),
                                        frame.xAxis, frame.yAxis));
                ringColor.push_back(hasColors ? colors[i] : Color{});
            }
        } else {
            // Last node
            vCurrent += length(polyline[i] - prevCenter);
            prevCenter = polyline[i];
            rings.push_back(addRing(frame, scales[i], nodeColor(i),
                                    frame.xAxis, frame.yAxis));
            ringColor.push_back(hasColors ? colors[i] : Color{});
        }
    }

    // ---- Connect consecutive rings with quad strips --------------------
    const int R = (int)rings.size();
    for (int r = 0; r+1 < R; r++) {
        const auto& r0 = rings[r];
        const auto& r1 = rings[r+1];
        for (int j = 0; j < P; j++) {
            int jn = (j+1) % P;
            // Vertex triangles
            int va0=r0.verts[j], vb0=r0.verts[jn];
            int va1=r1.verts[j], vb1=r1.verts[jn];
            mesh.triangles.push_back({va0, vb0, va1});
            mesh.triangles.push_back({vb0, vb1, va1});

            // Smooth normal triangles (same indices as vertices)
            if (smooth) {
                int na0=r0.normIdx[j], nb0=r0.normIdx[jn];
                int na1=r1.normIdx[j], nb1=r1.normIdx[jn];
                mesh.normalTris.push_back({na0, nb0, na1});
                mesh.normalTris.push_back({nb0, nb1, na1});
            }

            // UV triangles — use P+1 entries so the seam column has u=1
            if (doUVs) {
                int u0=r0.uvBase+j,   u0n=r0.uvBase+j+1;
                int u1=r1.uvBase+j,   u1n=r1.uvBase+j+1;
                mesh.uvTris.push_back({u0, u0n, u1});
                mesh.uvTris.push_back({u0n, u1n, u1});
            }
        }
    }

    // ---- End caps (always flat-shaded) ---------------------------------
    if (opts.closeEnds) {
        // Tangent directions for the two caps
        Vec3 t_start = normalize(polyline[1]   - polyline[0]);
        Vec3 t_end   = normalize(polyline[N-1] - polyline[N-2]);

        // Cap UV layout: P entries for ring vertices, then one for the centre.
        // Uses a circular disc projection from the polygon's local XY.
        auto pushCapUVs = [&](bool /*unused*/) -> int {
            int base = (int)mesh.uvs.size();
            if (doUVs) {
                for (int j = 0; j < P; j++)
                    mesh.uvs.push_back({ 0.5 + 0.5*polygon[j].x/polyRadius,
                                        0.5 + 0.5*polygon[j].y/polyRadius });
                mesh.uvs.push_back({0.5, 0.5});  // centre at index P
            }
            return base;
        };

        auto buildCap = [&](const RingRecord& ring, const Color& col,
                            const Vec3& flatNormal, bool flipWinding) {
            // One flat normal shared by all cap triangles
            int ni = doNormals ? (int)mesh.normals.size() : -1;
            if (doNormals) mesh.normals.push_back(flatNormal);

            int capUVBase = pushCapUVs(false);

            // Centre vertex
            Vec3 ctr{};
            for (int idx : ring.verts) ctr += mesh.vertices[idx];
            ctr = ctr / (double)P;
            int ci = (int)mesh.vertices.size();
            mesh.vertices.push_back(ctr);
            if (hasColors) mesh.vertexColors.push_back(col);

            for (int j = 0; j < P; j++) {
                int jn = (j+1) % P;
                Triangle vTri = flipWinding
                                    ? Triangle{ring.verts[j], ci, ring.verts[jn]}
                                    : Triangle{ring.verts[j], ring.verts[jn], ci};
                mesh.triangles.push_back(vTri);

                if (doNormals)
                    mesh.normalTris.push_back({ni, ni, ni});

                if (doUVs) {
                    // UV: (ring[j], centre, ring[jn]) or flipped
                    Triangle uTri = flipWinding
                                        ? Triangle{capUVBase+j, capUVBase+P, capUVBase+jn}
                                        : Triangle{capUVBase+j, capUVBase+jn, capUVBase+P};
                    mesh.uvTris.push_back(uTri);
                }
            }
        };

        buildCap(rings.front(), ringColor.front(), -t_start, /*flip*/true);
        buildCap(rings.back(),  ringColor.back(),   t_end,   /*flip*/false);
    }

    // ---- Flat shading: replace smooth normals with per-face normals ----
    // Caps were already added with their correct flat normals above, so we
    // recompute everything from geometry (which naturally gives flat cap
    // normals too — the cross product of two cap edges equals ±tangent).
    if (doNormals && !opts.smoothNormals) {
        mesh.normals.clear();
        mesh.normalTris.clear();
        for (const Triangle& t : mesh.triangles) {
            Vec3 v0 = mesh.vertices[t.a];
            Vec3 v1 = mesh.vertices[t.b];
            Vec3 v2 = mesh.vertices[t.c];
            Vec3 n  = normalize(cross(v1-v0, v2-v0));
            int ni  = (int)mesh.normals.size();
            mesh.normals.push_back(n);
            mesh.normalTris.push_back({ni, ni, ni});
        }
    }

    return mesh;
}

} // namespace PolylineSweep

#endif // POLYLINESWEEP_H
