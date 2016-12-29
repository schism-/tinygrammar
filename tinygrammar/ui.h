#ifndef _UI_H_
#define _UI_H_

#include "common.h"
#include "yocto_math.h"
#include "../external/glfw/glfw3.h"

#define UI_WIDGETS
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

// types
using ui_window = GLFWwindow;

// callbacks
struct ui_callbacks {
    function<void (ui_window*, unsigned int)>       text;
    function<void (ui_window*, int, int, int, int)> key;
    function<void (ui_window*,int,int)>             reshape;
    function<void (ui_window*,int,int,int)>         mouse_button;
    function<void (ui_window*,const ym_vec2f&)>        mouse_pos;
    function<void (ui_window*,const ym_vec2f&)>        scroll;
    
#ifdef UI_WIDGETS
    bool    widgets = false;
#endif
};

// constants
const int ui_mouse_left = GLFW_MOUSE_BUTTON_LEFT;
const int ui_mouse_right = GLFW_MOUSE_BUTTON_RIGHT;
const int ui_key_shift = GLFW_KEY_LEFT_SHIFT;
const int ui_key_control = GLFW_KEY_LEFT_CONTROL;
const int ui_key_alt = GLFW_KEY_LEFT_ALT;

// glfw window callback adaptors
inline void _ui_text_callback(GLFWwindow* window, unsigned int key);
inline void _ui_key_callback(GLFWwindow* window,int key, int scancode, int action, int mods);
inline void _ui_reshape_callback(GLFWwindow* window, int width, int height);
inline void _ui_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
inline void _ui_mouse_pos_callback(GLFWwindow* window, double x, double y);
inline void _ui_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// init the ui state for a window
inline ui_window* ui_init_window(const ym_vec2i& size, const string& title, int samples = 1) {
    // init glfw
    auto ok = glfwInit();
    error_if_not(ok, "glfw init error");
    
    // setting an error callback
    glfwSetErrorCallback([](int ecode, const char* msg){ return error(msg); });
    
    // setting windows hints before creation
    if(samples > 1) glfwWindowHint(GLFW_SAMPLES, samples);
    
    // create window
    auto window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
    error_if_not(window, "glfw window error");
    
    // make gl context current
    glfwMakeContextCurrent(window);
    
    // configure input modes
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
#ifdef _WIN32
    // gl3w on windows
    auto ok1 = gl3wInit();
    error_if_not(ok1, "gl3w init error");
#endif
    
    // done
    return window;
}

// clear window
inline void ui_clear_window(ui_window* window) { glfwDestroyWindow(window); glfwTerminate(); }

// init ui callbacks
inline ui_callbacks* ui_init_callbacks(ui_window* window) {
    // init callbacks
    auto callbacks = new ui_callbacks();
    // set user pointer for callbacks
    glfwSetWindowUserPointer(window, callbacks);
    // set callbacks
    glfwSetCharCallback(window, _ui_text_callback);
    glfwSetKeyCallback(window, _ui_key_callback);
    glfwSetWindowSizeCallback(window, _ui_reshape_callback);
    glfwSetMouseButtonCallback(window, _ui_mouse_button_callback);
    glfwSetCursorPosCallback(window, _ui_mouse_pos_callback);
    glfwSetScrollCallback(window, _ui_scroll_callback);
    // done
    return callbacks;
}

// clear callbacks
inline void ui_clear_callbacks(ui_callbacks* callbacks) { delete callbacks; }

// check if window should close
inline bool ui_done(ui_window* window) { return glfwWindowShouldClose(window); }

// windows properies
inline ym_vec2i ui_window_size(ui_window* window) { auto size = ym_vec2i(); glfwGetWindowSize(window, &size.x, &size.y); return size; }
// windows properies
inline ym_vec2i ui_framebuffer_size(ui_window* window) { auto size = ym_vec2i(); glfwGetFramebufferSize(window, &size.x, &size.y); return size; }

// mouse button
inline bool ui_mouse_button(ui_window* window, int button) { return glfwGetMouseButton(window, button); }
inline ym_vec2f ui_mouse_pos(ui_window* window) { double x, y; glfwGetCursorPos(window, &x, &y); return ym_vec2f(x,y); }
inline bool ui_key(ui_window* window, int key) { return glfwGetKey(window, key); }

// time
inline double ui_time(ui_window* window) { return glfwGetTime(); }

// begin/end frame (polling events, synching values, etc)
inline void ui_end_frame(ui_window* window) {
    // swap buffers
    glfwSwapBuffers(window);
    
    // process events
    glfwPollEvents();
}

// exit from windows loop
inline void ui_exit(ui_window* window) { glfwSetWindowShouldClose(window, true); }

// hides the window
inline void ui_hide(ui_window* window) { glfwHideWindow(window); }

// window title
inline void ui_set_title(ui_window* window, const string& title) { glfwSetWindowTitle(window, title.c_str()); }

#ifdef UI_WIDGETS
// includes
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_glfw.h"

// init widgets
inline void ui_init_widgets(ui_window* window, ui_callbacks* callbacks) {
    ImGui_ImplGlfw_Init(window, false);
    ImGui::GetIO().FontGlobalScale = ui_framebuffer_size(window).x / ui_window_size(window).x;
    callbacks->widgets = true;
}

// clean widgets
inline void ui_clear_widgets(ui_window* window) { ImGui_ImplGlfw_Shutdown(); }

// start drawing widgets
inline bool ui_begin_widgets(ui_window* window, const string& name) {
    ImGui_ImplGlfw_NewFrame();
    return ImGui::Begin(name.c_str(), nullptr, ImVec2(0,0), 1);
}

// end drawing widgets
inline void ui_end_widgets(ui_window* window) {
    ImGui::End();
    ImGui::Render();
}

// text widgets
inline void ui_vertical_separator(){ ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); }

inline bool ui_button_widget(ui_window* window, const string& msg) { return ImGui::Button(msg.c_str()); }
inline void ui_text_widget(ui_window* window, const string& msg) { ImGui::Text(msg.c_str()); }
inline void ui_text_widget(ui_window* window, const string& label, const string& msg) { ImGui::LabelText(label.c_str(),msg.c_str()); }
inline bool ui_slider_widget(ui_window* window, const string& label, float* value, const ym_range1f& range = {0,1}) {
    return ImGui::SliderFloat(label.c_str(), value, range.min, range.max);
}
inline bool ui_slider_widget(ui_window* window, const string& label, ym_vec2f* value, const ym_range1f& range = {0,1}) {
    return ImGui::SliderFloat2(label.c_str(), value->v, range.min, range.max);
}
inline bool ui_slider_widget(ui_window* window, const string& label, ym_vec3f* value, const ym_range1f& range = {0,1}) {
    return ImGui::SliderFloat3(label.c_str(), value->v, range.min, range.max);
}
inline bool ui_color_widget(ui_window* window, const string& label, ym_vec3f* value) {
    return ImGui::ColorEdit3(label.c_str(), value->v);
}
inline bool ui_colorf_widget(ui_window* window, const string& label, ym_vec3f* value) {
    auto changed = false;
    changed = changed or ui_color_widget(window, label+"##color", value);
    changed = changed or ui_slider_widget(window, label+"##float", value);
    return changed;
}
inline bool ui_colorl_widget(ui_window* window, const string& label, ym_vec3f* value, const ym_range1f& range = {0,1}) {
    auto changed = false;
    changed = changed or ui_color_widget(window, label+"##color", value);
    auto l = ym_mean(*value);
    auto black = l == 0;
    if(ui_slider_widget(window, label+"##float", &l, range)) {
        changed = true;
        if(black) *value = {l,l,l};
        else *value *= l / ym_mean(*value);
    }
    return changed;
}
inline bool ui_colori_widget(ui_window* window, const string& label, ym_vec3f* value, const ym_range1f& range = {0,1000}) {
    auto m = max(value->x,max(value->y,value->z));
    auto black = m == 0;
    auto c = (black) ? ym_zero3f : *value / m;
    auto changed = false;
    changed = changed or ui_color_widget(window, label+"##color", &c);
    changed = changed or ui_slider_widget(window, label+"##float", &m, range);
    if(changed) {
        if(black) *value = {m,m,m};
        else *value *= m * c;
    }
    return changed;
}
inline bool ui_combo_widget(ui_window* window, const string& label, const vector<string>& labels, int* value) {
    string str = ""; for(auto s : labels) str += s + '\0'; str += '\0';
    return ImGui::Combo(label.c_str(), value, str.c_str());
}
template<typename T>
inline bool ui_combo_widget(ui_window* window, const string& label, const vector<string>& labels, const vector<T>& values, T* value) {
    auto selected = (int)(std::find(values.begin(),values.end(),*value) - values.begin());
    auto edited = ui_combo_widget(window, label, labels, &selected);
    if(edited) *value = values[selected];
    return edited;
}
template<typename T>
inline bool ui_combo_null_widget(ui_window* window, const string& label, const vector<string>& labels, const vector<T*>& values, T** value) {
    auto nlabels = vector<string>({"nullptr"}) + labels;
    auto nvalues = vector<T*>({nullptr}) + values;
    return ui_combo_widget(window, label, nlabels, nvalues, value);
}
inline bool ui_rotation_widget(ui_window* window, const string& label, ym_frame3f* value) {
//    auto rot = euler_angles(frame_to_matrix(*value));
//    auto edited = ui_slider_widget(window, label, &rot, {-ym_pif,ym_pif});
//    if(edited) { auto r = euler_matrix(rot); auto ff = matrix_to_frame(r); ff.o = value->o; *value = ff; }
    return false;
}
inline bool ui_frame_widget(ui_window* window, const string& label, ym_frame3f* value, const ym_range1f& range = {-10,10}) {
    auto pos = ui_slider_widget(window, label+".o", &value->o, range);
    auto rot = ui_rotation_widget(window, label+".r", value);
    return pos or rot;
}
inline bool ui_header_widget(ui_window* window, const string& label) {
    return ImGui::CollapsingHeader(label.c_str());
}
inline void ui_user_guide_widget(ui_window* window, const string& label = "user guide") {
    static auto visible = false; ImGui::Checkbox(label.c_str(), &visible); if(visible) ImGui::ShowUserGuide();
}
inline void ui_style_editor_widget(ui_window* window, const string& label = "style editor") {
    static auto visible = false; ImGui::Checkbox(label.c_str(), &visible); if(visible) ImGui::ShowStyleEditor();
}
inline void ui_test_window_widget(ui_window* window, const string& label = "test window") {
    static auto visible = false; ImGui::Checkbox(label.c_str(), &visible); if(visible) ImGui::ShowTestWindow();
}
inline void ui_begin_group_widget(ui_window* window, const string& name) { ImGui::PushID(name.c_str()); }
inline void ui_end_group_widget(ui_window* window) { ImGui::PopID(); }

#endif

// glfw window callback adaptors
inline void _ui_text_callback(ui_window* window, unsigned int c) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
#ifdef UI_WIDGETS
    if(callbacks->widgets) {
        ImGui_ImplGlfw_CharCallback(window, c);
        if(ImGui::GetIO().WantCaptureKeyboard) return;
    }
#endif
    if(callbacks->text) callbacks->text(window,c);
}
inline void _ui_key_callback(ui_window* window, int key, int scancode, int action, int mods) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
#ifdef UI_WIDGETS
    if(callbacks->widgets) {
        ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
        if(ImGui::GetIO().WantCaptureKeyboard) return;
    }
#endif
    if(callbacks->key) callbacks->key(window,key,scancode,action,mods);
}
inline void _ui_reshape_callback(ui_window* window, int width, int height) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
    if(callbacks->reshape) callbacks->reshape(window, width, height);
}
inline void _ui_mouse_button_callback(ui_window* window, int button, int action, int mods) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
#ifdef UI_WIDGETS
    if(callbacks->widgets) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        if(ImGui::GetIO().WantCaptureMouse) return;
    }
#endif
    if(callbacks->mouse_button) callbacks->mouse_button(window,button,action,mods);
}
inline void _ui_mouse_pos_callback(ui_window* window, double x, double y) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
#ifdef UI_WIDGETS
    if(callbacks->widgets) {
        if(ImGui::GetIO().WantCaptureMouse) return;
    }
#endif
    if(callbacks->mouse_pos) callbacks->mouse_pos(window,ym_vec2f(x,y));
}
inline void _ui_scroll_callback(ui_window* window, double x, double y) {
    auto callbacks = (ui_callbacks*)glfwGetWindowUserPointer(window);
#ifdef UI_WIDGETS
    if(callbacks->widgets) {
        if(ImGui::GetIO().WantCaptureMouse) return;
        ImGui_ImplGlfw_ScrollCallback(window, x, y);
    }
#endif
    if(callbacks->scroll) callbacks->scroll(window,ym_vec2f(x,y));
}

#endif
