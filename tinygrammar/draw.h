//
//  draw.hpp
//  tinygrammar
//
//  Created by tangles on 9/7/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef draw_h
#define draw_h

#include "ui.h"
#include "shape.h"

#define NANOVG_GL2
#include "../external/nanovg/nanovg.h"
#include "../external/nanovg/nanovg_gl.h"

#include "../external/format.h"

struct DrawContext {
    NVGcontext* draw_init();
    
    virtual void begin_frame(const ym_vec2i& wh, const ym_vec2r& offset, const ym_vec2r& scale) = 0;
    virtual void end_frame(const ym_vec2r& offset, const ym_vec2r& scale) = 0;
    
    virtual void draw_shape(TangleShape* shape, bool draw_frames = false, bool draw_as_points = false,
                            const ym_vec4f& stroke = {0,0,0,1}, const ym_vec4f& fill = {0,0,0,0});
    virtual void draw_labels(const vector<string>& labels);
    
    virtual void draw_polygon(const polygon2r& poly, const ym_vec4f& stroke, const ym_vec4f& fill) = 0;
    virtual void draw_line(const polyline2r& curve, const ym_vec4f& stroke) = 0;
    virtual void draw_point(const ym_vec2r& p, const ym_vec4f& stroke) = 0;
    virtual void draw_text(const ym_vec2r& p, const string& msg, double size, const ym_vec4f& stroke) = 0;
};

struct NVGContext : DrawContext {
    NVGcontext* vg = nullptr;
    
    NVGContext();
    
    virtual void begin_frame(const ym_vec2i& wh, const ym_vec2r& offset, const ym_vec2r& scale) override;
    virtual void end_frame(const ym_vec2r& offset, const ym_vec2r& scale) override;
    
    virtual void draw_polygon(const polygon2r& poly, const ym_vec4f& stroke, const ym_vec4f& fill) override;
    virtual void draw_line(const polyline2r& curve, const ym_vec4f& stroke) override;
    virtual void draw_point(const ym_vec2r& p, const ym_vec4f& stroke) override;
    virtual void draw_text(const ym_vec2r& p, const string& msg, double size, const ym_vec4f& stroke) override;
};

struct SVGContext : DrawContext {
    string  svg = "";
    
    SVGContext();
    
    virtual void begin_frame(const ym_vec2i& wh, const ym_vec2r& offset, const ym_vec2r& scale = {}) override;
    virtual void end_frame(const ym_vec2r& offset, const ym_vec2r& scale = {}) override;
    
    virtual void draw_polygon(const polygon2r& poly, const ym_vec4f& stroke, const ym_vec4f& fill) override;
    virtual void draw_line(const polyline2r& curve, const ym_vec4f& stroke) override;
    virtual void draw_point(const ym_vec2r& p, const ym_vec4f& stroke) override;
    virtual void draw_text(const ym_vec2r& p, const string& msg, double size, const ym_vec4f& stroke) override;
    
    string _svg_style(const ym_vec4f& stroke, const ym_vec4f& fill);
    string _svg_points(const polyline2r& curve);
};

#endif /* draw_hpp */
