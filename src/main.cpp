//
//  main.cpp
//  schema
//
//  Created by Bartholomew Joyce on 02/05/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

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

static AppData data;

char DATE_TEMP[20];
const char* DAY_NAMES[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char* date_string(int index) {
    auto day = DAY_NAMES[index % 7];
    auto month = index <= 1 ? "Apr"      : index <= 32 ? "May"     : "Jun";
    auto date  = index <= 1 ? index + 29 : index <= 32 ? index - 1 : index - 32;
    
    sprintf(DATE_TEMP, "%s %s %d", day, month, date);
    return DATE_TEMP;
}

void update(Context ctx) {

    int OUTER_MARGIN = 10;
    int BIG_SPACING = 8;
    int SMALL_SPACING = 4;
    int BLOCK_SIZE = 16;
    int BLOCK_SPACING = 1;
    float ascender, descender, line_height;
    
    int x, y;
    x = OUTER_MARGIN;
    y = OUTER_MARGIN;

    nvgFontFace(ctx.vg, "regular");

    // Background
    nvgBeginPath(ctx.vg);
    nvgRect(ctx.vg, 0, 0, ctx.width, ctx.height);
    nvgFillColor(ctx.vg, nvgRGB(0x3c, 0x2c, 0x4a));
    nvgFill(ctx.vg);
    
    // Header text
    nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
    
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    nvgText(ctx.vg, x, y + ascender, date_string(0), NULL);
    y += line_height;
    
    nvgFontSize(ctx.vg, 20.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    nvgText(ctx.vg, x, y + ascender, data.module_names['B'], NULL);
    y += line_height;
    
    y += BIG_SPACING;
    
    // Revision Schedule
    nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    nvgText(ctx.vg, x, y + ascender, "Revision Schedule", NULL);
    y += line_height;
    y += SMALL_SPACING;
    
    int MAX_Y_BLOCKS = 10;
    
    for (auto& chunk : data.chunks) {
        auto x2 = x + (BLOCK_SIZE + BLOCK_SPACING) * chunk.start_date;
        auto i = 0;
        
        for (auto& pair : chunk.stock) {
            nvgFillColor(ctx.vg, data.module_colors[pair.first]);
            auto amount = pair.second;
            
            if (i > 0) {
                int blocks = amount > MAX_Y_BLOCKS - i ? MAX_Y_BLOCKS - i : amount;
                nvgBeginPath(ctx.vg);
                nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, blocks * BLOCK_SIZE);
                nvgFill(ctx.vg);
                i += blocks;
                amount -= blocks;
                if (i >= MAX_Y_BLOCKS) {
                    i = 0;
                    x2 += BLOCK_SIZE + BLOCK_SPACING;
                }
            }
            
            while (amount > 0) {
                int blocks = amount > MAX_Y_BLOCKS ? MAX_Y_BLOCKS : amount;
                nvgBeginPath(ctx.vg);
                nvgRect(ctx.vg, x2, y, BLOCK_SIZE, blocks * BLOCK_SIZE);
                nvgFill(ctx.vg);
                amount -= blocks;
                if (blocks < MAX_Y_BLOCKS) {
                    i += blocks;
                } else {
                    x2 += BLOCK_SIZE + BLOCK_SPACING;
                }
            }
        }
    }
    
    y += MAX_Y_BLOCKS * BLOCK_SIZE;
    
    y += BIG_SPACING;
    
    // Revision Reality
    nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    nvgText(ctx.vg, x, y + ascender, "Revision Reality", NULL);
    y += line_height;
    y += SMALL_SPACING;
    
    int max_blocks_in_day = 0;
    
    for (auto& pair : data.days) {
        int x2 = x + pair.first * (BLOCK_SIZE + BLOCK_SPACING);
        int i = 0;
        
        for (auto& pair2 : pair.second.stock) {
            nvgFillColor(ctx.vg, data.module_colors[pair2.first]);
            nvgBeginPath(ctx.vg);
            nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, pair2.second * BLOCK_SIZE);
            nvgFill(ctx.vg);
            i += pair2.second;
        }
        
        if (max_blocks_in_day < i) {
            max_blocks_in_day = i;
        }
    }
    
    y += max_blocks_in_day * BLOCK_SIZE;
    
    y += BIG_SPACING;
    
    // Schema
    nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    nvgText(ctx.vg, x, y + ascender, "Schema", NULL);
    y += line_height;
    
    y += SMALL_SPACING;
    for (auto& pair : data.module_names) {
    
        nvgFillColor(ctx.vg, data.module_colors[pair.first]);
        nvgBeginPath(ctx.vg);
        nvgRect(ctx.vg, x, y, line_height, line_height);
        nvgFill(ctx.vg);
    
        if (mouse_over(ctx, 0, y, ctx.width, line_height)) {
            *ctx.cursor = CURSOR_POINTING_HAND;
            nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
        } else {
            nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
        }
        
        nvgText(ctx.vg, x + line_height + SMALL_SPACING, y + ascender, pair.second, NULL);
        y += line_height;
    }
}

char* read_data_file() {
    FILE* fp = fopen("assets/data.txt", "r");
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

int main(int argc, const char** argv) {

    app::init("Schema");

    app::load_font_face("regular", "assets/SFRegular.ttf");
    
    char* buffer = read_data_file();
    data = parse_file(buffer);

    app::run(&update);

    return 0;
}

