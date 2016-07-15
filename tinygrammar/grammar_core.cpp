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

Grammar* main_grammar = nullptr;

int add_rule_to_mapping(Grammar* grammar, string rulename){
    if (grammar->rule_mapping.count(rulename) > 0) return grammar->rule_mapping.at(rulename);
    else {grammar->rule_mapping[rulename] = (int)(grammar->rule_mapping.size()); return grammar->rule_mapping.at(rulename);}
}

rule_tags add_tags(Grammar* grammar, vector<string> tags){
    auto res = vector<int>();
    for (auto t : tags){
    
    }
    return rule_tags();
}

Grammar* get_grammar(string filename){
    if (main_grammar == nullptr){
        auto res = new Grammar();
        
        ifstream t(filename);
        string str;
        t.seekg(0, std::ios::end);  str.reserve(t.tellg()); t.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        
        Object o;
        o.parse(str);
        cout << o.json() << endl;
        
        assert(o.has<String>("grammar_name"));
        assert(o.has<Array>("rules"));
        
        res->name = o.get<String>("grammar_name");
        res->seed = o.get<Number>("seed");
        
        auto r_arr = o.get<Array>("rules");
        for (auto i = 0; i < r_arr.size(); i++){
            auto rule_json = r_arr.get<Object>(i);
            auto rule = new Rule();
            
            rule->rule_name = add_rule_to_mapping(res, rule_json.get<String>("rule_name"));
            
            auto json_mtags = vector<string>();
            auto mt_arr = rule_json.get<Array>("matching_tags");
            for (auto j = 0; j < mt_arr.size(); j++) json_mtags.push_back(mt_arr.get<String>(j));
            rule->matching_tags = add_tags(res, json_mtags);
            
            auto json_ptags = vector<string>();
            auto pt_arr = rule_json.get<Array>("produced_tags");
            for (auto j = 0; j < pt_arr.size(); j++) json_ptags.push_back(pt_arr.get<String>(j));
            rule->produced_tags = add_tags(res, json_ptags);
            
            if (rule_json.get<String>("operator") == "split")
                rule->op = Operator(op_split);
            else if (rule_json.get<String>("operator") == "place")
                rule->op = Operator(op_place);
            else if (rule_json.get<String>("operator") == "init")
                rule->op = Operator(op_init);
            
            auto json_params = rule_params();
            auto p_arr = rule_json.get<Array>("parameters");
            for (auto j = 0; j < p_arr.size(); j++) json_params[j] = p_arr.get<Number>(j);
            rule->parameters = json_params;
            
            res->rules.push_back(rule);
        }
        main_grammar = res;
        return main_grammar;
    }
    else return main_grammar;
}

vector<Rule*> get_inits(Grammar* g){
    auto res = vector<Rule*>();
    if (g != nullptr){
        for (auto r : g->rules) if (r->op.operator_name == op_init) res.push_back(r);
    }
    return res;
}

vector<Rule*> get_rules(Grammar* g){
    if (g != nullptr) return g->rules;
    return vector<Rule*>();
}

Rule* matching_init(){
    auto grammar = get_grammar(grammar_filename);
    auto inits = get_inits(grammar);
    if (not inits.empty())
        return inits[(int)(ym_rng_nextf(&grammar->rn) * inits.size())];
    else {
        printf("Didn't find any init rule for matching\n");
        return nullptr;
    }
}

pair<PartitionShapeGroup, Rule*> matching(const ShapeGroup& active_shapes){
    auto grammar = get_grammar(grammar_filename);
    //matching of the shapes
    auto matched_shapes = matching_shapes(active_shapes);
    //matching of the rule
    if (not matched_shapes.match.empty()){
        auto rule_to_apply = matching_rule(matched_shapes.match);
        if (rule_to_apply != nullptr){
            return make_pair(matched_shapes, rule_to_apply);
        }
    }
    printf("Didn't find any rule for matching\n");
    return pair<PartitionShapeGroup, Rule*>(PartitionShapeGroup(), nullptr);
}

bool tag_in_rule(int tag, const rule_tags& tags){
    for (auto i = 0; i < TAG_SIZE; i++){
        if (tag == tags[i]) return true;
    }
    return false;
}

Rule* tangle_match_rule(Grammar* grammar, int tag){
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto matches = vector<int>();
            auto grammar = get_grammar(grammar_filename);
            auto rules = get_rules(grammar);
            for(auto i = 0; i < (int)rules.size(); i++){
                if (tag_in_rule(tag, rules[i]->matching_tags))
                    matches.push_back(i);
            }
            if(matches.empty()) return nullptr;
            return rules[(int)(ym_rng_nextf(&grammar->rn) * rules.size())];
        }
        default:
            break;
    }
    return nullptr;
}

PartitionShapeGroup matching_shapes(const ShapeGroup& active_shapes){
    auto res = PartitionShapeGroup();
    auto grammar = get_grammar(grammar_filename);
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto rule = (Rule*)nullptr;
            auto start = (TangleShape*)nullptr;
            for(auto&& shape : active_shapes) {
                rule = tangle_match_rule(grammar, ((TangleShape*)shape)->tag);
                start = ((TangleShape*)shape);
                if(rule) break;
            }
            if(not rule) return res;
            for(auto shape : active_shapes) {
                if(((TangleShape*)shape)->gid == start->gid) res.match.push_back(shape);
                else res.remainder.push_back(shape);
            }
            break;
        }
        default:
            printf("[matching_shapes] ERROR: Shouldn't have gotten here! Invalid grammar_type \n");
            break;
    }
    return res;
}

Rule* matching_rule(const ShapeGroup& matched) {
    auto grammar = get_grammar(grammar_filename);
    switch (ACTIVE_GRAMMAR) {
        case tangle_grammar:
        {
            auto shape = matched[0];
            auto rule = tangle_match_rule(grammar, ((TangleShape*)shape)->tag);
            return rule;
        }
        default:
            printf("[matching_rule] ERROR: Shouldn't have gotten here! Invalid grammar_type \n");
            break;
    }
    return nullptr;
}