//
//  model.hpp
//  schema
//
//  Created by Bartholomew Joyce on 06/05/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <ddui/app>
#include <stdio.h>
#include <map>
#include <vector>

struct Chunk {
    int start_date;
    int end_date;
    
    std::vector<std::pair<char, int>> stock;
};
struct Day {
    std::vector<std::pair<char, int>> stock;
};
struct AppData {
    std::map<char, char*> module_names;
    std::map<char, NVGcolor> module_colors;
    std::vector<Chunk> chunks;
    std::map<int, Day> days;
};

#endif
