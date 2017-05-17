//
//  draw.cpp
//  tinygrammar
//
//  Created by tangles on 9/7/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "draw.h"

void DrawContext::draw_shape(TangleShape *shape, bool draw_frames,
                             bool draw_as_points, const ym_vec4f &stroke,
                             const ym_vec4f &fill) {
  if (draw_as_points) {
    for (auto &curve : shape->poly)
      for (auto &&p : curve)
        draw_point(p, stroke);
  } else {
    draw_polygon(shape->poly, stroke,
                 shape->invert ? ym_vec4f(0.f, 0.f, 0.f, 1.f) : fill);
  }
  if (draw_frames) {
    draw_line({shape->frame.o, shape->frame.o + shape->frame.x * 5},
              {1, 0, 0, 0.5f});
    draw_line({shape->frame.o, shape->frame.o + shape->frame.y * 5},
              {0, 0, 1, 0.5f});
  }
}

void DrawContext::draw_shape(AnimatedShape *shape, const ym_vec4f &stroke,
                             const ym_vec4f &fill, bool use_colors) {
  draw_polygon(shape->poly, use_colors ? stroke : shape->border_color,
               use_colors ? fill : shape->fill_color, shape->literal_tag);
}

void DrawContext::draw_labels(const vector<string> &labels) {
  auto offset = ym_vec2r{8, 18};
  for (auto i : range(labels)) {
    draw_text({offset.x, offset.y * (i + 1)}, labels[i], 18, {0, 0, 0, 1});
  }
}

void NVGContext::draw_polygon(const polygon2r &poly, const ym_vec4f &stroke,
                              const ym_vec4f &fill, string tag) {
  nvgBeginPath(vg);
  auto hole = false;
  nvgStrokeWidth(vg, 1.0f);
  for (auto &&curve : poly) {
    auto first = true;
    for (auto &&p : curve) {
      if (first) {
        nvgMoveTo(vg, p.x, p.y);
        first = false;
      } else
        nvgLineTo(vg, p.x, p.y);
    }
    nvgClosePath(vg);
    if (hole)
      nvgPathWinding(vg, NVG_HOLE);
    hole = true;
  }
  if (fill.w > 0) {
    nvgFillColor(vg, nvgRGBA(fill.x, fill.y, fill.z, fill.w * 255.0));
    nvgFill(vg);
  }
  if (stroke.w > 0) {
    nvgStrokeColor(vg, nvgRGBA(stroke.x, stroke.y, stroke.z, stroke.w * 255.0));
    nvgStroke(vg);
  }
}

void NVGContext::draw_line(const polyline2r &curve, const ym_vec4f &stroke) {
  nvgBeginPath(vg);
  auto first = true;
  for (auto &&p : curve) {
    if (first) {
      nvgMoveTo(vg, p.x, p.y);
      first = false;
    } else
      nvgLineTo(vg, p.x, p.y);
  }
  nvgStrokeColor(vg, nvgRGBA(255 * stroke.x, 255 * stroke.y, 255 * stroke.z,
                             255 * stroke.w));
  nvgStroke(vg);
}

void NVGContext::draw_point(const ym_vec2r &p, const ym_vec4f &stroke) {
  nvgBeginPath(vg);
  nvgMoveTo(vg, p.x, p.y);
  nvgLineTo(vg, p.x + 1, p.y + 1);
  nvgStrokeColor(vg, nvgRGBA(255 * stroke.x, 255 * stroke.y, 255 * stroke.z,
                             255 * stroke.w));
  nvgStroke(vg);
}

void NVGContext::draw_text(const ym_vec2r &p, const string &text, double size,
                           const ym_vec4f &stroke) {
  nvgFontSize(vg, size);
  nvgFontFace(vg, "default");
  nvgFillColor(vg, nvgRGBA(stroke.x * 255, stroke.y * 255, stroke.z * 255,
                           stroke.w * 255));
  nvgBeginPath(vg);
  nvgText(vg, p.x, p.y, text.c_str(), NULL);
  nvgRestore(vg);
}

NVGContext::NVGContext() {
  vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  nvgCreateFont(vg, "default", "resources/open-sans/OpenSans-Regular.ttf");
}

void NVGContext::begin_frame(const ym_vec2i &wh, const ym_vec2r &offset,
                             const ym_vec2r &scale) {
  // glClearColor(255, 255, 255, 0);
  // glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  nvgBeginFrame(vg, wh.x, wh.y, 1);
  nvgTranslate(vg, offset.x, offset.y);
  nvgScale(vg, scale.x, scale.y);
}

void NVGContext::end_frame(const ym_vec2r &offset, const ym_vec2r &scale) {
  nvgScale(vg, (double)1.0 / scale.x, (double)1.0 / scale.x);
  nvgTranslate(vg, -offset.x, -offset.y);
  nvgEndFrame(vg);
}

void SVGContext::draw_polygon(const polygon2r &poly, const ym_vec4f &stroke,
                              const ym_vec4f &fill, string tag) {
  auto style = _svg_style(stroke, fill);
  if (tag != "")
    svg += fmt::format("<path {} id=\"{}\" d=\"", style, tag);
  else
    svg += fmt::format("<path {} d=\"", style);
  for (auto &&curve : poly)
    svg += _svg_points(curve) + " ";
  svg += fmt::format("\" />\n");
}

void SVGContext::draw_line(const polyline2r &curve, const ym_vec4f &stroke) {
  auto style = _svg_style(stroke, {0, 0, 0, 0});
  auto points = _svg_points(curve);
  svg += fmt::format("<path {} d=\"{}\" />\n", style, points);
}

void SVGContext::draw_point(const ym_vec2r &p, const ym_vec4f &stroke) {}

void SVGContext::draw_text(const ym_vec2r &p, const string &text, double size,
                           const ym_vec4f &stroke) {}

string SVGContext::_svg_style(const ym_vec4f &stroke, const ym_vec4f &fill) {
  string svg = "";
  if (stroke.w != 0)
    svg += fmt::format("stroke=\"rgb({},{},{})\" stroke-opacity=\"{}\"",
                       (int)stroke.x, (int)stroke.y, (int)stroke.z, stroke.w);
  else
    svg += "stroke=\"none\"";
  svg += " ";
  if (fill.w != 0)
    svg += fmt::format("fill=\"rgb({},{},{})\" fill-opacity=\"{}\"",
                       (int)fill.x, (int)(int)fill.y, (int)fill.z, fill.w);
  else
    svg += "fill=\"none\"";
  return svg;
}

string SVGContext::_svg_points(const polyline2r &curve) {
  string svg = "";
  auto first = true;
  for (auto &&p : curve) {
    svg += fmt::format("{}{} {} ", (first) ? "M" : "L", p.x, p.y);
    first = false;
  }
  //    if(closed_polyline(curve)) svg += "Z";
  svg += "Z";
  return svg;
}

SVGContext::SVGContext() { svg = "<!DOCTYPE svg>\n"; }

void SVGContext::begin_frame(const ym_vec2i &wh, const ym_vec2r &offset,
                             const ym_vec2r &scale) {
  svg += "<svg xmlns=\"http://www.w3.org/2000/svg\" "
         "xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" +
         to_string(wh.x) + "\" height=\"" + to_string(wh.y) + "\">\n";
  svg += "<g transform=\"translate(" + to_string(offset.x) + "," +
         to_string(offset.y) + ")\">\n";
  svg += "<g transform=\"scale(" + to_string(scale.x) + "," +
         to_string(scale.y) + ")\">\n";
}

void SVGContext::end_frame(const ym_vec2r &offset, const ym_vec2r &scale) {
  svg += "</g>\n";
  svg += "</g>\n";
  svg += "</svg>\n";
}

void SVGFileContext::draw_polygon(const polygon2r &poly, const ym_vec4f &stroke,
                                  const ym_vec4f &fill, string tag) {
  fprintf(f, "<path ");
  _svg_style(stroke, fill);
  if (tag != "") {
    fprintf(f, " id=\"%s\"", tag.c_str());
  }
  fprintf(f, " d=\"");
  for (auto &&curve : poly) {
    _svg_points(curve);
    fprintf(f, " ");
  }
  fprintf(f, "\" />\n");
}

void SVGFileContext::draw_line(const polyline2r &curve,
                               const ym_vec4f &stroke) {
  fprintf(f, "<path ");
  _svg_style(stroke, {0, 0, 0, 0});
  fprintf(f, "d=\"");
  _svg_points(curve);
  fprintf(f, "\" />\n");
}

void SVGFileContext::draw_point(const ym_vec2r &p, const ym_vec4f &stroke) {}

void SVGFileContext::draw_text(const ym_vec2r &p, const string &text,
                               double size, const ym_vec4f &stroke) {}

void SVGFileContext::_svg_style(const ym_vec4f &stroke, const ym_vec4f &fill) {
  if (stroke.w != 0) {
    fprintf(f, "stroke=\"rgb(%d,%d,%d)\" stroke-opacity=\"%.3g\"", (int)stroke.x,
            (int)stroke.y, (int)stroke.z, stroke.w);
  } else {
    fprintf(f, "stroke=\"none\"");
  }
  fprintf(f, " ");
  if (fill.w != 0) {
    fprintf(f, "fill=\"rgb(%d,%d,%d)\" fill-opacity=\"%.3g\"", (int)fill.x,
            (int)fill.y, (int)fill.z, fill.w);
  } else {
    fprintf(f, "fill=\"none\"");
  }
}

void SVGFileContext::_svg_points(const polyline2r &curve) {
  auto first = true;
  for (auto &&p : curve) {
    fprintf(f, "%s%.4g %.4g ", (first) ? "M" : "L", p.x, p.y);
    first = false;
  }
  //    if(closed_polyline(curve)) svg += "Z";
  fprintf(f, "Z");
}

SVGFileContext::SVGFileContext(const std::string &filename) {
  this->filename = filename;
  f = fopen(filename.c_str(), "wt");
  if (!f)
    printf("BUG: could not open file %s\n", filename.c_str());
  fprintf(f, "<!DOCTYPE svg>\n");
}

void SVGFileContext::begin_frame(const ym_vec2i &wh, const ym_vec2r &offset,
                                 const ym_vec2r &scale) {
  fprintf(f,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" "
          "xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%d\" "
          "height=\"%d\">\n",
          wh.x, wh.y);
  fprintf(f, "<g transform=\"translate(%f,%f)\">\n", offset.x, offset.y);
  fprintf(f, "<g transform=\"scale(%f,%f)\">\n", scale.x, scale.y);
}

void SVGFileContext::end_frame(const ym_vec2r &offset, const ym_vec2r &scale) {
  fprintf(f, "</g>\n");
  fprintf(f, "</g>\n");
  fprintf(f, "</svg>\n");
    fclose(f);
}
