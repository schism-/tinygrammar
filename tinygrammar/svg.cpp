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
    
    for (auto&& s : t->root->content->shapes) context->draw_shape(s, false, false, {0, 0, 0, 1}, {200, 200, 200,1});
    
    context->end_frame(offset, scale_factor);
    time_t rawtime; time (&rawtime);
    save_text_file("./results/tangleSVG_" + std::to_string(get_sec()) + "_" + postfix + ".svg", context->svg);
    delete context;
}

