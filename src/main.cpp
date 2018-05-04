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
#include <ddui/views/ScrollArea>
#include <ddui/views/PlainTextBox>

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

enum Selection {
    NO_SELECTION,
    SELECTION_MODULE,
    SELECTION_STOCK,
    SELECTION_SECTION
};

static AppData data;
static ScrollArea::ScrollAreaState scroll_area_1;
static ScrollArea::ScrollAreaState scroll_area_2;
static TextEdit::Model text_model;
static PlainTextBox::PlainTextBoxState text_state;

char DATE_TEMP[20];
const char* DAY_NAMES[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char* date_string(int from, int to) {
    auto day1 = DAY_NAMES[from % 7];
    auto month1 = from <= 1 ? "Apr"     : from <= 32 ? "May"    : "Jun";
    auto date1  = from <= 1 ? from + 29 : from <= 32 ? from - 1 : from - 32;
    
    auto day2 = DAY_NAMES[to % 7];
    auto month2 = to <= 1 ? "Apr"   : to <= 32 ? "May"  : "Jun";
    auto date2  = to <= 1 ? to + 29 : to <= 32 ? to - 1 : to - 32;
    
    if (from != to) {
        sprintf(DATE_TEMP, "%s %s %d – %s %s %d", day1, month1, date1, day2, month2, date2);
    } else {
        sprintf(DATE_TEMP, "%s %s %d", day1, month1, date1);
    }
    return DATE_TEMP;
}

void update(Context ctx) {

    auto selection = NO_SELECTION;
    int selected_date_from = -1;
    int selected_date_to = -1;
    char selected_module = '\0';

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
    
    // Header text space
    auto header_saved_y = y;
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    y += line_height;
    
    nvgFontSize(ctx.vg, 20.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
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
                if (mouse_over(ctx, x2, y + i * BLOCK_SIZE, BLOCK_SIZE + BLOCK_SPACING, blocks * BLOCK_SIZE)) {
                    *ctx.cursor = CURSOR_POINTING_HAND;
                    selection = SELECTION_STOCK;
                    selected_date_from = chunk.start_date;
                    selected_date_to = chunk.end_date;
                    selected_module = pair.first;
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
                    *ctx.cursor = CURSOR_POINTING_HAND;
                    selection = SELECTION_STOCK;
                    selected_date_from = chunk.start_date;
                    selected_date_to = chunk.end_date;
                    selected_module = pair.first;
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
            if (mouse_over(ctx, x2, y + i * BLOCK_SIZE, BLOCK_SIZE + BLOCK_SPACING, pair2.second * BLOCK_SIZE)) {
                *ctx.cursor = CURSOR_POINTING_HAND;
                selection = SELECTION_SECTION;
                selected_date_from = selected_date_to = pair.first;
                selected_module = pair2.first;
            }
            nvgFillColor(ctx.vg, data.module_colors[pair2.first]);
            nvgBeginPath(ctx.vg);
            nvgRect(ctx.vg, x2, y + i * BLOCK_SIZE, BLOCK_SIZE, pair2.second * BLOCK_SIZE - BLOCK_SPACING);
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
            selection = SELECTION_MODULE;
            selected_module = pair.first;
            nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
        } else {
            nvgFillColor(ctx.vg, nvgRGBA(0xff, 0xff, 0xff, 0x80));
        }
        
        nvgText(ctx.vg, x + line_height + SMALL_SPACING, y + ascender, pair.second, NULL);
        y += line_height;
    }
    
    // Draw selection
    switch (selection) {
        case NO_SELECTION:
            break;
        case SELECTION_MODULE:
            break;
        case SELECTION_STOCK:
            break;
        case SELECTION_SECTION:
            break;
    }
    
    // Header text
    y = header_saved_y;
    nvgFillColor(ctx.vg, nvgRGB(0xff, 0xff, 0xff));
    
    nvgFontSize(ctx.vg, 16.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    if (selected_date_from != -1) {
        nvgText(ctx.vg, x, y + ascender, date_string(selected_date_from, selected_date_to), NULL);
    }
    y += line_height;
    
    nvgFontSize(ctx.vg, 20.0);
    nvgTextMetrics(ctx.vg, &ascender, &descender, &line_height);
    if (selected_module != '\0') {
        nvgText(ctx.vg, x, y + ascender, data.module_names[selected_module], NULL);
    }
    y += line_height;
    
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
    
    auto buffer = new char[fsize + 1];
    fread(buffer, fsize, 1, fp);
    fclose(fp);
    
    buffer[fsize] = '\0';
    
    return buffer;
}

void read_string(char** ch, const char* str) {
    while (**ch != '\0' && *str != '\0') {
        if (**ch != *str) {
            printf("Expected: '%c' got '%c'\n", *str, **ch);
            throw "Parse error";
        }
        
        *ch += 1;
        str++;
    }
    
    if (*str != '\0') {
        printf("Expected '%c' got EOF\n", *str);
        throw "Parse error";
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
        throw "Parse error";
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
            throw "Parse error";
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

#define NEED_LINE(lineno) if (text_model.lines.size() < lineno) throw "Parse error"
#define LINE(lineno) text_model.lines[lineno].content.get()
#define MATCH(ch, str) if (strcmp(ch, str) != 0) throw "Parse error"

AppData parse_file() {
    TextEdit::set_style(&text_model, false, 16.0, nvgRGBA(0xff, 0xff, 0xff, 0x80));
    
    TextEdit::StyleCommand style;
    style.type = TextEdit::StyleCommand::COLOR;
    style.color_value = nvgRGB(0xff, 0xff, 0xff);
    
    TextEdit::Selection selection;

    AppData data;

    int lineno = 0;

    NEED_LINE(lineno);
    MATCH(LINE(lineno), "MODULES");
    selection.a_line = selection.b_line = lineno;
    selection.a_index = 0;
    selection.b_index = 7;
    TextEdit::apply_style(&text_model, selection, style);
    lineno++;
    while (true) {
        NEED_LINE(lineno);
        if (LINE(lineno)[0] == '\0') {
            break;
        }
        
        auto ch = LINE(lineno);
        char label = *ch++;
        eat_spaces(&ch);
        
        auto color = read_color(&ch);
        eat_spaces(&ch);
        
        char* module_name = read_until(&ch, '\0');
        data.module_names.insert(std::make_pair(label, module_name));
        data.module_colors.insert(std::make_pair(label, color));
        
        lineno++;
    }
    
    lineno++;
    NEED_LINE(lineno);
    MATCH(LINE(lineno), "CHUNKS");
    selection.a_line = selection.b_line = lineno;
    selection.a_index = 0;
    selection.b_index = 6;
    TextEdit::apply_style(&text_model, selection, style);
    lineno++;
    while (true) {
        NEED_LINE(lineno);
        if (LINE(lineno)[0] == '\0') {
            break;
        }
        
        auto ch = LINE(lineno);
    
        Chunk chunk;

        chunk.start_date = read_date(&ch);
        eat_spaces(&ch);

        chunk.end_date = read_date(&ch);
        eat_spaces(&ch);

        while (*ch != '\0') {
            chunk.stock.push_back(read_stock(&ch));
            eat_spaces(&ch);
        }

        data.chunks.push_back(std::move(chunk));

        lineno++;
    }

    lineno++;
    NEED_LINE(lineno);
    MATCH(LINE(lineno), "DAYS");
    selection.a_line = selection.b_line = lineno;
    selection.a_index = 0;
    selection.b_index = 4;
    TextEdit::apply_style(&text_model, selection, style);
    lineno++;
    for (; lineno < text_model.lines.size(); lineno++) {
        if (LINE(lineno)[0] == '\0') {
            break;
        }
        
        auto ch = LINE(lineno);
    
        Day day;

        int date = read_date(&ch);
        eat_spaces(&ch);

        while (*ch != '\0' && *ch != '\n') {
            day.stock.push_back(read_stock(&ch));
            eat_spaces(&ch);
        }

        data.days.insert(std::make_pair(date, std::move(day)));
    }
    
    return data;
}

int main(int argc, const char** argv) {

    app::init("Schema");

    app::load_font_face("regular", "assets/SFRegular.ttf");
    app::load_font_face("mono", "assets/PTMono.ttf");
    
    char* buffer = read_data_file();
    
    text_model.regular_font = "mono";
    text_model.bold_font = "mono";
    TextEdit::set_style(&text_model, false, 16.0, nvgRGBA(0xff, 0xff, 0xff, 0x80));
    
    text_state.model = &text_model;
    text_state.multiline = true;
    
    text_state.bg_color = text_state.bg_color_focused = nvgRGB(0x3c, 0x2c, 0x4a);
    text_state.border_width = 0;
    text_state.border_radius = 0;
    text_state.cursor_color = nvgRGB(0xff, 0xff, 0xff);
    
    TextEdit::set_text_content(&text_model, buffer);

    app::run([](Context ctx) {
    
        nvgFillColor(ctx.vg, nvgRGB(0x3c, 0x2c, 0x4a));
        nvgBeginPath(ctx.vg);
        nvgRect(ctx.vg, 0, 0, ctx.width, ctx.height);
        nvgFill(ctx.vg);
        
        float ratio = 0.64;
        
        {
            auto child_ctx = child_context(ctx, 0, 0, ctx.width * ratio, ctx.height);
            ScrollArea::update(&scroll_area_1, child_ctx, 717, 590, update);
            nvgRestore(ctx.vg);
        }
        
        {
            try {
                auto new_data = parse_file();
                data = new_data;
            } catch (const char* ex) { }
        
            auto child_ctx = child_context(ctx, ctx.width * ratio, 0, ctx.width * (1.0 - ratio), ctx.height);
            PlainTextBox::update(&text_state, child_ctx);
            nvgRestore(ctx.vg);
        }
    });

    return 0;
}

