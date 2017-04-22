//
//  operator.cpp
//  tinygrammar
//
//  Created by tangles on 6/21/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "operator.h"
#include "clipper_methods.h"
#include "grammar_core.h"

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
    auto g = get_grammar(grammar_filename);
    for(auto&& shape : shapes) {
        auto temp = (TimeSliceShape*)shape;
        auto new_slices = vector<TimeManager::TimeSlice*>();
        if (is_tag_invert(g, temp->slice->ts_tag)){
            auto temp_v = vector<double>();
            auto new_params = rule_params();
            for(auto i = 0 ; i < PARAM_SIZE; i++){
                if (parameters[i] != INFINITY) temp_v.insert(temp_v.begin(), parameters[i]);
                else temp_v.push_back(INFINITY);
            }
            for(auto i = 0 ; i < PARAM_SIZE; i++) new_params[i] = temp_v[i];
            
            auto t_temp_v = vector<int>();
            auto new_tags = rule_tags();
            for(auto i = 0 ; i < TAG_SIZE; i++){
                if (tags[i] != 0) t_temp_v.insert(t_temp_v.begin(), tags[i]);
                else t_temp_v.push_back(0);
            }
            for(auto i = 0 ; i < TAG_SIZE; i++) {
                if (t_temp_v[i] == 0) new_tags[i] = t_temp_v[i];
                else new_tags[i] = invert_tag(g, t_temp_v[i]);
            }
            
            new_slices = TimeManager::TimeSliceCut(timeline, temp->slice, new_params, new_tags);
        }
        else{
            new_slices = TimeManager::TimeSliceCut(timeline, temp->slice, parameters, tags);
        }
        
        for (auto&& ns : new_slices){
            children.push_back(new TimeSliceShape(ns));
        }
    }
    return children;
}

// |============================|
// |      SPACE OPERATORS       |
// |============================|

ym_range2r compute_bbox(const vector<AnimatedShape*>& anim_shapes){
    ym_range2r bbox;
    bbox = bounds_polygons(make_vector(anim_shapes, [&](AnimatedShape* as){return as->poly;}));
    bbox = ym_range2r({-3000.0, -3000.0}, {3000.0, 3000.0});
    return bbox;
}

vector<pair<TimeSliceShape*, TimeManager::NodeTimeLine*>> get_data(Grammar* g, TimeManager::TimeLine* timeline, const ShapeGroup& shapes,
                                                                   double offset, double& n_ma_d, double& i_ma_d){

    double n_m_d = 10000.0, i_m_d = 10000.0;
    int n_c = 0, i_c = 0;
    int n_tid_max = 0, i_tid_max = 0;
    auto data = vector<pair<TimeSliceShape*, TimeManager::NodeTimeLine*>>();
    data.reserve(shapes.size());
    for (auto&& s : shapes){
        auto temp = (TimeSliceShape*)s;
        auto ntl = TimeManager::FindTimeLine(timeline, temp->slice);
        if (is_tag_invert(g, temp->slice->ts_tag)) { i_c++; i_m_d = min(i_m_d, temp->slice->duration); i_tid_max = max(i_tid_max, ntl->node->shapes[0]->tid);}
        else { n_c++; n_m_d = min(n_m_d, temp->slice->duration); n_tid_max = max(n_tid_max, ntl->node->shapes[0]->tid);}
        data.push_back(make_pair(temp, ntl));
    }
//    n_ma_d = max(0.0, n_m_d - offset * n_c);
//    i_ma_d = max(0.0, i_m_d - offset * i_c);
    n_ma_d = max(0.0, n_m_d - offset * (n_tid_max + 1));
    i_ma_d = max(0.0, i_m_d - offset * (i_tid_max + 1));

    return data;
}


ShapeGroup affine_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline){
    auto g = get_grammar(grammar_filename);
    auto children = ShapeGroup();
    
    auto offset = parameters[7];
    auto n_off_count = 1, i_off_count = 1;
    auto n_max_dur = 0.0, i_max_dur = 0.0;
    auto data = get_data(g, timeline, shapes, offset, n_max_dur, i_max_dur);
    
    auto anim_shapes = vector<AnimatedShape*>();
    for (auto&& ntl : data) anim_shapes.insert(anim_shapes.end(), ntl.second->node->shapes.begin(), ntl.second->node->shapes.end());
    auto bbox = compute_bbox(anim_shapes);
    
    children.reserve(data.size());
    for(auto&& d : data) {
        auto am  = AnimatorMatrix();
        double start_delta, end_delta;
        
        if (is_tag_invert(g, d.first->slice->ts_tag)){
            auto mat = ym_affine<double, 3, false>();
            mat.x = {parameters[1], parameters[2], 0.0};
            mat.y = {parameters[3], parameters[4], 0.0};
            mat.z = {parameters[5], parameters[6], 1.0};
            mat = ym_inverse(mat);
//            am  = AnimatorMatrix(bbox, {{mat.x.x, mat.x.y},{mat.y.x, mat.y.y}, {mat.z.x, mat.z.y}});
            am  = AnimatorMatrix(bbox, {{mat.x.x, mat.x.y},{mat.y.x, mat.y.y}, {-parameters[5], -parameters[6]}});

            start_delta = (offset * d.second->node->shapes[0]->tid) / d.first->slice->duration;
            end_delta = (offset * d.second->node->shapes[0]->tid + i_max_dur) / d.first->slice->duration;
            i_off_count++;
        }
        else {
            am  = AnimatorMatrix(bbox, {{parameters[1], parameters[2]},{parameters[3], parameters[4]}, {parameters[5], parameters[6]}});
            start_delta = (offset * d.second->node->shapes[0]->tid) / d.first->slice->duration;
            end_delta = (offset * d.second->node->shapes[0]->tid + n_max_dur) / d.first->slice->duration;
            n_off_count++;
        }
        
        if (offset == 0.0) { start_delta = 0.0; end_delta = 1.0; }
        
        d.first->slice->animation = AnimatorKeyframes(am, {start_delta, end_delta}, parameters[0] == 1.0 ? anim_single : anim_group, 0.0);;
        d.first->slice->ts_tag = tags[0];
        static int counter = 0;
        counter++;
        children.push_back(d.first);
    }
    return children;
}

ShapeGroup move_towards_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline){
    auto g = get_grammar(grammar_filename);
    auto children = ShapeGroup();
    
    auto offset = parameters[4];
    auto n_off_count = 1, i_off_count = 1;
    auto n_max_dur = 0.0, i_max_dur = 0.0;
    auto data = get_data(g, timeline, shapes, offset, n_max_dur, i_max_dur);
    
    auto anim_shapes = vector<AnimatedShape*>();
    for (auto&& ntl : data) anim_shapes.insert(anim_shapes.end(), ntl.second->node->shapes.begin(), ntl.second->node->shapes.end());
    auto bbox = compute_bbox(anim_shapes);
    
    for(auto&& d : data) {
        auto am  = AnimatorMatrix();
        double start_delta, end_delta;
        
        if (is_tag_invert(g, d.first->slice->ts_tag)){
            if (parameters[0] == 1.0) {
                auto c = ym_rcenter(bounds_polygon(d.second->node->shapes[0]->poly));
                am  = move_towards_point(bbox, {c.x + parameters[1], c.y + parameters[2]}, -parameters[3]);
            }
            else am  = move_towards_point(bbox, {parameters[1], parameters[2]}, -parameters[3]);
            start_delta = (offset * d.second->node->shapes[0]->tid) / d.first->slice->duration;
            end_delta = (offset * d.second->node->shapes[0]->tid + i_max_dur) / d.first->slice->duration;
            i_off_count++;
        }
        else {
            if (parameters[0] == 1.0) {
                auto c = ym_rcenter(bounds_polygon(d.second->node->shapes[0]->poly));
                am  = move_towards_point(bbox, {c.x + parameters[1], c.y + parameters[2]}, parameters[3]);
            }
            else am  = move_towards_point(bbox, {parameters[1], parameters[2]}, parameters[3]);
            start_delta = (offset * d.second->node->shapes[0]->tid) / d.first->slice->duration;
            end_delta = (offset * d.second->node->shapes[0]->tid + n_max_dur) / d.first->slice->duration;
            n_off_count++;
        }
        
        if (offset == 0.0) { start_delta = 0.0; end_delta = 1.0; }
        
        auto akf = AnimatorKeyframes(am, {start_delta, end_delta}, anim_group, 0.0);
        
        d.first->slice->animation = akf;
        d.first->slice->ts_tag = tags[0];
        children.push_back(d.first);
    }
    return children;
}

ShapeGroup morph_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline){
    auto g = get_grammar(grammar_filename);
    auto children = ShapeGroup();
    int animator_type;
    ym_range2r bbox;
    
    auto data = vector<pair<TimeSliceShape*, TimeManager::NodeTimeLine*>>();
    for (auto&& s : shapes){
        auto temp = (TimeSliceShape*)s;
        data.push_back(make_pair(temp, TimeManager::FindTimeLine(timeline, temp->slice)));
    }
    
    if (parameters[0] == 1.0) animator_type = anim_single;
    else animator_type = anim_group;
    
    bbox = ym_range2r({-3000.0, -3000.0}, {3000.0, 3000.0});
    
    for(auto&& d : data) {
        auto am  = AnimatorMatrix();
        double start_delta, end_delta;
        am  = morph_to_circle(bbox, {parameters[1], parameters[2]}, 10.0);
        start_delta = 0.0;
        end_delta = 1.0;
        
        auto akf = AnimatorKeyframes();
        if (animator_type == anim_single)   akf = AnimatorKeyframes(am, {start_delta, end_delta}, anim_single, 0.0);
        else                                akf = AnimatorKeyframes(am, {start_delta, end_delta}, anim_group, 0.0);
        
        d.first->slice->animation = akf;
        d.first->slice->ts_tag = tags[0];
        children.push_back(d.first);
    }
    return children;
}

ShapeGroup attributes_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline){
    auto children = ShapeGroup();
    
    auto off_count = 1;
    auto g = get_grammar(grammar_filename);
    
    auto bbox = ym_range2r({-100.0, -100.0}, {100.0, 100.0});
    
    for(auto&& s : shapes) {
        auto am  = AnimatorMatrix();
        auto temp = (TimeSliceShape*) s;
        if (is_tag_invert(g, temp->slice->ts_tag)){
            am  = AnimatorMatrix(bbox, {{1.0, 0.0},{0.0, 1.0}, {0.0, 0.0}});
            //put the original values
            am.end_b_color = {(float)parameters[0], (float)parameters[1], (float)parameters[2], (float)parameters[3]};
            am.start_b_color =   {(float)parameters[4], (float)parameters[5], (float)parameters[6], (float)parameters[7]};
            
            am.end_f_color = {(float)parameters[8], (float)parameters[9], (float)parameters[10], (float)parameters[11]};
            am.start_f_color =   {(float)parameters[12], (float)parameters[13], (float)parameters[14], (float)parameters[15]};
            if (parameters[16] == 1.0) am.has_trail = true;
        }
        else {
            am  = AnimatorMatrix(bbox, {{1.0, 0.0},{0.0, 1.0}, {0.0, 0.0}});
            //change attributes here
            am.start_b_color = {(float)parameters[0], (float)parameters[1], (float)parameters[2], (float)parameters[3]};
            am.end_b_color =   {(float)parameters[4], (float)parameters[5], (float)parameters[6], (float)parameters[7]};
            
            am.start_f_color = {(float)parameters[8], (float)parameters[9], (float)parameters[10], (float)parameters[11]};
            am.end_f_color =   {(float)parameters[12], (float)parameters[13], (float)parameters[14], (float)parameters[15]};
            if (parameters[16] == 1.0) am.has_trail = true;
        }
        
        auto akf = AnimatorKeyframes(am, {0.0, 1.0}, anim_attribute, parameters[17] * off_count);
        
        temp->slice->animation = akf;
        temp->slice->ts_tag = tags[0];
        off_count++;
        children.push_back(temp);
    }
    return children;
}


