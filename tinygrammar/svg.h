//
//  svg.h
//  tinygrammar
//
//  Created by tangles on 9/7/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef svg_h
#define svg_h

#include "draw.h"
#include "expansion_manager.h"

void save_svg(History* h, const ym_vec2i& wh, const ym_vec2r& offset,  const ym_vec2r& scale_factor);
void save_svg(CSGTree::Tree* t, const ym_vec2i& wh, const ym_vec2r& offset,  const ym_vec2r& scale_factor);

#endif /* svg_h */
