//
//  animator_matrix.cpp
//  tinygrammar
//
//  Created by Christian on 03/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "animator_matrix.h"

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

polygon2r transform(const AnimatorMatrix& am, const polygon2r& poly){
    auto new_shape = polygon2r();
    for (auto&& line : poly){
        new_shape += polyline2r();
        for (auto&& p : line){
            auto m = get_matrix(am, p);
            auto new_p = ym_transform_point(m, p);
            new_shape.back() += new_p;
        }
    }
    return new_shape;
}

polygon2r transform_group(const AnimatorMatrix& am, const polygon2r& poly){
    auto new_shape = polygon2r();
    auto centroid = get_centroid(poly);
    auto m = get_matrix(am, centroid);
    for (auto&& line : poly){
        new_shape += polyline2r();
        for (auto&& p : line){
            auto new_p = ym_transform_point(m, p);
            new_shape.back() += new_p;
        }
    }
    return new_shape;
}



AnimatorMatrix get_matrix(const AnimatorKeyframes& akf, int keyframe){
    auto key_idx = std::distance(akf.keyframes_idx.begin(), std::find(akf.keyframes_idx.begin(), akf.keyframes_idx.end(), keyframe));
    return akf.keyframes.at(key_idx);
}