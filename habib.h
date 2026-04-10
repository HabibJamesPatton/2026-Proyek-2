#ifndef HABIB_H
#define HABIB_H

typedef struct {
    char *data;
    int length;
    int capacity;
} Line;

typedef struct {
    Line *lines;
    int total_lines;
    int lines_capacity;
    int cursor_row;
    int cursor_col;
} Editor;

#define MAX_HISTORY 20

typedef struct {
    Editor data[MAX_HISTORY];
    int top;
} HistoryStack;

extern HistoryStack undo_stack;
extern HistoryStack redo_stack;

void editor_init(Editor *ed);
void editor_free(Editor *ed);

void editor_insert_char(Editor *ed, char ch);
void editor_backspace(Editor *ed);
void editor_enter(Editor *ed);

void editor_move_up(Editor *ed);
void editor_move_down(Editor *ed);
void editor_move_left(Editor *ed);
void editor_move_right(Editor *ed);

void init_stacks();
void push_undo(const Editor *current_state);
void perform_undo(Editor *current_state);
void perform_redo(Editor *current_state);
Editor* editor_create_snapshot(const Editor *ed);
void editor_load_snapshot(Editor *dest, const Editor *src);

void editor_append_line(Editor *ed, const char *text);
const char* editor_get_line_text(const Editor *ed, int row);

#endif