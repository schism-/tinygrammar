//
//  clipper_methods.cpp
//  tinygrammar
//
//  Created by Christian on 20/09/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "clipper_methods.h"

inline clipper_point to_clipper_point(const ym_vec2r& p) { return clipper_point((p.x*clipper_scale),-(p.y*clipper_scale)); }

inline clipper_path to_clipper_path(const polyline2r& curve) {
    if(closed_polyline(curve)) {
        auto ccurve = clipper_path();
        for(auto i : range((int)curve.size()-1)) ccurve.push_back(to_clipper_point(curve[i]));
        return ccurve;
    }
    else return make_vector(curve, to_clipper_point);
}

inline clipper_paths to_clipper_paths(const polygon2r& poly) {
    return make_vector<polyline2r>(poly, to_clipper_path);
}

inline ym_vec2r from_clipper_point(const clipper_point& p) { return {p.X/clipper_scale,-p.Y/clipper_scale}; }

inline polyline2r from_clipper_path(const clipper_path& curve, bool closed) {
    if(closed) return make_polyline(curve, from_clipper_point) + from_clipper_point(curve[0]);
    else return make_polyline(curve, from_clipper_point);
}

inline vector<polyline2r> from_clipper_paths(const clipper_paths& paths, bool closed) {
    return make_vector(paths, [&](const clipper_path& path){return from_clipper_path(path,closed);});
}

inline void from_clipper_node(const clipper_node* node, vector<polygon2r>& ret) {
    ret += { from_clipper_path(node->Contour,true) };
    for(auto&& hole : node->Childs) {
        ret.back() += from_clipper_path(hole->Contour,true);
        for(auto&& inner : hole->Childs) from_clipper_node(inner, ret);
    }
}

inline vector<polygon2r> from_clipper_tree(const clipper_tree& tree, bool flip) {
    auto ret = vector<polygon2r>();
    for(auto&& node : tree.Childs) from_clipper_node(node, ret);
    if(flip) for(auto& poly : ret) for(auto& curve : poly) curve = reverse_polyline(curve);
    return ret;
}

bool inside_polygon_clipper(const polygon2r& poly, const ym_vec2r& p) {
    if(not ClipperLib::PointInPolygon(to_clipper_point(p), to_clipper_path(poly[0])))return false;
    if ((int)poly.size() > 1){
        for(auto i : range(1,(int)poly.size()-1)) {
            if(ClipperLib::PointInPolygon(to_clipper_point(p), to_clipper_path(poly[i])))return false;
        }
    }
    return true;
}

vector<polygon2r> intersect_polygons(const polygon2r& a, const polygon2r& b) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctIntersection, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}

vector<polygon2r> intersect_polygons(const vector<polygon2r>& as, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    for(auto&& a : as) { for(auto&& al : a) clipper.AddPath(to_clipper_path(al), ClipperLib::ptSubject, closed_polyline(al)); }
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctIntersection, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}



vector<polygon2r> subtract_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctDifference, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}

vector<polygon2r> subtract_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    for(auto&& a : as) clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctDifference, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}



vector<polygon2r> union_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctUnion, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}

vector<polygon2r> union_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    for(auto&& a : as) clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctUnion, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}



vector<polygon2r> xor_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctXor, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}

vector<polygon2r> xor_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs) {
    auto clipper = ClipperLib::Clipper();
    clipper.PreserveCollinear(true);
    for(auto&& a : as) clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
    for(auto&& b : bs) clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
    auto tree = ClipperLib::PolyTree();
    if(not clipper.Execute(ClipperLib::ctXor, tree, ClipperLib::pftNonZero)) return {};
    return from_clipper_tree(tree,false);
}



vector<polygon2r> outline_polygon_clipper(const polygon2r& poly, double offset) {
    auto clipper = ClipperLib::ClipperOffset();
    clipper.AddPaths(to_clipper_paths(poly), ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
    auto tree = ClipperLib::PolyTree();
    clipper.Execute(tree, offset*clipper_scale);
    return from_clipper_tree(tree,true);
}