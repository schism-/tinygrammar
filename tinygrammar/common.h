//
//  common.h
//  tinygrammar
//
//  Created by tangles on 6/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef common_h
#define common_h

#include <string>
#include <vector>
#include <array>
#include <initializer_list>
#include <set>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>
#include <random>

#include "tree.hh"
#include "yocto_math.h"

using namespace std;

#define PARAM_SIZE 20
#define TAG_SIZE 16

#define ACTIVE_GRAMMAR 2

#define IS_DEBUG 0

#define SPEEDUP_SUM 1
#define SPEEDUP_ALIASSUM 1
#define SPEEDUP_SAVESVG 1

static constexpr double resolution = 2;
// static constexpr double matrix_resolution = 100;
static constexpr double matrix_resolution = 25;
static constexpr int matrix_num = (int)(matrix_resolution * matrix_resolution);

enum {
    basic_shape = 0,
    tangle_shape,
    annotated_shape,
    animated_shape,
    time_shape
};

enum {
    linear_history = 0,
    tree_history,
    animation_history
};

enum {
    tangle_grammar = 0,
    learning_grammar,
    animation_grammar
};

enum {
    op_default = 0,
    op_init,
    op_time_init,
    op_split,
    op_place,
    op_time_slice,
    op_affine,
    op_affine_rot,
    op_affine_tran,
    op_affine_scale,
    op_affine_rot_tran,
    op_affine_rot_scale,
    op_affine_scale_tran,
    op_move_towards,
    op_explode,
    op_attribute
};

enum {
    anim_eulerian = 0,
    anim_perturb,
    anim_wave,
    anim_single,
    anim_group,
    anim_morph,
    anim_attribute,
    anim_noop
};

enum {
    union_op = 0,
    difference_op,
    intersection_op,
    xor_op,
    sum_op,
    place_in_op
};

typedef ym_vec<double, 2> ym_vec2r;
typedef ym_affine<double, 2, false> ym_affine2r;
const ym_affine2r ym_identity_affine_2r = ym_affine2r({{1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0}});

typedef ym_mat<double, 3, 3> ym_mat3r;

const ym_vec2r ym_x2r = ym_vec2r(1, 0);
const ym_vec2r ym_y2r = ym_vec2r(0, 1);

typedef ym_rng_pcg32 rng;

typedef ym_range<ym_vec2r> ym_range2r;

typedef ym_affine<double, 2, true> ym_frame2r;
const ym_frame2r ym_identity_frame2r = ym_frame2r{{1, 0}, {0, 1}, {0, 0}};

typedef ym_vec<double, PARAM_SIZE> rule_params;
typedef ym_vec<int, TAG_SIZE> rule_tags;

typedef ym_vec<double, PARAM_SIZE> anim_params;

extern string grammar_filename;
extern string svgout_filename;

// ==============================================
// ====== METHODS FOR LOG/EXP OF MATRICES =======
// ==============================================

#define EPS_1 0.0001
#define EPS_2 0.000000001
#define EPS_3 0.000001

inline double frobenius_norm(const ym_mat3r& m) {
    return sqrt( m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2] +
                 m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2] +
                 m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2] );
}

inline ym_mat3r pow(const ym_mat3r& m, int exponent) {
    auto ret = ym_mat3r(m);
    auto factor = ym_mat3r(m);
    for(auto t = 1; t < exponent; t++) ret = ret * factor;
    return ret;
}

inline ym_mat3r sqrt(const ym_mat3r& m) {
    auto A = ym_mat3r(m);
    auto x = ym_mat3r(m);
    auto y = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    
    int i = 0;
    auto min_A = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    double min_norm = 1000000.0f;
    
    while(frobenius_norm((- A) + (x * x)) > EPS_3) {
        auto ix = ym_inverse(x);
        auto iy = ym_inverse(y);
        
        x = (x + iy) * 0.5;
        y = (y + ix) * 0.5;
        i++;
        
        if(frobenius_norm((- A) + (x * x)) < min_norm){
            min_A = x;
            min_norm = frobenius_norm((- A) + (x * x));
        }
        if(i>=5) {
            break;
        }
    }
    
    return min_A;
}

inline ym_mat3r ln(const ym_mat3r& m){
    auto k = 0;
    auto A = ym_mat3r(m);
    auto idm = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    
    while(frobenius_norm(A + (-idm)) > EPS_1) {
        A = sqrt(A);
        k += 1;
    }
    
    A = A + (-idm);
    
    auto Z = ym_mat3r(A);
    auto X = ym_mat3r(A);
    auto i = 1.;

    while(frobenius_norm(Z) > EPS_2) {
        Z = Z * A;
        i += 1.;
        X = X + (Z * (1.0 / i));
    }
    
    return X * pow(2, k);
}

inline ym_mat3r exp(const ym_mat3r& m) {
    auto A = ym_mat3r(m);
    
    int tmp = (1 + floor(log2(frobenius_norm(A))));
    
    double j = max(0,tmp);
    
    A = A * pow(2.0, -j);
    
    auto D = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    auto N = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    auto X = ym_mat3r({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    
    double c = 1;
    double q = 6;
    double q1 = q + 1;
    
    for(float k = 1; k < q1; k++) {
        c = c * (q1 - k + 1.) / (k * (2.0 * q1 - k + 1.));
        X = A * X;
        N = N + X * c;
        D = D + X * (pow(-1, k) * c);
    }
    
    X = ym_inverse(D) * N;
    X = pow(X, pow(2.0, j));
    
    return X;
}



// Prints a message (printf style) and flushes the output (useful during debugging)
inline void message(const char* msg, ...) { va_list args; va_start(args, msg); vprintf(msg, args); va_end(args); fflush(stdout); }
// Prints an error message (printf style), flushes the output (useful during debugging) and asserts
inline void error(const char* msg, ...) { va_list args; va_start(args, msg); vprintf(msg, args); va_end(args);  fflush(stdout); assert(false); }
// Checks for error, prints an error message (printf style), flushes the output (useful during debugging) and asserts
inline void error_if_not(bool check, const char* msg, ...) { if(check) return; va_list args; va_start(args, msg); vprintf(msg, args); va_end(args); fflush(stdout); assert(false); }
// simple string creation (printf style) --- warning: out should be < 4096
inline string tostring(const char* msg, ...) { char buf[4096]; va_list args; va_start(args, msg); vsprintf(buf, msg, args); va_end(args); return string(buf); }

// Python-style range: iterates from min to max in range-based for loops
struct range {
    int min = 0; // start
    int max = 1; // end
    
    // iterator
    struct iterator {
        int current; // current value
        iterator(int value) : current(value) { } // constructor
        int& operator*() { return current; } // iterator support
        iterator& operator++() { ++current; return *this; } // iterator support
        bool operator!=(const iterator& other) { return current != other.current; } // iterator support
    };
    
    // constructor for range in [0,max)
    range(int max) : min(0), max(max) { }
    // constructor for range in [min,max)
    range(int min, int max) : min(min), max(max) { }
    // constructor for range in [0,v.size())
    template<typename T>
    range(const vector<T>& v) : min(0), max(int(v.size())) { }
    
    // iterator support
    iterator begin() const { return iterator(min); }
    // iterator support
    iterator end() const { return iterator(max); }
};

// simple implementation of optional value
// use only for small classes since it is not optimized for speed
template<typename T>
struct optional {
    bool        ok = false; // whether it exists
    T           value;      // the actual value
    
    // optional initialization
    optional() : ok(false), value() { }
    optional(const T& v): ok(true), value(v) { }
    
    // check whether it is defined
    operator bool() const { return ok; }
};

// makes an optional
template<typename T>
inline optional<T> make_optional(const T& v) { return {v}; }

// python-like list and dictionary manipulation
template<typename T>
inline vector<T> operator+(const vector<T>& a, const vector<T>& b) { auto ret = vector<T>(); ret.insert(ret.end(),a.begin(),a.end()); ret.insert(ret.end(),b.begin(),b.end()); return ret; }

template<typename T>
inline vector<T> operator+(const vector<T>& a, const T& b) { auto ret = vector<T>(); ret.insert(ret.end(),a.begin(),a.end()); ret.push_back(b); return ret; }

template<typename T>
inline vector<T>& operator+=(vector<T>& a, const vector<T>& b) { a.insert(a.end(),b.begin(),b.end()); return a; }

template<typename T>
inline vector<T>& operator+=(vector<T>& a, const T& b) { a.push_back(b); return a; }

// check if a container contains an element
inline bool contains(const string& a, const string& b) { return a.find(b) != string::npos; }
template<typename T> inline bool contains(const vector<T>& a, const T& b) { return std::find(a.begin(),a.end(),b) != a.end(); }
template<typename T> inline bool contains(const set<T>& a, const T& b) { return a.find(b) != a.end(); }
template<typename T1, typename T2> inline bool contains(const map<T1,T2>& a, const T1& b) { return a.find(b) != a.end(); }

// python maps and comprehension
template<typename func>
inline auto make_vector(int nelems, const func& f) -> vector<decltype(f(0))> { auto v = vector<decltype(f(0))>(nelems); for(auto i = 0; i < nelems; i ++) v[i] = f(i); return v; }
template<typename T, typename func>
inline auto make_vector(const vector<T>& vv, const func& f) -> vector<decltype(f(vv[0]))> { auto v = vector<decltype(f(vv[0]))>(vv.size()); for(auto i = 0; i < vv.size(); i ++) v[i] = f(vv[i]); return v; }

// python-like conversion between basic types
template<typename T>
inline set<T> make_set(const vector<T>& v) { return set<T>(v.begin(),v.end()); }
template<typename T>
inline vector<T> make_vector(const set<T>& v) { return vector<T>(v.begin(),v.end()); }

inline string load_text_file(const char* filename) {
    auto text = string("");
    auto f = fopen(filename,"r");
    error_if_not(f, "could not open file: %s\n", filename);
    char line[4096];
    while (fgets(line, 4096, f)) text += line;
    fclose(f);
    return text;
}

inline string load_text_file(const string& filename) { return load_text_file(filename.c_str()); }
inline void save_text_file(const string& filename, const string& str) {
    auto f = fopen(filename.c_str(), "w");
    error_if_not(f, "could not open file: %s\n", filename.c_str());
    fwrite(str.c_str(), sizeof(char), str.length(), f);
    fclose(f);
}

struct polyline2r : vector<ym_vec2r> {
    using vector<ym_vec2r>::vector;
};

// python-like list and dictionary manipulation
inline polyline2r operator+(const polyline2r& a, const polyline2r& b) { auto ret = polyline2r(); ret.insert(ret.end(),a.begin(),a.end()); ret.insert(ret.end(),b.begin(),b.end()); return ret; }
inline polyline2r operator+(const polyline2r& a, const ym_vec2r& b) { auto ret = polyline2r(); ret.insert(ret.end(),a.begin(),a.end()); ret.push_back(b); return ret; }

inline polyline2r& operator+=(polyline2r& a, const polyline2r& b) { a.insert(a.end(),b.begin(),b.end()); return a; }
inline polyline2r& operator+=(polyline2r& a, const ym_vec2r& b) { a.push_back(b); return a; }

struct polygon2r : vector<polyline2r> {
    using vector<polyline2r>::vector;
};

#endif /* common_h */
