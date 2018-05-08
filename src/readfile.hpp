//
//  readfile.hpp
//  schema
//
//  Created by Bartholomew Joyce on 06/05/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#ifndef readfile_hpp
#define readfile_hpp
#include "model.hpp"

char* read_data_file(const char* filename);
AppData parse_file(char* ch);

#endif
