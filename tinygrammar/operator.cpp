//
//  operator.cpp
//  tinygrammar
//
//  Created by tangles on 6/21/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "operator.h"
#include "clipper_methods.h"

ShapeGroup _split_shapes(const vector<polyline2r>& curves, Shape* shape, int gid, const ym_frame2r& shape_frame, rule_tags tags) {
    auto children = ShapeGroup();
    for(auto i = 0; i < (int)curves.size() - 1; i++) {
        auto inters = intersect_polygons(((TangleShape*)shape)->poly, {curves[i] + reverse_polyline(curves[i+1]) + curves[i][0]});
        
        for(auto&& inter : inters)
            children.push_back(new TangleShape(tags[0], gid, i, shape_frame, {inter}));
    }
    return children;
}

vector<polyline2r> _curves(const polygon2r& poly, const ym_frame2r& frame, rule_params parameters, rng& rn) {
    auto bbox = bounds_polygon(transform_polygon_inverse(frame, poly));
    auto offset = ym_rsize(bbox).y / round(ym_rsize(bbox).y / parameters[0]);
    auto curves = vector<polyline2r>();
    auto curve = make_line({bbox.min.x,0}, {bbox.max.x,0});
    auto cbbox = bounds_polyline(curve);
    
    for(auto& p : curve) p -= {0, cbbox.max.y - bbox.min.y + resolution / 2};
    cbbox = bounds_polyline(curve);
    while(cbbox.min.y <= bbox.max.y + resolution/2) {
        curves.push_back(curve);
        for(auto& p : curve) p += {0,offset};
        cbbox = bounds_polyline(curve);
    }
    
    //curves = RuleUtils::perturb(curves, rn);
    auto bbox_0 = bounds_polyline(curves.front());
    for(auto& p : curves.front()) { p -= {0,(double)fabs(bbox.min.y - bbox_0.max.y)}; }
    auto bbox_1 = bounds_polyline(curves.back());
    for(auto& p : curves.back()) { p += {0,(double)fabs(bbox.max.y - bbox_1.min.y)}; }
    
    for(auto& curve : curves) curve = transform_polyline(frame, curve);
    
    return curves;
}

ShapeGroup tangle_split_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& rn, ShapeGroup* annotations){
    auto children = ShapeGroup();
    auto gid = TangleShape::next_gid();
    
    for(auto&& shape : shapes) {
        auto curves = vector<polyline2r>();
        auto t_shape = (TangleShape*)shape;
        auto shape_frame = t_shape->frame;
        if (parameters[1] != -1) shape_frame = rotate(shape_frame.o, parameters[1]);
        curves = _curves(t_shape->poly, shape_frame, parameters, rn);
        children += _split_shapes(curves, shape, gid, shape_frame, tags);
    }
    return children;
}

ShapeGroup init_operator(rule_tags tags, rule_params parameters, int init_value, rng& rn){
    auto children = ShapeGroup();
    if (parameters[0] == 0){
        // built-in shape
        auto gid = TangleShape::next_gid();
        auto shape = make_polyline_rect(ym_vec2r(- parameters[1] / 2.0, - parameters[1] / 2.0), ym_vec2r(parameters[1] / 2.0, parameters[1] / 2.0));
        auto new_s = new TangleShape(tags[0], gid, 0, ym_frame2r(), {shape});
        children.push_back(new_s);
    }
    else if (parameters[0] == 1){
        //load svg
        printf("Nothing to load at the moment. \n");
    }
    
    return children;
}

// |============================|
// |       TIME OPERATORS       |
// |============================|

ShapeGroup time_init_operator(rule_tags tags, rule_params parameters, int init_value, rng& rn, CSGTree::Tree* tree){
    auto children = ShapeGroup();
    
    if (tree == nullptr) {
        for (auto i = 0; i < (int)parameters[1]; i++){
            auto slice = new TimeSliceShape(new TimeManager::TimeSlice(parameters[0] / parameters[1], tags[0]));
            children.push_back(slice);
        }
    }
    else {
        for (auto&& node : tree->leaves){
            for (auto i = 0; i < (int)parameters[1]; i++){
                auto slice = new TimeSliceShape(new TimeManager::TimeSlice(parameters[0] / parameters[1], tags[0]));
                children.push_back(slice);
            }
            children.push_back(nullptr);
        }
    }
    
    return children;
}

ShapeGroup time_slice_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& rn, TimeManager::TimeLine* timeline){
    auto children = ShapeGroup();
    
    for(auto&& shape : shapes) {
        auto temp = (TimeSliceShape*)shape;
        auto new_slices = TimeManager::TimeSliceCut(timeline, temp->slice, parameters, tags);
        for (auto&& ns : new_slices){
            children.push_back(new TimeSliceShape(ns));
        }
    }
    return children;
}

ShapeGroup affine_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline){
    auto children = ShapeGroup();
    
    int animator_type;
    ym_range2r bbox;
    
    auto data = vector<pair<TimeSliceShape*, TimeManager::NodeTimeLine*>>();
    for (auto&& s : shapes){
        auto temp = (TimeSliceShape*)s;
        data.push_back(make_pair(temp, TimeManager::FindTimeLine(timeline, temp->slice)));
    }
    
    if (parameters[0] == 1.0) {
        animator_type = anim_single;
    }
    else {
        animator_type = anim_group;
    }
    
    auto anim_shapes = vector<AnimatedShape*>();
    for (auto&& ntl : data) anim_shapes.insert(anim_shapes.end(), ntl.second->node->content->shapes.begin(), ntl.second->node->content->shapes.end());
    bbox = bounds_polygons(make_vector(anim_shapes, [&](AnimatedShape* as){return as->poly;}));
    
    bbox = ym_range2r({-10000.0, -10000.0}, {10000.0, 10000.0});
    
    auto off_count = 1;
    for(auto&& d : data) {
        if (animator_type == anim_single){
//            auto am  = AnimatorMatrix(bounds_polygons(make_vector(d.second->node->content->shapes, [&](AnimatedShape* as){return as->poly;})),
//                                      {{parameters[1], parameters[2]},{parameters[3], parameters[4]}, {parameters[5], parameters[6]}});
            auto am  = AnimatorMatrix(bbox, {{parameters[1], parameters[2]},{parameters[3], parameters[4]}, {parameters[5], parameters[6]}});
            auto akf = AnimatorKeyframes(am, 1, anim_single, parameters[7] * off_count);
            d.first->slice->animation = akf;
            d.first->slice->ts_tag = tags[0];
        }
        else {
            auto am  = AnimatorMatrix(bbox, {{parameters[1], parameters[2]},{parameters[3], parameters[4]}, {parameters[5], parameters[6]}});
            auto akf = AnimatorKeyframes(am, 1, anim_group, parameters[7] * off_count);
            d.first->slice->animation = akf;
            d.first->slice->ts_tag = tags[0];
        }
        off_count++;
        children.push_back(d.first);
    }
    return children;
}


