//
//  tangle_utilities.cpp
//  tinygrammar
//
//  Created by tangles on 6/22/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "tangle_utilities.h"

polyline2r tangents_polyline(const polyline2r& curve, bool use_both) {
    auto tang = polyline2r(curve.size());
    if(use_both) {
        for(auto i : range(1,(int)curve.size()-1)) {
            tang[i] = ym_normalize(curve[i+1]-curve[i-1]);
        }
        if(closed_polyline(curve)) {
            tang.front() = ym_normalize(tang[1]-tang[tang.size()-2]);
        } else {
            tang.front() = ym_normalize(tang[1]-tang[0]);
            tang.back() = ym_normalize(tang[tang.size()-1]-tang[tang.size()-2]);
        }
    } else {
        for(auto i : range((int)curve.size()-1)) {
            tang[i] = ym_normalize(curve[i+1]-curve[i]);
        }
        if(closed_polyline(curve)) tang.back() = tang.front();
        else tang.back() = ym_normalize(tang[tang.size()-1]-tang[tang.size()-2]);
    }
    return tang;
}


polyline2r normals_polyline(const polyline2r& curve, bool flipped, bool use_both) {
    auto tang = tangents_polyline(curve, use_both);
    auto norms = polyline2r(curve.size());
    if(not flipped) {
        for(auto i : range(norms)) {
            norms[i] = {-tang[i].y,tang[i].x};
        }
    } else {
        for(auto i : range(norms)) {
            norms[i] = {tang[i].y,-tang[i].x};
        }
    }
    return norms;
}

#define STB_PERLIN_IMPLEMENTATION
#include "../external/stb/stb_perlin.h"

polyline2r perturb_polyline(const polyline2r& curve, double scale, double strength, rng& rn, bool ortho) {
    auto perturbed = polyline2r();
    auto seed = ym_rng_next2f(&rn);
    if(ortho) {
        perturbed.resize(curve.size());
        auto norms = normals_polyline(curve);
        auto len = 0.0f; auto last_p = curve[0];
        for(auto i : range(perturbed)) {
            len += ym_length(curve[i] - last_p); last_p = curve[i];
            perturbed[i] = curve[i] + norms[i] * strength * (stb_perlin_noise3(len / scale, seed.x*151, seed.y*977) - 0.5f);
        }
        if(closed_polyline(curve)) perturbed.back() = perturbed.front();
    } else {
        for (auto p : curve){
            perturbed.push_back( p + ym_vec2r{
                strength*(stb_perlin_noise3(p.x/scale, p.y/scale, seed.x*151)-0.5f),
                strength*(stb_perlin_noise3(p.x/scale, p.y/scale, seed.y*977)-0.5f),
            });
        }
    }
    // error_if_not(not intersect_polyline(curve), "curve self intersects");
    return perturbed;
}

polyline2r RuleUtils::perturb(const polyline2r& curve, rng& rn) {
    auto bbox = bounds_polyline(curve);
    auto seed = ym_rng_next2f(&rn);
    auto perturbed = make_polyline(curve, [&](const ym_vec2r& p) {
        return p + ym_vec2r{
            perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.x*151)-0.5f),
            perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.y*977)-0.5f),
        }; });
    perturbed = transform_polyline({-ym_rcenter(bounds_polyline(perturbed))+ym_rcenter(bbox),ym_x2r,ym_y2r}, perturbed);
    return perturbed;
}

polygon2r RuleUtils::perturb(const polygon2r& poly, rng& rn) {
    auto bbox = bounds_polygon(poly);
    auto seed = ym_rng_next2f(&rn);
    auto perturbed = make_polygon(poly, [&](const polyline2r& curve) {
        return make_polyline(curve, [&](const ym_vec2r& p) {
            return p + ym_vec2r{
                perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.x*151)-0.5f),
                perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.y*977)-0.5f),
            };
        });
    });
    perturbed = transform_polygon({-ym_rcenter(bounds_polygon(perturbed))+ym_rcenter(bbox),ym_x2r,ym_y2r}, perturbed);
    return perturbed;
}

vector<polyline2r> RuleUtils::perturb(const vector<polyline2r>& curves, rng& rn) {
    auto bbox = bounds_polylines(curves);
    auto seed = ym_rng_next2f(&rn);
    auto perturbed = make_vector(curves, [&](const polyline2r& curve) {
        return make_polyline(curve, [&](const ym_vec2r& p) {
            return p + ym_vec2r{
                perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.x*151)-0.5f),
                perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.y*977)-0.5f),
            };
        });
    });
    perturbed = transform_polylines({-ym_rcenter(bounds_polylines(perturbed))+ym_rcenter(bbox),ym_x2r,ym_y2r}, perturbed);
    return perturbed;
}

vector<polygon2r> RuleUtils::perturb(const vector<polygon2r>& polys, rng& rn) {
    auto bbox = bounds_polygons(polys);
    auto seed = ym_rng_next2f(&rn);
    auto perturbed = make_vector(polys, [&](const polygon2r& poly) {
        return make_polygon(poly, [&](const polyline2r& curve) {
            return make_polyline(curve, [&](const ym_vec2r& p) {
                return p + ym_vec2r{
                    perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.x*151)-0.5f),
                    perturb_strength*(stb_perlin_noise3(p.x/perturb_scale, p.y/perturb_scale, seed.y*977)-0.5f),
                };
            });
        });
    });
    auto offset = -ym_rcenter(bounds_polygons(perturbed))+ym_rcenter(bbox);
    for(auto& poly : perturbed) poly = transform_polygon({offset,ym_x2r,ym_y2r}, poly);
    return perturbed;
}