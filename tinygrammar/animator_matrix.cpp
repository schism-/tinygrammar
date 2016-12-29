//
//  animator_matrix.cpp
//  tinygrammar
//
//  Created by Christian on 03/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "animator_matrix.h"

AnimatorMatrix move_towards_point(const ym_range2r& bb, const ym_vec2r& pos, double scale){
    auto res_am = AnimatorMatrix(bb);
    for (auto i = 0; i <= res_am.mats.size(); i++){
        res_am.mats[i].t = ym_normalize(pos - res_am.mats_centers[i]) * scale;
    }
    return res_am;
}

AnimatorMatrix morph_to_circle(const ym_range2r& bb, const ym_vec2r& pos, double scale){
    auto res_am = AnimatorMatrix(bb);
    auto circle = make_polyline_circle({0.0, 0.0}, 25.0, 4.0);
    
    for (auto i = 0; i <= res_am.mats.size(); i++){
        res_am.mats[i].t = pos - res_am.mats_centers[i];
    }
    return res_am;
}

ym_affine2r get_matrix(const AnimatorMatrix& am, const ym_vec2r& pos){
    if (!ym_rinside(am.bounding_box, pos)){
        printf("[AnimatorMatrix] ERROR: position outside the bounding box;");
        return ym_affine2r();
    }
    else{
        auto ins = (pos - am.bounding_box.min) / am.step;
        return am.mats[(int)matrix_resolution * (int)ins.y + (int)ins.x];
    }
}

void set_matrix(AnimatorMatrix am, const ym_affine2r& mat, int x_idx, int y_idx){
    if (x_idx < 0 || y_idx < 0 || x_idx >= matrix_resolution || y_idx >= matrix_resolution){
        printf("[AnimatorMatrix] ERROR: invalid index;");
    }
    else{
        am.mats[(int)matrix_resolution * y_idx + x_idx] = mat;
    }
}

polygon2r transform(const AnimatorMatrix& am, const polygon2r& poly, double incr){
    auto new_shape = polygon2r();
    auto centroid = get_centroid(poly);
    for (auto&& line : poly){
        new_shape += polyline2r();
        for (auto&& p : line){
            auto m = get_matrix(am, p);
            
            auto mat = ym_mat<double, 3, 3>(m.m, {0.0, 0.0});
            auto log_m = ln(mat);
            log_m = log_m * incr;  
            log_m = exp(log_m);
            
            auto mat_t = ym_mat<double, 3, 3>({{1.0, 0.0}, {0.0, 1.0}}, m.t);
            auto log_m_t = ln(mat_t);
            log_m_t = log_m_t * incr;
            log_m_t = exp(log_m_t);
            
            auto incr_aff = ym_affine2r(log_m);
            auto incr_aff_t = ym_affine2r(log_m_t);
            
            auto new_p = ym_transform_point(incr_aff, p - centroid);
            new_p = ym_transform_point(incr_aff_t, new_p);
            
            new_shape.back() += (new_p + centroid);
        }
    }
    return new_shape;
}

polygon2r transform_group(const AnimatorMatrix& am, const polygon2r& poly, double incr){
    auto new_shape = polygon2r();
    auto centroid = get_centroid(poly);
    auto m = get_matrix(am, centroid);
    
    auto mat = ym_mat<double, 3, 3>(m.m, {0.0, 0.0});
    auto log_m = ln(mat);
    log_m = log_m * incr;
    log_m = exp(log_m);
    
    auto mat_t = ym_mat<double, 3, 3>({{1.0, 0.0}, {0.0, 1.0}}, m.t);
    auto log_m_t = ln(mat_t);
    log_m_t = log_m_t * incr;
    log_m_t = exp(log_m_t);
    
    auto incr_aff = ym_affine2r(log_m);
    auto incr_aff_t = ym_affine2r(log_m_t);
    
    for (auto&& line : poly){
        new_shape += polyline2r();
        for (auto&& p : line){
            auto new_p = ym_transform_point(incr_aff, p);
            new_p = ym_transform_point(incr_aff_t, new_p);
            new_shape.back() += new_p;
        }
    }
    return new_shape;
}

AnimatedShape* transform_attributes(const AnimatorMatrix& am, AnimatedShape* shape, double frame){
    shape->border_color = (1.0 - frame) * am.start_b_color + frame * am.end_b_color;
    shape->fill_color   = (1.0 - frame) * am.start_f_color + frame * am.end_f_color;
    return shape;
}

AnimatorMatrix get_matrix(const AnimatorKeyframes& akf, int keyframe){
    auto key_idx = std::distance(akf.keyframes_idx.begin(), std::find(akf.keyframes_idx.begin(), akf.keyframes_idx.end(), keyframe));
    return akf.keyframes.at(key_idx);
}

pair<AnimatorMatrix, ym_vec2r> get_matrix(const AnimatorKeyframes& akf, double delta){
    // For now it's just an hack for constant matrices.
    // If more keyframes are present, we should interpolate
    // to get the final matrix.
    if (delta < akf.keyframes_idx[0] || delta > akf.keyframes_idx.back()){
        auto bbox = ym_range2r({-3000.0, -3000.0}, {3000.0, 3000.0});
        return make_pair(AnimatorMatrix(bbox, ym_identity_affine_2r), ym_vec2r(0.0, 1.0));
    }
    else{
        auto start_idx = 0.0, end_idx = 1.0;
        for (auto i = 1; i < (int)akf.keyframes_idx.size(); i++){
            if (delta <= akf.keyframes_idx[i]) { start_idx = i - 1; end_idx = i; break;}
        }
        
        return make_pair(akf.keyframes[start_idx], ym_vec2r(akf.keyframes_idx[start_idx], akf.keyframes_idx[end_idx]));
    }
}

AnimatorKeyframes copy(const AnimatorKeyframes& akf){
    auto res = AnimatorKeyframes();
    res.keyframes = vector<AnimatorMatrix>();
    for (auto&& am : akf.keyframes) res.keyframes.push_back(copy(am));
    for (auto&& am_i : akf.keyframes_idx) res.keyframes_idx.push_back(am_i);
    return res;
}

AnimatorMatrix copy(const AnimatorMatrix& am){
    auto res = AnimatorMatrix(am.bounding_box);
    std::copy(std::begin(am.mats), std::end(am.mats), std::begin(res.mats));
    std::copy(std::begin(am.mats_centers), std::end(am.mats_centers), std::begin(res.mats_centers));
    res.step = ym_vec2r(am.step.x, am.step.y);
    return res;
}