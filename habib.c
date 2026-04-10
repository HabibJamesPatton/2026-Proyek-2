#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "habib.h"

#define INITIAL_LINES_CAPACITY 10
#define INITIAL_LINE_LENGTH 64

HistoryStack undo_stack;
HistoryStack redo_stack;

void init_stacks() {
    undo_stack.top = -1;
    redo_stack.top = -1;
}

void clear_redo_stack() {
    while(redo_stack.top >= 0) {
        for(int i = 0; i < redo_stack.data[redo_stack.top].total_lines; i++) {
            free(redo_stack.data[redo_stack.top].lines[i].data);
        }
        free(redo_stack.data[redo_stack.top].lines);
        redo_stack.top--;
    }
}

void free_stack_slot(HistoryStack *stack, int index) {
    for(int i = 0; i < stack->data[index].total_lines; i++) {
        free(stack->data[index].lines[i].data);
    }
    free(stack->data[index].lines);
}

void push_to_stack(HistoryStack *stack, const Editor *ed) {
    if (stack->top >= MAX_HISTORY - 1) {
        free_stack_slot(stack, 0);
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            stack->data[i] = stack->data[i+1];
        }
        stack->top = MAX_HISTORY - 2;
    }
    
    stack->top++;
    stack->data[stack->top].total_lines = ed->total_lines;
    stack->data[stack->top].lines_capacity = ed->lines_capacity;
    stack->data[stack->top].cursor_row = ed->cursor_row;
    stack->data[stack->top].cursor_col = ed->cursor_col;
    stack->data[stack->top].lines = (Line *)malloc(ed->lines_capacity * sizeof(Line));
    
    for (int i = 0; i < ed->total_lines; i++) {
        stack->data[stack->top].lines[i].length = ed->lines[i].length;
        stack->data[stack->top].lines[i].capacity = ed->lines[i].capacity;
        stack->data[stack->top].lines[i].data = (char *)malloc(ed->lines[i].capacity * sizeof(char));
        strcpy(stack->data[stack->top].lines[i].data, ed->lines[i].data);
    }
}

void push_undo(const Editor *ed) {
    push_to_stack(&undo_stack, ed);
    clear_redo_stack();
}

void perform_undo(Editor *ed) {
    if (undo_stack.top >= 0) {
        push_to_stack(&redo_stack, ed);
        editor_load_snapshot(ed, &undo_stack.data[undo_stack.top]);
        free_stack_slot(&undo_stack, undo_stack.top);
        undo_stack.top--;
    }
}

void perform_redo(Editor *ed) {
    if (redo_stack.top >= 0) {
        push_to_stack(&undo_stack, ed);
        editor_load_snapshot(ed, &redo_stack.data[redo_stack.top]);
        free_stack_slot(&redo_stack, redo_stack.top);
        redo_stack.top--;
    }
}

void editor_init(Editor *ed) {
    ed->lines = (Line *)malloc(INITIAL_LINES_CAPACITY * sizeof(Line));
    if (!ed->lines) exit(1);
    ed->total_lines = 1;
    ed->lines_capacity = INITIAL_LINES_CAPACITY;
    ed->cursor_row = 0;
    ed->cursor_col = 0;
    ed->lines[0].capacity = INITIAL_LINE_LENGTH;
    ed->lines[0].length = 0;
    ed->lines[0].data = (char *)malloc(INITIAL_LINE_LENGTH * sizeof(char));
    if (!ed->lines[0].data) exit(1);
    ed->lines[0].data[0] = '\0';
}

void editor_free(Editor *ed) {
    for (int i = 0; i < ed->total_lines; i++) {
        free(ed->lines[i].data);
    }
    free(ed->lines);
    ed->lines = NULL;
}

void editor_insert_char(Editor *ed, char ch) {
    push_undo(ed);
    Line *current_line = &ed->lines[ed->cursor_row];
    if (current_line->length + 1 >= current_line->capacity) {
        current_line->capacity *= 2;
        char *new_data = (char *)realloc(current_line->data, current_line->capacity * sizeof(char));
        if (!new_data) return;
        current_line->data = new_data;
    }
    memmove(&current_line->data[ed->cursor_col + 1], 
            &current_line->data[ed->cursor_col], 
            current_line->length - ed->cursor_col + 1);
    current_line->data[ed->cursor_col] = ch;
    current_line->length++;
    ed->cursor_col++;
}

void editor_backspace(Editor *ed) {
    push_undo(ed);
    Line *current_line = &ed->lines[ed->cursor_row];
    if (ed->cursor_col > 0) {
        memmove(&current_line->data[ed->cursor_col - 1], 
                &current_line->data[ed->cursor_col], 
                current_line->length - ed->cursor_col + 1);
        current_line->length--;
        ed->cursor_col--;
    } else if (ed->cursor_row > 0) {
        Line *prev_line = &ed->lines[ed->cursor_row - 1];
        int new_col = prev_line->length;
        if (prev_line->length + current_line->length + 1 > prev_line->capacity) {
            prev_line->capacity = prev_line->length + current_line->length + INITIAL_LINE_LENGTH;
            char *new_data = (char *)realloc(prev_line->data, prev_line->capacity * sizeof(char));
            if (!new_data) return;
            prev_line->data = new_data;
        }
        strcpy(&prev_line->data[prev_line->length], current_line->data);
        prev_line->length += current_line->length;
        free(current_line->data);
        memmove(&ed->lines[ed->cursor_row], 
                &ed->lines[ed->cursor_row + 1], 
                (ed->total_lines - ed->cursor_row - 1) * sizeof(Line));
        ed->total_lines--;
        ed->cursor_row--;
        ed->cursor_col = new_col;
    }
}

void editor_enter(Editor *ed) {
    push_undo(ed);
    if (ed->total_lines >= ed->lines_capacity) {
        ed->lines_capacity *= 2;
        Line *new_lines = (Line *)realloc(ed->lines, ed->lines_capacity * sizeof(Line));
        if (!new_lines) return;
        ed->lines = new_lines;
    }
    Line *current_line = &ed->lines[ed->cursor_row];
    memmove(&ed->lines[ed->cursor_row + 2], 
            &ed->lines[ed->cursor_row + 1], 
            (ed->total_lines - ed->cursor_row - 1) * sizeof(Line));
    Line *new_line = &ed->lines[ed->cursor_row + 1];
    int chars_to_move = current_line->length - ed->cursor_col;
    new_line->capacity = chars_to_move + INITIAL_LINE_LENGTH;
    new_line->data = (char *)malloc(new_line->capacity * sizeof(char));
    if (!new_line->data) return;
    strncpy(new_line->data, &current_line->data[ed->cursor_col], chars_to_move);
    new_line->data[chars_to_move] = '\0';
    new_line->length = chars_to_move;
    current_line->data[ed->cursor_col] = '\0';
    current_line->length = ed->cursor_col;
    ed->total_lines++;
    ed->cursor_row++;
    ed->cursor_col = 0;
}

void editor_move_left(Editor *ed) {
    if (ed->cursor_col > 0) ed->cursor_col--;
    else if (ed->cursor_row > 0) {
        ed->cursor_row--;
        ed->cursor_col = ed->lines[ed->cursor_row].length;
    }
}

void editor_move_right(Editor *ed) {
    if (ed->cursor_col < ed->lines[ed->cursor_row].length) ed->cursor_col++;
    else if (ed->cursor_row < ed->total_lines - 1) {
        ed->cursor_row++;
        ed->cursor_col = 0;
    }
}

void editor_move_up(Editor *ed) {
    if (ed->cursor_row > 0) {
        ed->cursor_row--;
        if (ed->cursor_col > ed->lines[ed->cursor_row].length) {
            ed->cursor_col = ed->lines[ed->cursor_row].length;
        }
    }
}

void editor_move_down(Editor *ed) {
    if (ed->cursor_row < ed->total_lines - 1) {
        ed->cursor_row++;
        if (ed->cursor_col > ed->lines[ed->cursor_row].length) {
            ed->cursor_col = ed->lines[ed->cursor_row].length;
        }
    }
}

Editor* editor_create_snapshot(const Editor *ed) {
    Editor *snap = (Editor *)malloc(sizeof(Editor));
    if (!snap) return NULL;
    snap->total_lines = ed->total_lines;
    snap->lines_capacity = ed->lines_capacity;
    snap->cursor_row = ed->cursor_row;
    snap->cursor_col = ed->cursor_col;
    snap->lines = (Line *)malloc(snap->lines_capacity * sizeof(Line));
    if (!snap->lines) { free(snap); return NULL; }
    for (int i = 0; i < snap->total_lines; i++) {
        snap->lines[i].length = ed->lines[i].length;
        snap->lines[i].capacity = ed->lines[i].capacity;
        snap->lines[i].data = (char *)malloc(snap->lines[i].capacity * sizeof(char));
        strcpy(snap->lines[i].data, ed->lines[i].data);
    }
    return snap;
}

void editor_load_snapshot(Editor *dest, const Editor *src) {
    if (!dest || !src) return;
    for (int i = 0; i < dest->total_lines; i++) {
        free(dest->lines[i].data);
    }
    free(dest->lines);
    dest->total_lines = src->total_lines;
    dest->lines_capacity = src->lines_capacity;
    dest->cursor_row = src->cursor_row;
    dest->cursor_col = src->cursor_col;
    dest->lines = (Line *)malloc(dest->lines_capacity * sizeof(Line));
    for (int i = 0; i < dest->total_lines; i++) {
        dest->lines[i].length = src->lines[i].length;
        dest->lines[i].capacity = src->lines[i].capacity;
        dest->lines[i].data = (char *)malloc(dest->lines[i].capacity * sizeof(char));
        strcpy(dest->lines[i].data, src->lines[i].data);
    }
}

void editor_append_line(Editor *ed, const char *text) {
    if (ed->total_lines >= ed->lines_capacity) {
        ed->lines_capacity *= 2;
        ed->lines = (Line *)realloc(ed->lines, ed->lines_capacity * sizeof(Line));
    }
    int len = strlen(text);
    Line *new_line = &ed->lines[ed->total_lines];
    new_line->capacity = len + INITIAL_LINE_LENGTH;
    new_line->data = (char *)malloc(new_line->capacity * sizeof(char));
    if (!new_line->data) return;
    strcpy(new_line->data, text);
    new_line->length = len;
    ed->total_lines++;
}

const char* editor_get_line_text(const Editor *ed, int row) {
    if (row >= 0 && row < ed->total_lines) {
        return ed->lines[row].data;
    }
    return NULL;
}