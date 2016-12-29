//
//  main_tag_svg.cpp
//  tinygrammar
//
//  Created by Christian on 28/12/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <stdio.h>
#include <functional>

#include "tangle_utilities.h"
#include "draw.h"
#include "svg.h"

static GLuint       g_FontTexture = 0;

void errorcb(int error, const char* desc)
{
    printf("GLFW error %d: %s\n", error, desc);
}

// ==========================
// =      UI FUNCTIONS      =
// ==========================

bool ImGui_ImplGlfwGL3_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
    
    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
    
    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    
    return true;
}

inline void setupStyle(){
    ImGuiStyle &style = ImGui::GetStyle();
    style.FramePadding = ImVec2(6,3);
    style.AntiAliasedLines = true;
    style.AntiAliasedShapes = true;
    style.ItemSpacing = ImVec2(10,10);
    style.Alpha = 1.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 10.0f;
    
    static int hue = 140;
    static float col_main_sat = 180.f/255.f;
    static float col_main_val = 161.f/255.f;
    static float col_area_sat = 124.f/255.f;
    static float col_area_val = 100.f/255.f;
    static float col_back_sat = 59.f/255.f;
    static float col_back_val = 40.f/255.f;
    
    ImVec4 col_text = ImColor::HSV(hue/255.f,  20.f/255.f, 235.f/255.f);
    ImVec4 col_main = ImColor::HSV(hue/255.f, col_main_sat, col_main_val);
    ImVec4 col_back = ImColor::HSV(hue/255.f, col_back_sat, col_back_val);
    ImVec4 col_area = ImColor::HSV(hue/255.f, col_area_sat, col_area_val);
    
    style.Colors[ImGuiCol_Text]                  = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(col_main.x, col_main.y, col_main.z, 0.64f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// ==========================
// =     /UI FUNCTIONS      =
// ==========================


ym_range2r ui_bounds_polygons(const ShapeGroup& polys) {
    auto bbox = ym_range2r();
    for(auto&& poly : polys) {
        auto temp = (AnimatedShape*)poly;
        bbox = ym_rexpand(bbox,bounds_polygon(temp->poly));
    }
    return bbox;
}

inline ym_vec2r to_vec2r(const ym_vec2f& v) { return {v.x,v.y}; }

ym_vec2r ui_scale_factor(GLFWwindow* window, ShapeGroup loaded_shapes) {
    auto wh = ui_window_size(window) - ym_vec2i{425, 200};
    auto bounds = ui_bounds_polygons(loaded_shapes);
    auto padding = 10.0;
    auto dim_x = bounds.max.x - bounds.min.x + padding;
    auto dim_y = bounds.max.y - bounds.min.y + padding;
    auto final_scale = ym_vec2r(1.0, 1.0);
    
    if (dim_x >= wh.x and dim_y >= wh.y){
        final_scale = ym_vec2r( min(wh.x / dim_x, wh.y / dim_y), min(wh.x / dim_x, wh.y / dim_y) );
    }
    else if (dim_x >= wh.x) final_scale = ym_vec2r(wh.x / dim_x, wh.x / dim_x);
    else if (dim_y >= wh.y) final_scale = ym_vec2r(wh.y / dim_y, wh.y / dim_y);

    return final_scale;
}

ym_vec2r ui_offset(GLFWwindow* window, ShapeGroup loaded_shapes) {
    auto wh = ui_window_size(window) - ym_vec2i{425, -200};
    ym_range2r rr = ui_bounds_polygons(loaded_shapes);
    double d = (ui_scale_factor(window, loaded_shapes)).x;
    ym_range2r r = ym_range2r (ym_rcenter(rr) - ym_rsize(rr) * d / 2, ym_rcenter(rr) + ym_rsize(rr) * d / 2);
    auto final_off = ym_vec2r(wh.x,wh.y) / 2 - 0.5 * ym_rcenter(r);
    return final_off;
}

ym_vec2r window_to_tangle(ym_vec2r p, GLFWwindow* window, ShapeGroup loaded_shapes) {
    return (1.0 / ui_scale_factor(window, loaded_shapes).x) * (p - ui_offset(window, loaded_shapes));
}

AnimatedShape* select_shape(ym_vec2r pos, ShapeGroup shapes) {
    auto picked = (AnimatedShape*)nullptr;
    for(auto shape : shapes) {
        if(inside_polygon(((AnimatedShape*)shape)->poly, pos)) picked = (AnimatedShape*)shape;
        if(picked) break;
    }
    if(not picked) return nullptr;
    else return picked;
}

void run() {
    glfwInit();
    
    GLFWwindow* window;
    NVGcontext* vg = NULL;
    
    if (!glfwInit()) {
        printf("Failed to init GLFW.");
        return;
    }
    
    glfwSetErrorCallback(errorcb);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    //window = glfwCreateWindow(1920, 1080, "gTangle", NULL, NULL);
    window = glfwCreateWindow(1440, 900, "gTangle", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    auto callbacks = ui_init_callbacks(window);
    ui_init_widgets(window, callbacks);
    
    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    glfwSwapInterval(0);
    
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("resources/open-sans/OpenSans-Regular.ttf", 20.0f);
    unsigned char* pixels;
    int f_width, f_height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &f_width, &f_height);
    
    /* ========================================
     +                                      +
     +              VARIABLES               +
     +                                      +
     ======================================== */
    
    auto context = new NVGContext();
    
    auto loaded_shapes = ShapeGroup();
    
    auto selected = vector<AnimatedShape*>();
    
    auto mouse_mode = "mouse_select"; // modes: select, outline, stroke
    auto mouse_stroke = polyline2r();
    
    bool right_pressed = false;
    
    static char tag[128] = "";
    
    auto color_map = vector<pair<AnimatedShape*, ym_vec4f>>();
    
    /* ========================================
     +                                      +
     +              CALLBACKS               +
     +                                      +
     ======================================== */
    
    auto init = [&](string filename) {
        loaded_shapes = load_svg(filename, false);
    };
    
    auto mouse_pos = [&]{ return window_to_tangle(to_vec2r(ui_mouse_pos(window)), window, loaded_shapes); };
    
    callbacks->text = [&](ui_window* window, int key) {
        switch (key) {
            case '1': init("resources/svg/woman.svg"); break;
            case '2': init("resources/svg/ciao.svg"); break;
            case '3': init("resources/svg/quads.svg"); break;
        }
    };
    
    /* ========================================
     +                                      +
     +             ACTUAL CODE              +
     +                                      +
     ======================================== */
    
    init("resources/svg/woman.svg");
        
    setupStyle();
    
    while (!glfwWindowShouldClose(window))
    {
        double mx, my;
        int winWidth, winHeight;
        int fbWidth, fbHeight;
        float pxRatio;
        
        glfwGetCursorPos(window, &mx, &my);
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = {static_cast<float>(mx), static_cast<float>(my)};
        io.MouseDown[0] = ui_mouse_left;
        io.MouseDown[1] = ui_mouse_right;
        
        // Calculate pixel ration for hi-dpi devices.
        pxRatio = (float)fbWidth / (float)winWidth;
        
        // Update and render
        glViewport(0, 0, fbWidth, fbHeight);
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        context->begin_frame({winWidth, winHeight}, ui_offset(window, loaded_shapes), ui_scale_factor(window, loaded_shapes));
        
        ui_begin_widgets(window,"GUI");
        
        if (ImGui::Button("Reset")){
            init("resources/svg/woman.svg");
        };
        
        ui_vertical_separator();
        
        ImGui::InputText("Insert tag", tag, IM_ARRAYSIZE(tag));
        
        if (ImGui::Button("Assign tag")){
            ym_vec4f rand_col = ym_vec4f(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, 1.0);
            for (auto ss : selected){
                ss->literal_tag = (string)tag;
                color_map.push_back(make_pair(ss, rand_col));
            }
            selected.clear();
        };
        
        ui_vertical_separator();
        if (ImGui::Button("Save tagged svg")){
            auto k = 0;
            for (auto ls : loaded_shapes)
                printf("%d : %s \n", k++, ((AnimatedShape*)ls)->literal_tag.c_str());
            auto bounds = ui_bounds_polygons(loaded_shapes);
            save_svg(loaded_shapes,
                     {(int)(1.2 * (bounds.max.x - bounds.min.x)), (int)(1.2 * (bounds.max.y - bounds.min.y))},
                     {-0.5 * (bounds.max.x - bounds.min.x), -0.5 * (bounds.max.y - bounds.min.y)},
                     {1.0, 1.0}, "_tagged");
            
        }
        
        bool mouse_on_widgets = ImGui::IsWindowHovered();
        ui_end_widgets(window);
        
        for(auto shape : loaded_shapes) context->draw_shape((AnimatedShape*)shape);
        
        if(!color_map.empty()) {
            for (auto cm : color_map) context->draw_shape(cm.first, {0, 0, 0, 1.0}, cm.second, true);
        }
        
        if(!selected.empty()) {
            for (auto ss : selected) context->draw_shape(ss, {0, 255, 0, 0.5}, {0, 255, 0, 0.5}, true);
        }
        
        context->end_frame(ui_offset(window, loaded_shapes), ui_scale_factor(window, loaded_shapes));
        
        if(ui_mouse_button(window, ui_mouse_left) ) {
            if(not mouse_on_widgets) {
                auto sel = select_shape(mouse_pos(), loaded_shapes);
                if (sel != nullptr and find(selected.begin(), selected.end(), sel) == selected.end())
                    selected.push_back(sel);
            }
        }
        else if(ui_mouse_button(window, ui_mouse_right) ) {
            if((not mouse_on_widgets) and (not right_pressed)) {
                right_pressed = true;
                selected.clear();
            }
        }
        
        if(not ui_mouse_button(window, ui_mouse_right)) right_pressed = false;
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    nvgDeleteGL2(vg);
    ui_clear_window(window);
    glfwTerminate();
    return;
}

int main(int argc, const char * argv[]) {
    run();
}
