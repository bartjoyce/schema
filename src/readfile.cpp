//
//  readfile.cpp
//  schema
//
//  Created by Bartholomew Joyce on 06/05/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#include "readfile.hpp"

char* read_data_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Couldn't open data.txt\n");
        exit(0);
    }
    
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);  //same as rewind(f);
    
    char* buffer = (char*)malloc(fsize + 1);
    fread(buffer, fsize, 1, fp);
    fclose(fp);
    
    return buffer;
}

void read_string(char** ch, const char* str) {
    while (**ch != '\0' && *str != '\0') {
        if (**ch != *str) {
            printf("Expected: '%c' got '%c'\n", *str, **ch);
            exit(0);
        }
        
        *ch += 1;
        str++;
    }
    
    if (*str != '\0') {
        printf("Expected '%c' got EOF\n", *str);
    }
}

void eat_spaces(char** ch) {
    while (**ch != '\0' && **ch == ' ') {
        *ch += 1;
    }
}

void eat_until(char** ch, char until) {
    while (**ch != '\0' && **ch != until && **ch != '\n') {
        *ch += 1;
    }
}

char* read_until(char** ch, char until) {
    auto end_ch = *ch;
    eat_until(&end_ch, until);
    auto result = new char[end_ch - *ch + 1];
    strncpy(result, *ch, end_ch - *ch);
    result[end_ch - *ch] = '\0';
    *ch = *end_ch == until ? end_ch + 1 : end_ch;
    return result;
}

int read_number(char** ch) {
    int number = 0;
    while (**ch >= '0' && **ch <= '9') {
        number = number * 10 + **ch - '0';
        *ch += 1;
    }
    return number;
}

int read_date(char** ch) {
    int day = read_number(ch);
    int month = **ch == 'A' ? 0 : **ch == 'M' ? 1 : **ch == 'J' ? 2 : -1;
    if (month == -1) {
        exit(1);
    }
    
    *ch += 3;
    return month == 0 ? day - 29 : month == 1 ? day + 1 : day + 32;
}

unsigned char read_hex_byte(char** ch) {
    int number = 0;
    for (int i = 0; i < 2; ++i) {

        if (**ch >= '0' && **ch <= '9') {
            number = number * 16 + **ch - '0';
        } else if (**ch >= 'a' && **ch <= 'f') {
            number = number * 16 + **ch - 'a' + 10;
        } else if (**ch >= 'A' && **ch <= 'F') {
            number = number * 16 + **ch - 'A' + 10;
        } else {
            exit(1);
        }
        
        *ch += 1;
        
    }
    
    return (unsigned char)number;
}

NVGcolor read_color(char** ch) {
    read_string(ch, "#");
    
    auto r = read_hex_byte(ch);
    auto g = read_hex_byte(ch);
    auto b = read_hex_byte(ch);
    
    return nvgRGB(r, g, b);
}

std::pair<char, int> read_stock(char** ch) {
    char label = **ch;
    *ch += 1;
    return std::make_pair(label, read_number(ch));
}

AppData parse_file(char* ch) {
    AppData data;

    read_string(&ch, "MODULES\n");
    while (*ch != '\0' && *ch != '\n') {
        char label = *ch++;
        eat_spaces(&ch);
        
        auto color = read_color(&ch);
        eat_spaces(&ch);
        
        char* module_name = read_until(&ch, '\n');
        data.module_names.insert(std::make_pair(label, module_name));
        data.module_colors.insert(std::make_pair(label, color));
    }
    
    read_string(&ch, "\nCHUNKS\n");
    while (*ch != '\0' && *ch != '\n') {
        Chunk chunk;
        
        chunk.start_date = read_date(&ch);
        eat_spaces(&ch);
        
        chunk.end_date = read_date(&ch);
        eat_spaces(&ch);
        
        while (*ch != '\0' && *ch != '\n') {
            chunk.stock.push_back(read_stock(&ch));
            eat_spaces(&ch);
        }
        
        data.chunks.push_back(std::move(chunk));
        
        if (*ch == '\n') ++ch;
    }
    
    read_string(&ch, "\nDAYS\n");
    while (*ch != '\0' && *ch != '\n') {
        Day day;
    
        int date = read_date(&ch);
        eat_spaces(&ch);
        
        while (*ch != '\0' && *ch != '\n') {
            day.stock.push_back(read_stock(&ch));
            eat_spaces(&ch);
        }
        
        data.days.insert(std::make_pair(date, std::move(day)));
        
        if (*ch == '\n') ++ch;
    }
    
    return data;
}
