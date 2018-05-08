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

#include "model.hpp"
#include "readfile.hpp"

static AppData data;

const char* FILE_NAME = "assets/data.txt";

const char* DAY_NAMES[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
void date_string(char* dst, int from, int to) {
    auto day1 = DAY_NAMES[from % 7];
    auto month1 = from <= 1 ? "Apr"     : from <= 32 ? "May"    : "Jun";
    auto date1  = from <= 1 ? from + 29 : from <= 32 ? from - 1 : from - 32;
    
    auto day2 = DAY_NAMES[to % 7];
    auto month2 = to <= 1 ? "Apr"   : to <= 32 ? "May"  : "Jun";
    auto date2  = to <= 1 ? to + 29 : to <= 32 ? to - 1 : to - 32;
    
    if (from != to) {
        sprintf(dst, "%s %s %d – %s %s %d", day1, month1, date1, day2, month2, date2);
    } else {
        sprintf(dst, "%s %s %d", day1, month1, date1);
    }
}

void update(Context ctx) {

    if (mouse_hit(ctx, 0, 0, ctx.width, ctx.height)) {
        ctx.mouse->accepted = true;
        char* buffer = read_data_file(FILE_NAME);
        data = parse_file(buffer);
        delete buffer;
    }

    char context_line_1[64];
    char context_line_2[64];
    context_line_1[0] = context_line_2[0] = '\0';

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
    nvgFillColor(ctx.vg, nvgRGB(0x3a, 0x23, 0x4c));
    nvgBeginPath(ctx.vg);
    nvgRect(ctx.vg, 0, 0, ctx.width, ctx.height);
    nvgFill(ctx.vg);
    
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
        
            auto draw_stock = [&]() {
                bool selected = false;
                
                int amount = pair.second;
                
                if (i > 0) {
                    int blocks = amount > MAX_Y_BLOCKS - i ? MAX_Y_BLOCKS - i : amount;
                    if (mouse_over(ctx, x2, y + i * BLOCK_SIZE, BLOCK_SIZE + BLOCK_SPACING, blocks * BLOCK_SIZE)) {
                        selected = true;
                    }
                    nvgBeginPath(ctx.vg);
                    nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, blocks * BLOCK_SIZE - BLOCK_SPACING);
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
                    if (mouse_over(ctx, x2, y, BLOCK_SIZE + BLOCK_SPACING, blocks * BLOCK_SIZE)) {
                        selected = true;
                    }
                    nvgBeginPath(ctx.vg);
                    nvgRect(ctx.vg, x2, y, BLOCK_SIZE, blocks * BLOCK_SIZE - BLOCK_SPACING);
                    nvgFill(ctx.vg);
                    amount -= blocks;
                    if (blocks < MAX_Y_BLOCKS) {
                        i += blocks;
                    } else {
                        x2 += BLOCK_SIZE + BLOCK_SPACING;
                    }
                }

                return selected;
            };
        
            nvgFillColor(ctx.vg, data.module_colors[pair.first]);
            
            int old_i = i;
            int old_x2 = x2;
            if (draw_stock()) {
                *ctx.cursor = CURSOR_POINTING_HAND;
                date_string(context_line_2, chunk.start_date, chunk.end_date);
                if (pair.second % 2 == 0) {
                    sprintf(context_line_1, "%s – %dh", context_line_2, pair.second / 2);
                } else if (pair.second == 1) {
                    sprintf(context_line_1, "%s – 30m", context_line_2);
                } else {
                    sprintf(context_line_1, "%s – %dh30m", context_line_2, pair.second / 2);
                }
                strcpy(context_line_2, data.module_names[pair.first]);
            
                i = old_i;
                x2 = old_x2;
                
                nvgFillColor(ctx.vg, nvgRGBA(0x00, 0x00, 0x00, 0x40));
                draw_stock();
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
            nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, pair2.second * BLOCK_SIZE - BLOCK_SPACING);
            nvgFill(ctx.vg);
            
            if (mouse_over(ctx, x2, y + i * BLOCK_SIZE, BLOCK_SIZE + BLOCK_SPACING, pair2.second * BLOCK_SIZE)) {
                *ctx.cursor = CURSOR_POINTING_HAND;
                date_string(context_line_2, pair.first, pair.first);
                if (pair2.second % 2 == 0) {
                    sprintf(context_line_1, "%s – %dh", context_line_2, pair2.second / 2);
                } else if (pair2.second == 1) {
                    sprintf(context_line_1, "%s – 30m", context_line_2);
                } else {
                    sprintf(context_line_1, "%s – %dh30m", context_line_2, pair2.second / 2);
                }
                strcpy(context_line_2, data.module_names[pair2.first]);
                
                nvgFillColor(ctx.vg, nvgRGBA(0x00, 0x00, 0x00, 0x40));
                nvgBeginPath(ctx.vg);
                nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, pair2.second * BLOCK_SIZE - BLOCK_SPACING);
                nvgFill(ctx.vg);
            }
            
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
            strcpy(context_line_2, data.module_names[pair.first]);
            nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
        } else {
            nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
        }
        
        nvgText(ctx.vg, x + line_height + SMALL_SPACING, y + ascender, pair.second, NULL);
        y += line_height;
    }
    
    // Context overlay
    if (context_line_1[0] || context_line_2[0]) {
        int width = 0, height = 0;
        int line_1_y, line_2_y;
        
        height += SMALL_SPACING;
        
        if (context_line_1[0]) {
            float bounds[4];
            nvgFontSize(ctx.vg, 16.0);
            nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
            nvgTextBounds(ctx.vg, 0, 0, context_line_1, NULL, bounds);
            
            line_1_y = height + ascender;
            width = width < bounds[2] ? bounds[2] : width;
            height += line_height + SMALL_SPACING;
        }
        
        if (context_line_2[0]) {
            float bounds[4];
            nvgFontSize(ctx.vg, 20.0);
            nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
            nvgTextBounds(ctx.vg, 0, 0, context_line_2, NULL, bounds);
            
            line_2_y = height + ascender;
            width = width < bounds[2] ? bounds[2] : width;
            height += line_height + SMALL_SPACING;
        }
        
        width += 2 * SMALL_SPACING;
        
        int x = ctx.mouse->x - ctx.x + 5;
        int y = ctx.mouse->y - ctx.y + 10;
        
        if (x + width > ctx.width) {
            x = ctx.width - width;
        }
        if (y + height > ctx.height) {
            y = ctx.height - height;
        }
    
        nvgFillColor(ctx.vg, nvgRGBA(0x00, 0x00, 0x00, 0xbb));
        nvgBeginPath(ctx.vg);
        nvgRect(ctx.vg, x, y, width, height);
        nvgFill(ctx.vg);
        
        nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
        
        if (context_line_1[0]) {
            nvgFontSize(ctx.vg, 16.0);
            nvgText(ctx.vg, x + SMALL_SPACING, y + line_1_y, context_line_1, NULL);
        }
        
        if (context_line_2[0]) {
            nvgFontSize(ctx.vg, 20.0);
            nvgText(ctx.vg, x + SMALL_SPACING, y + line_2_y, context_line_2, NULL);
        }
    }
    
}

int main(int argc, const char** argv) {

    app::init("Schema");

    app::load_font_face("regular", "assets/SFRegular.ttf");
    app::load_font_face("mono", "assets/PTMono.ttf");
    
    char* buffer = read_data_file(FILE_NAME);
    data = parse_file(buffer);

    app::run(update);

    return 0;
}

