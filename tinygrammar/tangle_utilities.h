//
//  tangle_utilities.hpp
//  tinygrammar
//
//  Created by tangles on 6/22/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef tangle_utilities_h
#define tangle_utilities_h

#include "common.h"

inline ym_vec2r orthogonal(const ym_vec2r& x) { return {x.y,-x.x}; }
inline ym_frame2r frame_from_x(const ym_vec2r& o, const ym_vec2r& x) { auto f = ym_identity_frame2r; f.o = o; f.x = ym_normalize(x); f.y = orthogonal(f.x); return f; }
inline ym_frame2r rotate(const ym_vec2r& o, const double angle) { return frame_from_x(o, ym_vec2r(cos(angle * ym_pi / 180.0), sin(angle * ym_pi / 180.0))); }

// |=====================================|
// |======== POLYLINES METHODS ==========|
// |=====================================|

inline bool closed_polyline(const polyline2r& curve) {
    return curve.front() == curve.back();
}

inline polyline2r close_polyline(const polyline2r& curve) {
    if(closed_polyline(curve)) return curve;
    else return curve + curve[0];
}

inline polyline2r remove_duplicates_polyline(const polyline2r& curve) {
    if(curve.empty()) return curve;
    auto cleaned = polyline2r({curve.front()});
    for(auto&& p : curve) if(not (p == cleaned.back())) cleaned += p;
    return cleaned;
}

inline polyline2r make_polyline_segment(const ym_vec2r& a, const ym_vec2r& b) {
    return {a,b};
}

inline polyline2r make_polyline_segment(const ym_vec2r& a, const ym_vec2r& b, double dist, bool skip_last = false) {
    auto steps = round(ym_length(a-b) / dist);
    auto curve = polyline2r();
    for(auto i : range(steps)) {
        auto t = double(i) / double(steps);
        curve += a*(1-t)+b*t;
    }
    if(not skip_last) curve += b;
    return curve;
}

inline polyline2r make_polyline_rect(const ym_vec2r& a, const ym_vec2r& b) {
    return { {
        {min(a.x,b.x),min(a.y,b.y)},
        {max(a.x,b.x),min(a.y,b.y)},
        {max(a.x,b.x),max(a.y,b.y)},
        {min(a.x,b.x),max(a.y,b.y)},
        {min(a.x,b.x),min(a.y,b.y)},
    } };
}

inline polyline2r make_polyline_rect(const ym_vec2r& a, const ym_vec2r& b, double dist) {
    auto points = make_polyline_rect(a,b);
    return
    make_polyline_segment(points[0],points[1],dist,true) +
    make_polyline_segment(points[1],points[2],dist,true) +
    make_polyline_segment(points[2],points[3],dist,true) +
    make_polyline_segment(points[3],points[4],dist,true) +
    points[4];
}

inline polyline2r make_polyline_circle(const ym_vec2r& c, double radius, double dist) {
    auto steps = round(2*ym_pif*radius / dist);
    auto curve = polyline2r();
    for(auto i : range(steps)) {
        auto theta = 2 * ym_pif * double(i) / double(steps);
        curve += c + radius * ym_vec2r{cos(theta),sin(theta)};
    }
    curve = close_polyline(curve);
    return curve;
}

static polyline2r make_line(const ym_vec2r& a, const ym_vec2r& b) {
    return make_polyline_segment(a, b, resolution);
}

template<typename T>
inline polyline2r make_polyline(const polyline2r& curve, const T& f) {
    auto nc = polyline2r(curve.size());
    for(auto i : range(curve)) nc[i] = f(curve[i]);
    return nc;
}

template<typename A, typename T>
inline polyline2r make_polyline(const vector<A>& points, const T& f) {
    auto nc = polyline2r(points.size());
    for(auto i : range(points)) nc[i] = f(points[i]);
    return nc;
}

inline polyline2r transform_polyline(const ym_frame2r& frame, const polyline2r& curve) {
    return make_polyline(curve,[&](const ym_vec2r& p){ return ym_transform_point(frame, p); });
}
inline polyline2r transform_polyline_inverse(const ym_frame2r& frame, const polyline2r& curve) {
    return make_polyline(curve,[&](const ym_vec2r& p){ return ym_transform_point(frame, p); });
}

inline vector<polyline2r> transform_polylines(const ym_frame2r& frame, const vector<polyline2r>& curves) {
    return make_vector(curves, [&](const polyline2r& curve){ return transform_polyline(frame,curve); });
}
inline vector<polyline2r> transform_polylines_inverse(const ym_frame2r& frame, const vector<polyline2r>& curves) {
    return make_vector(curves, [&](const polyline2r& curve){ return transform_polyline_inverse(frame,curve); });
}

inline polyline2r reverse_polyline(const polyline2r& curve) {
    auto ret = curve; std::reverse(ret.begin(), ret.end()); return ret;
}

inline double length_polyline(const polyline2r& curve) {
    auto l = 0.0f;
    for(auto i : range((int)curve.size()-1)) l += ym_length(curve[i]-curve[i+1]);
    return l;
}

inline ym_range2r make_range2r(const polyline2r& points) { auto bbox = ym_range2r(); for(auto& p : points) bbox = ym_rexpand(bbox, p); return bbox; }

inline ym_range2r bounds_polyline(const polyline2r& curve) { return make_range2r(curve); }
inline ym_range2r bounds_polylines(const vector<polyline2r>& curves) {
    auto bbox = ym_range2r();
    for(auto& curve : curves) bbox = ym_rexpand(bbox, bounds_polyline(curve));
    return bbox;
}

inline polyline2r clenup_polyline(const polyline2r& curve) {
    for(auto i : range((int)curve.size()-1)) {
        if(curve[i] == curve[i+1]) printf("bad");
    }
    return curve;
}

// uses even-odd rule
inline bool inside_polylines(const vector<polyline2r>& curves, const ym_vec2r& p) {
    auto flips = false;
    for(auto&& curve : curves) {
        for(auto i : range((int)curve.size()-1)) {
            if ( ((curve[i].y>p.y) != (curve[i+1].y>p.y)) &&
                (p.x < (curve[i].x-curve[i+1].x) * (p.y-curve[i].y) /
                 (curve[i+1].y-curve[i].y) + curve[i].x) )
                flips = not flips;
        }
    }
    return flips;
}

inline bool inside_polyline(const polyline2r& curve, const ym_vec2r& p) {
    return inside_polylines({curve}, p);
}

// only for closed ones
inline double area_polyline(const polyline2r& curve) {
    auto area = 0.0f;
    for(auto i : range((int)curve.size()-1)) area += curve[i].x*curve[i+1].y + curve[i+1].x*curve[i].y;
    return area/2;
}

polyline2r tangents_polyline(const polyline2r& curve, bool use_both = false);
polyline2r normals_polyline(const polyline2r& curve, bool flipped = false, bool use_both = false);
polyline2r perturb_polyline(const polyline2r& curve, double scale, double strength, rng& rn, bool ortho);


// |=====================================|
// |========= POLYGONS METHODS ==========|
// |=====================================|


template<typename T>
inline polygon2r make_polygon(const polygon2r& poly, const T& f) {
    auto nc = polygon2r(poly.size());
    for(auto i : range(poly)) nc[i] = f(poly[i]);
    return nc;
}

template<typename A, typename T>
inline polygon2r make_polygon(const vector<A>& curves, const T& f) {
    auto nc = polygon2r(curves.size());
    for(auto i : range(curves)) nc[i] = f(curves[i]);
    return nc;
}

inline ym_range2r bounds_polygon(const polygon2r& poly) { return bounds_polyline(poly[0]); }

inline ym_range2r bounds_polygons(const vector<polygon2r>& polys) {
    auto bbox = ym_range2r();
    for(auto&& poly : polys) bbox = ym_rexpand(bbox,bounds_polygon(poly));
    return bbox;
}

inline ym_vec2r get_centroid(const polygon2r& poly) {
    auto res_p = ym_vec2r();
    for(auto&& p : poly[0]) res_p += p;
    res_p.x = res_p.x / poly[0].size();
    res_p.y = res_p.y / poly[0].size();
    return res_p;
}

inline polygon2r transform_polygon(const ym_frame2r& frame, const polygon2r& poly) {
    return make_polygon(poly, [&](const polyline2r& curve){ return transform_polyline(frame, curve); });
}
inline polygon2r transform_polygon_inverse(const ym_frame2r& frame, const polygon2r& poly) {
    return make_polygon(poly, [&](const polyline2r& curve){ return transform_polyline_inverse(frame, curve); });
}

// uses even-odd rule
inline bool inside_polygon(const polygon2r& poly, const ym_vec2r& p) { return inside_polylines(poly, p); }
inline bool inside_polygons(const vector<polygon2r>& polys, const ym_vec2r& p) {
    for(auto&& poly : polys) if(inside_polylines(poly, p)) return true;
    return false;
}


inline polygon2r perturb_polygon(const polygon2r& poly, double scale, double strength, rng& rn, bool ortho = false) {
    return make_polygon(poly,[&](const polyline2r& curve){ return perturb_polyline(curve, scale, strength, rn, ortho); });
}

// |=====================================|
// |======= PERTURBATION METHODS ========|
// |=====================================|

struct RuleUtils {
    static constexpr double          resolution = 2;
    static constexpr double          perturb_scale = 100;
    static constexpr double          perturb_strength = 5;
    
    static polyline2r perturb(const polyline2r& curve, rng& rn);
    static polygon2r perturb(const polygon2r& poly, rng& rn);
    static vector<polyline2r> perturb(const vector<polyline2r>& curves, rng& rn);
    static vector<polygon2r> perturb(const vector<polygon2r>& polys, rng& rn);
};

#endif /* tangle_utilities_h */
