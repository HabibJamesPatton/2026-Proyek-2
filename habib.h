#ifndef HABIB_H
#define HABIB_H

// Perubahan untuk menggunakan struktur data linked list untuk menyimpan baris teks
typedef char* infotype;
typedef struct LineNode* address;
typedef struct LineNode{
    infotype data;
    int length;
    int capacity;
    address next;
    address prev;
} LineNode;

typedef struct Editor{
    address Head;
    address Tail;
    int total_lines;
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

address editor_get_node(const Editor *ed, int row);
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

void editor_append_line(Editor *ed, const infotype text);
const char* editor_get_line_text(const Editor *ed, int row);


#endif