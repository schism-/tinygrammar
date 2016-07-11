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
#include "tree.hh"
#include "yocto_math.h"

using namespace std;

#define PARAM_SIZE 16
#define TAG_SIZE 4

#define ACTIVE_GRAMMAR 0

static constexpr double resolution = 2;

enum {
    basic_shape = 0,
    tangle_shape,
    annotated_shape,
    anim_shape
};

enum {
    linear_history = 0,
    tree_history = 1
};

enum {
    tangle_grammar = 0,
    learning_grammar,
    animation_grammar
};

enum {
    op_split = 0,
    op_place = 1
};

typedef ym_vec<double, 2> ym_vec2r;

typedef ym_rng_pcg32 rng;

typedef ym_range<ym_vec2r> ym_range2r;

typedef ym_affine<double, 2, true> ym_frame2r;
const ym_frame2r ym_identity_frame2r = ym_frame2r{{1, 0}, {0, 1}, {0, 0}};

typedef ym_vec<double, PARAM_SIZE> rule_params;
typedef ym_vec<int, TAG_SIZE> rule_tags;


static string grammar_filename = "grammars/test_grammar.json";



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

#endif /* common_h */
