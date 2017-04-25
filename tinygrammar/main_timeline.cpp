//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "time_manager.h"
#include "expansion_manager.h"
#include "input_figures.h"
#include "svg.h"


void printTimeLine(Grammar* g, TimeManager::TimeLine* t){
    int scale = 80;
    std::cout << std::endl;
    std::cout << " ~~~ Printing Timeline ~~~ " << std::endl;
    auto k = 0;
    for (auto&& ntl : t->timelines){
        std::cout << (k++) << " " << mapping_to_tag(g, ntl->node->shapes[0]->tag) << "\t|";
        auto ntl_duration = ntl->duration;
        char slice_char = '-';
        for (auto&& s : ntl->slices){
            int num = (int)(scale * (s->duration / ntl_duration));
            std::cout << "{";
            std::cout << std::string(num, slice_char);
            std::cout << "}";
        }
        std::cout << "|" << std::endl;
        
        std::cout << "\t\t|";
        slice_char = ' ';
        for (auto&& s : ntl->slices){
            int num = (int)(scale * (s->duration / ntl_duration));
            stringstream ss;
            ss << mapping_to_tag(g, s->ts_tag) << " (" << s->duration << ")";
            int padding = num - (int)ss.str().length();
            
            if (padding > 0){
                std::cout << "{" << std::string(padding / 2, slice_char);
                
                std::cout << ss.str();
                
                if (padding % 2 == 0) std::cout << std::string(padding / 2, slice_char);
                else std::cout << std::string(padding / 2 + 1, slice_char);
            }
            else {
                padding = ym_clamp(num - (int)(mapping_to_tag(g, s->ts_tag).length()), 0, 10);
                std::cout << "{" << std::string(padding / 2, slice_char);
                
                std::cout << mapping_to_tag(g, s->ts_tag);
                
                if (padding % 2 == 0) std::cout << std::string(padding / 2, slice_char);
                else std::cout << std::string(padding / 2 + 1, slice_char);
            }
            
            std::cout << "}";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, const char * argv[]) {
    auto input_grammar = std::string(argv[1]);
    auto input_svg = std::string(argv[2]);
    auto input_out = std::string(argv[3]);
    
    grammar_filename = input_grammar;
    svgout_filename = input_out;
    
    auto em = (HistoryAnim*)(make_history(animation_history));
    auto grammar = get_grammar(grammar_filename);
//    auto tree = initialize_tree(grammar, 3, 5, "resources/svg/teaser.svg");
//    auto tree = initialize_tree(grammar, 2, 7, "resources/svg/test_tagged_2.svg"); // squares and circles
//    auto tree = initialize_tree(grammar, 2, 2, "resources/svg/test_tagged_2.svg"); // squares and circles
//    auto tree = initialize_tree(grammar, 3, 5, "resources/svg/edo_square4000.svg");
    auto tree = initialize_tree(grammar, 3, 5, input_svg);
    
    auto init_step = matching_init();
    auto init_shapes = init_step->op.apply(ShapeGroup(), init_step->produced_tags, init_step->parameters, grammar->rn, nullptr, nullptr, tree);
    auto init_partition = PartitionShapeGroup();
    init_partition.added = init_shapes;
    init_partition.remainder = ShapeGroup();
    init_partition.match = ShapeGroup();
    
    update_history(em, init_partition, init_step, tree);
    
    ((ExpansionAnim*)(em->history.back()))->tree = tree;
    
    //printTimeLine(grammar, ((ExpansionAnim*)(em->history.back()))->timeline);
    std::chrono::steady_clock::time_point exp_begin = std::chrono::steady_clock::now();
    int step = 0;
    while (expand(em)){
        printf("Expanding %d...", step);
        step++;
        //printTimeLine(grammar, ((ExpansionAnim*)(em->history.back()))->timeline);
    };
    printTimeLine(grammar, ((ExpansionAnim*)(em->history.back()))->timeline);
    std::chrono::steady_clock::time_point exp_end= std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(exp_end - exp_begin).count() <<std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (exp_end - exp_begin).count() <<std::endl;

    
    auto last_exp = ((ExpansionAnim*)(em->history.back()));
    auto duration = last_exp->timeline->duration;
    double frame_rate = grammar->framerate;
    double frame_step = 1.0 / frame_rate;
    auto k = 1;
    
    stringstream ss;
    ss << std::setfill('0') << std::setw(3) << 0;
    
    auto bbox = grammar->output_bbox;
    auto size = ym_rsize(bbox);
    
    std::chrono::steady_clock::time_point anim_begin = std::chrono::steady_clock::now();
    TimeManager::AnimateTimeLine(last_exp->timeline, last_exp->tree, 0.0, frame_step);
    save_svg(last_exp->tree, {(int)size.x, (int)size.y}, {size.x/2.0, size.y/2.0}, {3.0, 3.0}, ss.str());
    if (grammar->dry_run) return 0;
    
    for (auto i = frame_step; (i - duration) <= EPS_2_3; i = i + frame_step){
        if (IS_DEBUG) printf("Animating frame %d\n", k);
        else  { printf("#%d...\n", k); fflush(stdout); }
        TimeManager::AnimateTimeLine(last_exp->timeline, last_exp->tree, ym_clamp(i, 0.0, duration), frame_step);
        stringstream ss1;
        ss1 << std::setfill('0') << std::setw(3) << k;
        save_svg(last_exp->tree, {(int)size.x, (int)size.y}, {size.x/2.0, size.y/2.0}, {3.0, 3.0}, ss1.str());
        k++;
    }
    
    std::chrono::steady_clock::time_point anim_end= std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(anim_end - anim_begin).count() <<std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (anim_end - anim_begin).count() <<std::endl;
    
    // !!!!!!!!!!!!!!!! TODO IMPORTANT !!!!!!!!!!!!!!!!!!!
    // + OPTIMIZATION OF ANIMATION ROUTINE -> ONE TREE UPDATE ONCE *ALL* LEAVES ARE UPDATED
    // + fix offset and document size of saved svg
    // + test animation with big svg
    // + consider change resolution of imported svg
    // + consider implementing a wandering behaviour -> same as move_towards but with random directions (maybe perlin to smooth?)
    
    //convert -density 40 -resize 500x500 svg/*.svg -set filename:base "%[base]" png/"%[filename:base].png"
    //convert -delay 1/8 -loop 0 png/*.png animated4.gif
    
    printf("end main\n");
}
