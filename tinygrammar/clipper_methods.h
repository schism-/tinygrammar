//
//  clipper_methods.h
//  tinygrammar
//
//  Created by Christian on 19/09/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef clipper_methods_h
#define clipper_methods_h

#include "../external/clipper/clipper.hpp"
#include "tangle_utilities.h"

const auto clipper_scale = (double)100000.0;

using clipper_point = ClipperLib::IntPoint;
using clipper_path = ClipperLib::Path;
using clipper_paths = ClipperLib::Paths;
using clipper_tree = ClipperLib::PolyTree;
using clipper_node = ClipperLib::PolyNode;

inline clipper_point to_clipper_point(const ym_vec2r& p);

inline clipper_path to_clipper_path(const polyline2r& curve);

inline clipper_paths to_clipper_paths(const polygon2r& poly);

inline ym_vec2r from_clipper_point(const clipper_point& p);

inline polyline2r from_clipper_path(const clipper_path& curve, bool closed);

inline vector<polyline2r> from_clipper_paths(const clipper_paths& paths, bool closed);

inline void from_clipper_node(const clipper_node* node, vector<polygon2r>& ret);

inline vector<polygon2r> from_clipper_tree(const clipper_tree& tree, bool flip);

bool inside_polygon_clipper(const polygon2r& poly, const ym_vec2r& p);

vector<polygon2r> intersect_polygons(const polygon2r& a, const polygon2r& b);
vector<polygon2r> intersect_polygons(const vector<polygon2r>& as, const vector<polygon2r>& bs);

vector<polygon2r> subtract_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs);
vector<polygon2r> subtract_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs);

vector<polygon2r> union_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs);
vector<polygon2r> union_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs);

vector<polygon2r> xor_polygons_clipper(const polygon2r& a, const vector<polygon2r>& bs);
vector<polygon2r> xor_polygons_clipper(const vector<polygon2r>& as, const vector<polygon2r>& bs);

vector<polygon2r> outline_polygon_clipper(const polygon2r& poly, double offset);

#endif /* clipper_methods_h */
