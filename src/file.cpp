#ifndef FILE_HANDLER
#define FILE_HANDLER
#include "drawing.cpp"
#include <SDL2/SDL_keycode.h>
#include <climits>
#include <ext/rope>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
typedef __gnu_cxx::crope rope;
enum Direction { UP, DOWN, LEFT, RIGHT };
#define SPEED 2
#define BREAK 30
bool Pressed(int num) {
    if (num==1) return num;
    if (num>=BREAK&&((num-BREAK)%SPEED==0)) return 1;
    return 0;
}
class File {
    rope data;
    size_t cursor;
    size_t col=0,row=0;
    size_t savepos=0;
    size_t size;
    Window* window;
    size_t count_total_lines() const {
        size_t lines = 0;
        for (size_t i=0;i<data.size();i++) {
            if (data[i] == '\n') lines++;
        }
        return lines;
    }
    size_t prev_newline(size_t pos) const {
        if (pos==0) return (size_t)-1;
        for (size_t i=pos-1;i!=(size_t)-1;i--) {
            if (data[i] == '\n') return i;
            if (i == 0) break;
        }
        return (size_t)-1;
    }
    size_t next_newline(size_t pos) const {
        for (size_t i = pos; i < data.size(); ++i) {
            if (data[i] == '\n') return i;
        }
        return data.size();
    }
    size_t get_line_start(size_t pos) const {
        size_t line_start = prev_newline(pos);
        return (line_start == (size_t)-1) ? 0 : line_start + 1;
    }
    size_t get_line_end(size_t pos) const {
        return next_newline(pos);
    }
    void update_column() {
        size_t line_start = get_line_start(cursor);
        col = cursor - line_start;
    }
    void update_row_col() {
        row = 0;
        for (size_t i = 0; i < cursor && i < data.size(); ++i) {
            if (data[i] == '\n') row++;
        }
        update_column();
    }
public:
    std::pair<int,int> mousePos() {
        return {col,row};
    }
    File(const std::string& filename,Window* w) : cursor(0),window(w),col(0),row(0),savepos(0) {
        std::ifstream file(filename);
        if (!file) {
            data = rope("");
            size = 0;
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        data = rope(buffer.str().c_str());
        size = data.size();
    }
    rope& getrope() {
        return data;
    }
    void move(Direction d) {
        switch(d) {
            case LEFT:
                if (cursor > 0) {
                    cursor--;
                    if (cursor < data.size() && data[cursor] == '\n') {
                        if (row > 0) row--;
                        update_column();
                    } else {
                        col = (col > 0) ? col - 1 : 0;
                    }
                }
                savepos = col;
                break;
            case RIGHT:
                if (cursor < size) {
                    if (cursor < data.size() && data[cursor] == '\n') {
                        row++;
                        col = 0;
                    } else {
                        col++;
                    }
                    cursor++;
                }
                savepos = col;
                break;
            case UP: {
                if (row > 0) {
                    row--;
                    size_t current_line_start = get_line_start(cursor);
                    if (current_line_start > 0) {
                        size_t prev_line_end = current_line_start - 1;
                        size_t prev_line_start = get_line_start(prev_line_end);
                        size_t prev_line_len = prev_line_end - prev_line_start;
                        size_t target_col = (savepos <= prev_line_len) ? savepos : prev_line_len;
                        cursor = prev_line_start + target_col;
                        col = target_col;
                    }
                }
                break;
            }
            case DOWN: {
                size_t total_lines = count_total_lines();
                if (row < total_lines) {
                    row++;
                    size_t current_line_end = get_line_end(cursor);
                    if (current_line_end < size) {
                        size_t next_line_start = current_line_end + 1;
                        size_t next_line_end = get_line_end(next_line_start);
                        size_t next_line_len = next_line_end - next_line_start;
                        size_t target_col = (savepos <= next_line_len) ? savepos : next_line_len;
                        cursor = next_line_start + target_col;
                        col = target_col;
                    } else {
                        cursor = size;
                        update_column();
                    }
                } else {
                    cursor = size;
                    update_column();
                }
                break;
            }
        }
        if (cursor > size) {
            cursor = size;
            update_row_col();
        }
    }
    void insert(char c) {
        if (cursor > size) cursor = size;
        data.insert(cursor, &c, 1);
        cursor++;
        size++;
        if (c == '\n') {
            col = 0;
            row++;
            savepos = 0;
        } else {
            col++;
            savepos = col;
        }
    }
    void remove() {
        if (cursor == 0 || size == 0) return;
        char removed = data[cursor - 1];
        data.erase(cursor - 1, 1);
        cursor--;
        size--;
        if (removed == '\n') {
            if (row > 0) row--;
            update_column();
            savepos = col;
        } else {
            col = (col > 0) ? col - 1 : 0;
            savepos = col;
        }
        if (cursor > size) {
            cursor = size;
            update_row_col();
            savepos = col;
        }
    }
    void updateFromWindow() {
        bool shift_pressed = window->keyspressed[SDLK_LSHIFT] || window->keyspressed[SDLK_RSHIFT];
        bool caps_lock_on = (SDL_GetModState() & KMOD_CAPS) != 0;
        int min_frame = INT_MAX;
        char inserted_char = 0;
        bool key_pressed = false;
        for (SDL_Keycode key = SDLK_SPACE; key <= SDLK_z; ++key) {
            int frame_count = window->keyspressed[key];
            if (frame_count == 0 || frame_count >= min_frame) continue;
            min_frame = frame_count;
            key_pressed = true;
            if (key >= SDLK_a && key <= SDLK_z) {
                inserted_char = 'a' + (key - SDLK_a);
                if ((shift_pressed && !caps_lock_on) || (!shift_pressed && caps_lock_on)) {
                    inserted_char = inserted_char - 'a' + 'A';
                }
            }
            else if (key >= SDLK_0 && key <= SDLK_9) {
                if (shift_pressed) {
                    const char shift_nums[] = ")!@#$%^&*(";
                    inserted_char = shift_nums[key - SDLK_0];
                } else {
                    inserted_char = '0' + (key - SDLK_0);
                }
            }
            else {
                switch(key) {
                    case SDLK_SPACE: inserted_char = ' '; break;
                    case SDLK_PERIOD: inserted_char = shift_pressed ? '>' : '.'; break;
                    case SDLK_COMMA: inserted_char = shift_pressed ? '<' : ','; break;
                    case SDLK_SEMICOLON: inserted_char = shift_pressed ? ':' : ';'; break;
                    case SDLK_QUOTE: inserted_char = shift_pressed ? '"' : '\''; break;
                    case SDLK_LEFTBRACKET: inserted_char = shift_pressed ? '{' : '['; break;
                    case SDLK_RIGHTBRACKET: inserted_char = shift_pressed ? '}' : ']'; break;
                    case SDLK_BACKSLASH: inserted_char = shift_pressed ? '|' : '\\'; break;
                    case SDLK_SLASH: inserted_char = shift_pressed ? '?' : '/'; break;
                    case SDLK_EQUALS: inserted_char = shift_pressed ? '+' : '='; break;
                    case SDLK_MINUS: inserted_char = shift_pressed ? '_' : '-'; break;
                    case SDLK_BACKQUOTE: inserted_char = shift_pressed ? '~' : '`'; break;
                    default: inserted_char = 0;
                }
            }
        }
        if (Pressed(window->keyspressed[SDLK_LEFT])) move(LEFT);
        if (Pressed(window->keyspressed[SDLK_RIGHT])) move(RIGHT);
        if (Pressed(window->keyspressed[SDLK_UP])) move(UP);
        if (Pressed(window->keyspressed[SDLK_DOWN])) move(DOWN);
        if (window->keyspressed[SDLK_TAB]&&window->keyspressed[SDLK_TAB]<min_frame) {
            min_frame = window->keyspressed[SDLK_TAB];
            key_pressed = true;
            inserted_char = '\t';
        }
        if (window->keyspressed[SDLK_RETURN]&&window->keyspressed[SDLK_RETURN]<min_frame) {
            min_frame = window->keyspressed[SDLK_RETURN];
            key_pressed = true;
            inserted_char = '\n';
        }
        if (window->keyspressed[SDLK_BACKSPACE]&&window->keyspressed[SDLK_BACKSPACE]<min_frame) {
            min_frame = window->keyspressed[SDLK_BACKSPACE];
            key_pressed = true;
            inserted_char = '\b';
        }
        if (key_pressed && Pressed(min_frame)) {
            if (inserted_char == '\t') {
                insert(' ');
                insert(' ');
                insert(' ');
                insert(' ');
            } else if (inserted_char == '\b') {
                remove();
            } else if (inserted_char != 0) {
                insert(inserted_char);
            }
        }
    }
};
#endif
