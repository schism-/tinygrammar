//
//  grammar_core.cpp
//  tinygrammar
//
//  Created by tangles on 6/15/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "grammar_core.h"

#include <fstream>
#include <streambuf>
#include "jsonxx.h"

using namespace jsonxx;

#define ACTIVE_GRAMMAR 0

Grammar* main_grammar = nullptr;

Grammar* get_grammar(string filename){
    if (main_grammar == nullptr){
        auto res = new Grammar();
        
        std::ifstream t(filename);
        std::string str;
        
        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        
        str.assign((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
        
        // Parse string or stream
        Object o;
        cout << o.parse(str) << endl;
        
        return res;
    }
    else return main_grammar;
}

vector<Rule*> get_rules(Grammar* g){
    if (g != nullptr) return g->rules;
    return vector<Rule*>();
}

ShapeGroup* matching(ShapeGroup* active_shapes){
    auto grammar = get_grammar("grammars/test_grammar.json");
    //matching of the shapes
    auto matched_shapes = matching_shapes(active_shapes);
    
    //matching of the rule
    if (matched_shapes){
        auto rule_to_apply = matching_rule(matched_shapes);
        //call operator
        
        //retrieve and return results
    }
    return new ShapeGroup();
}

Rule* tangle_match_rule(int tag){
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto matches = vector<int>();
            auto grammar = get_grammar("");
            auto rules = get_rules(grammar);
            for(auto i = 0; i < (int)rules.size(); i++){
                if (find(rules[i]->matching_tags.begin(), rules[i]->matching_tags.end(), tag) != rules[i]->matching_tags.end())
                    matches.push_back(i);
            }
            if(matches.empty()) return nullptr;
            return rules[0];
        }
        default:
            break;
    }
    return nullptr;
}

ShapeGroup* matching_shapes(ShapeGroup* active_shapes){
    auto res = new ShapeGroup();
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto rule = (Rule*)nullptr;
            auto start = (TangleShape*)nullptr;
            for(auto&& shape : *active_shapes) {
                rule = tangle_match_rule(((TangleShape*)shape)->tag);
                start = ((TangleShape*)shape);
                if(rule) break;
            }
            if(not rule) return nullptr;
            for(auto shape : *active_shapes) {
                if(((TangleShape*)shape)->gid == start->gid) res->push_back(shape);
            }
            break;
        }
        default:
            printf("Shouldn't have gotten here! Invalid grammar_type \n");
            break;
    }
    return res;
}

Rule* matching_rule(ShapeGroup* matched) {
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto shape = &matched[0];
            auto rule = tangle_match_rule(((TangleShape*)shape)->tag);
            return rule;
        }
        default:
            printf("Shouldn't have gotten here! Invalid grammar_type \n");
            break;
    }
    return nullptr;
}