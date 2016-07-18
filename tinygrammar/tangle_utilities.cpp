//
//  tangle_utilities.cpp
//  tinygrammar
//
//  Created by tangles on 6/22/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "tangle_utilities.h"

//#include "common.h"

//const auto clipper_scale = (double)100000.0;
//using clipper_point = ClipperLib::IntPoint;
//using clipper_path = ClipperLib::Path;
//using clipper_paths = ClipperLib::Paths;
//using clipper_tree = ClipperLib::PolyTree;
//using clipper_node = ClipperLib::PolyNode;
//
//inline clipper_point to_clipper_point(const ym_vec2r& p) { return clipper_point((p.x*clipper_scale),-(p.y*clipper_scale)); }
//
//inline clipper_path to_clipper_path(const polyline2r& curve) {
//    if(closed_polyline(curve)) {
//        auto ccurve = clipper_path();
//        for(auto i : range((int)curve.size()-1)) ccurve.push_back(to_clipper_point(curve[i]));
//        return ccurve;
//    }
//    else return make_vector(curve, to_clipper_point);
//}
//inline clipper_paths to_clipper_paths(const polygon2r& poly) {
//    return make_vector<polyline2r>(poly, to_clipper_path);
//}
//
//inline ym_vec2r from_clipper_point(const clipper_point& p) { return {p.X/clipper_scale,-p.Y/clipper_scale}; }
//
//inline polyline2r from_clipper_path(const clipper_path& curve, bool closed) {
//    if(closed) return make_polyline(curve, from_clipper_point) + from_clipper_point(curve[0]);
//    else return make_polyline(curve, from_clipper_point);
//}
//
//inline void from_clipper_node(const clipper_node* node, vector<polygon2r>& ret) {
//    ret += { from_clipper_path(node->Contour,true) };
//    for(auto&& hole : node->Childs) {
//        ret.back() += from_clipper_path(hole->Contour,true);
//        for(auto&& inner : hole->Childs) from_clipper_node(inner, ret);
//    }
//}
//
//inline vector<polygon2r> from_clipper_tree(const clipper_tree& tree, bool flip) {
//    auto ret = vector<polygon2r>();
//    for(auto&& node : tree.Childs) from_clipper_node(node, ret);
//    if(flip) for(auto& poly : ret) for(auto& curve : poly) curve = reverse_polyline(curve);
//    return ret;
//}
//
//
//inline vector<polygon2r> intersect_polygons(const polygon2r& a, const polygon2r& b){
//    auto clipper = ClipperLib::Clipper();
//    clipper.PreserveCollinear(true);
//    clipper.AddPaths(to_clipper_paths(a), ClipperLib::ptSubject, true);
//    clipper.AddPaths(to_clipper_paths(b), ClipperLib::ptClip, true);
//    auto tree = ClipperLib::PolyTree();
//    if(not clipper.Execute(ClipperLib::ctIntersection, tree, ClipperLib::pftNonZero)) return {};
//    return from_clipper_tree(tree,false);
//    return vector<polygon2r>();
//}