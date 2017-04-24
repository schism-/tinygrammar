//
//  svg.cpp
//  tinygrammar
//
//  Created by tangles on 9/7/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "svg.h"

double get_sec(){
    time_t timer;
    struct tm y2k = {0};
    double seconds;
    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
    time(&timer);  /* get current time; same as: timer = time(NULL)  */
    seconds = difftime(timer,mktime(&y2k));
    return seconds;
}

polyline2r sample_bezier_polyline(const vector<ym_vec2r>& cp, int steps) {
    if (steps == 0) return polyline2r();
    auto curve = polyline2r(steps+1);
    curve[0] = cp[0];
    for(auto i : range(1,steps)) {
        auto t = i / (double)steps;
        auto it = 1-t;
        curve[i] = it*it*it*cp[0] + 3*it*it*t*cp[1] + 3*it*t*t*cp[2] + t*t*t*cp[3];
    }
    curve[steps] = cp[3];
    return curve;
}

#define NANOSVG_ALL_COLOR_KEYWORDS	// Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "../external/nanosvg/nanosvg.h"

vector<pair<polyline2r, string>> parse_svg_polylines(const string& svg, double resolution) {
    auto buffer = new char[svg.size()+1]; strcpy(buffer, svg.c_str());
    auto image = nsvgParse(buffer, "px", 96);
    auto tags = vector<string>();
    delete [] buffer;
    auto curves = vector<pair<polyline2r, string>>();
    for (auto shape = image->shapes; shape != nullptr; shape = shape->next) {
        for (auto path = shape->paths; path != nullptr; path = path->next) {
            curves.push_back(make_pair(polyline2r(), shape->id));
            auto cp = vector<ym_vec2r>();
            for(auto i : range(path->npts)) cp += ym_vec2r(path->pts[i*2],path->pts[i*2+1]);
            for (auto i = 0; i < cp.size()-1; i += 3) {
                if(cp[i+0] == cp[i+1] and cp[i+1] == cp[i+2] and cp[i+2] == cp[i+3]) continue;
                auto len = length_polyline(sample_bezier_polyline({cp[i+0],cp[i+1],cp[i+2],cp[i+3]}, 100));
                curves.back().first += sample_bezier_polyline({cp[i+0],cp[i+1],cp[i+2],cp[i+3]}, (int)round(len / resolution));
            }
            curves.back().first = remove_duplicates_polyline(curves.back().first);
            if(path == nullptr) continue;
            if(curves.back().first.empty()) { curves.pop_back(); continue;}
            if(path->closed) curves.back().first = close_polyline(curves.back().first);
        }
    }
    nsvgDelete(image);
    auto final_curves = vector<pair<polyline2r, string>>();
    for (auto c : curves) { if (not c.first.empty()) final_curves.push_back(c); }
    return final_curves;
}

vector<pair<polygon2r, string>> parse_svg_polygons(const string& svg, double resolution) {
    auto curves = parse_svg_polylines(svg, resolution);
    printf ("%d curves \n", (int)curves.size());
    for(auto& curve : curves) curve.first = close_polyline(curve.first);
    printf ("%d curves \n", (int)curves.size());
    auto inside = vector<vector<bool>>(curves.size());
    for(auto i : range(curves)) {
        inside[i] = vector<bool>(curves.size(),false);
        for(auto j : range(curves)) {
            if(i == j) continue;
            auto ins = true;
            for(auto&& p : curves[i].first) ins = ins and inside_polyline(curves[j].first, p);
            if (ins) inside[i][j] = true;
        }
    }
    auto polys = vector<pair<polygon2r, string>>();
    for(auto i : range(curves)) {
        auto inner = false;
        for(auto j : range(curves)) inner = inner or inside[i][j];
        //if(inner) continue;
        polys.push_back(make_pair(polygon2r({curves[i].first}), curves[i].second));
        //for(auto j : range(curves)) if(inside[j][i]) polys.back().first += curves[j].first;
    }
    printf ("%d polys \n", (int)polys.size());
    return polys;
}

ShapeGroup load_svg(string filename, bool flipy){
    auto svg = load_text_file(filename);
    auto data = parse_svg_polygons(svg, resolution);
    auto polys = make_vector(data, [&](pair<polygon2r, string> d){return d.first;});
    auto tags = make_vector(data, [&](pair<polygon2r, string> d){return d.second;});
    auto offset = ym_rcenter(bounds_polygons(polys));
    for(auto& poly : polys) poly = transform_polygon({ym_x2r, ym_y2r, -offset}, poly);
    if(flipy) {
        auto bbox = bounds_polygons(polys);
        for(auto& poly : polys) for(auto& curve : poly) for(auto& p : curve) p.y = bbox.max.y-p.y;
    }
    auto shapes = ShapeGroup();
    
    for(auto k = 0; k < polys.size(); k++){
        auto temp = new AnimatedShape(polys[k], 0, 0);
        temp->literal_tag = tags[k];
        shapes.push_back(temp);
    }
    printf ("%d shapes \n", (int)shapes.size());
    return shapes;
}

void save_svg(History* h, const ym_vec2i& wh, const ym_vec2r& offset,  const ym_vec2r& scale_factor){
    auto context = new SVGContext();
    context->begin_frame(wh, offset, scale_factor);
    switch (h->history_type) {
        case linear_history:
            for(auto shape : ((HistoryLinear*)h)->history.back()->shapes)
                context->draw_shape((TangleShape*)shape, false, false, {0,0,0,1}, {0,0,0,0});
            break;
            
        default:
            break;
    }
    context->end_frame(offset, scale_factor);
    time_t rawtime; time (&rawtime);
    auto r = new rng();
    save_text_file("images/tangleSVG_" + std::to_string(get_sec()) + "_" + std::to_string(ym_rng_nextf(r)) + ".svg", context->svg);
    delete context;
}

void save_svg(CSGTree::Tree* t, const ym_vec2i& wh, const ym_vec2r& offset,  const ym_vec2r& scale_factor, string postfix){
    auto context = new SVGContext();
    context->begin_frame(wh, offset, scale_factor);
    
    for (auto&& s : t->root->shapes)
        context->draw_shape(s);
    
    context->end_frame(offset, scale_factor);
    time_t rawtime; time (&rawtime);
    //save_text_file("./results/tangleSVG_" + std::to_string(get_sec()) + "_" + postfix + ".svg", context->svg);
    save_text_file(svgout_filename + "/tangleSVG_" + postfix + ".svg", context->svg);
    delete context;
}

void save_svg(const ShapeGroup& shapes, const ym_vec2i& wh, const ym_vec2r& offset,  const ym_vec2r& scale_factor, string postfix){
    auto context = new SVGContext();
    context->begin_frame(wh, offset, scale_factor);
    
    for (auto&& s : shapes)
        context->draw_shape((AnimatedShape*)s);
    
    context->end_frame(offset, scale_factor);
    time_t rawtime; time (&rawtime);
    save_text_file("./resources/svg/" + std::to_string(get_sec()) + "" + postfix + ".svg", context->svg);
    delete context;
}

