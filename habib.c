#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "habib.h"

#define INITIAL_LINES_CAPACITY 10
#define INITIAL_LINE_LENGTH 64

HistoryStack undo_stack;
HistoryStack redo_stack;

// Post ETS - Operasi Node DLL
static address create_node(const infotype text){
    address node = (address)malloc(sizeof(LineNode));
    if(!node) return NULL;

    int len = strlen(text);
    node->capacity = (len + INITIAL_LINE_LENGTH);
    node->data = (infotype)malloc(node->capacity);
    if(!node->data) { 
        free(node); 
        return NULL;
    }

    strcpy(node->data, text);
    node->length = len;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

static void free_node(address node){
    if(!node) return;
    free(node->data);
    free(node);
}

static void insert_after(Editor *ed, address target, address new_node){
    if (!target || !new_node) return;
    address successor = target->next;
    new_node->prev = target;
    new_node->next = successor;
    target->next = new_node;

    if(successor != NULL){
        successor->prev = new_node;
    } else {
        ed->Tail = new_node;
    }

    ed->total_lines++;
}

static void unlink_node (Editor *ed, address node){
    if(node->prev){
        node->prev->next = node->next;
    }else{
        ed->Head = node->next;
    }

    if(node->next){
        node->next->prev = node->prev;
    }else{
        ed->Tail = node->prev;
    }

    ed->total_lines--;
}

static void editor_free_data(Editor *ed){
    address current = ed->Head;
    while(current != NULL){
        address temp = current->next;
        free_node(current);
        current = temp;
    }
    ed->Head = NULL;
    ed->Tail = NULL;
    ed->total_lines = 0;
}

static void editor_clone_dll(Editor *dest, const Editor *src){
    dest->Head = NULL;
    dest->Tail = NULL;
    dest->total_lines = 0;
    dest->cursor_row = src->cursor_row;
    dest->cursor_col = src->cursor_col;

    address current = src->Head;
    while(current != NULL){
        address clone = create_node(current->data);
        if(!clone) {
            editor_free_data(dest);
            return;
        }
        clone->length = current->length;

        if(dest->Head == NULL){
            dest->Head = clone;
            dest->Tail = clone;
        }else{
            clone->prev = dest->Tail;
            dest->Tail->next = clone;
            dest->Tail = clone;
        }
        dest->total_lines++;
        current = current->next;
    }
}

static void push_to_stack(HistoryStack *stack, const Editor *ed) {
    if (stack->top >= MAX_HISTORY - 1) {                      
        editor_free_data(&stack->data[0]);                    
        for (int i = 0; i < MAX_HISTORY - 1; i++) {          
            stack->data[i] = stack->data[i + 1];             
        }
        stack->top = MAX_HISTORY - 2;
        stack->data[MAX_HISTORY - 1].Head = NULL;
        stack->data[MAX_HISTORY - 1].Tail = NULL;
        stack->data[MAX_HISTORY - 1].total_lines = 0;                         
    }
    stack->top++;                                             
    editor_clone_dll(&stack->data[stack->top], ed);           
}

void init_stacks() {
    undo_stack.top = -1;   
    redo_stack.top = -1;   
}

void clear_stack(HistoryStack *stack) {
    while (stack->top >= 0) {                           
    editor_free_data(&stack->data[stack->top]);     
        stack->top--;                                   
    }
}

void push_undo(const Editor *ed) {
    push_to_stack(&undo_stack, ed);
    clear_stack(&redo_stack);
}

void perform_undo(Editor *ed) {
    if (undo_stack.top < 0) return;

    push_to_stack(&redo_stack, ed);                             

    editor_free_data(ed);                                       
    editor_clone_dll(ed, &undo_stack.data[undo_stack.top]);    

    editor_free_data(&undo_stack.data[undo_stack.top]);       
    undo_stack.top--;                                         
}


void perform_redo(Editor *ed) {
    if (redo_stack.top < 0) return;
    push_to_stack(&undo_stack, ed);
    editor_free_data(ed);
    editor_clone_dll(ed, &redo_stack.data[redo_stack.top]);
    editor_free_data(&redo_stack.data[redo_stack.top]);
    redo_stack.top--;
}

address editor_get_node(const Editor *ed, int row){
    if (row < 0 || row >= ed->total_lines) return NULL;
    address cursor = ed->Head;
    for (int i = 0; i < row; i++){
        cursor = cursor->next;
    }
    return cursor;
}

void editor_init(Editor *ed) {
    address node = create_node("");
    if(!node) return;
    ed->Head = node;
    ed->Tail = node;
    ed->total_lines = 1;
    ed->cursor_row = 0;
    ed->cursor_col = 0;
}

void editor_free(Editor *ed) {
    address current = ed->Head; 
    address temp; 
    while(current != NULL){
        temp = current;
        current = current->next;
        free_node(temp);
    }
    ed->Head = NULL;
    ed->Tail = NULL;
    ed->total_lines = 0;
    ed->cursor_row = 0;
    ed->cursor_col = 0; 
}

void editor_insert_char(Editor *ed, char ch) {
    address lines = editor_get_node(ed, ed->cursor_row);
    if(lines == NULL) return;
    if(lines->length + 1 >= lines->capacity){
        lines->capacity *= 2;
        char *new_data = (char *)realloc(lines->data, lines->capacity * sizeof(char));
        if(!new_data) return;
        lines->data = new_data;
    }
    memmove(&lines->data[ed->cursor_col + 1], &lines->data[ed->cursor_col], lines->length - ed->cursor_col + 1);
    lines->data[ed->cursor_col] = ch;
    lines->length++;
    ed->cursor_col++;
}

void editor_backspace(Editor *ed) {
    address current_line = editor_get_node(ed, ed->cursor_row);
    if (!current_line) return;

    if (ed->cursor_col > 0) {
        memmove(
            &current_line->data[ed->cursor_col - 1],
            &current_line->data[ed->cursor_col],
            current_line->length - ed->cursor_col + 1
        );
        current_line->length--;
        ed->cursor_col--;

    } else if (ed->cursor_row > 0) {
        address prev_line = current_line->prev;
        int new_col = prev_line->length;

        int total_len = prev_line->length + current_line->length;
        if (total_len + 1 >= prev_line->capacity) {
            prev_line->capacity = total_len + INITIAL_LINE_LENGTH;
            char *buf = (char *)realloc(prev_line->data, prev_line->capacity);
            if (!buf) return;
            prev_line->data = buf;
        }

        strcpy(&prev_line->data[prev_line->length], current_line->data);
        prev_line->length = total_len;

        unlink_node(ed, current_line);
        free_node(current_line);

        ed->cursor_row--;
        ed->cursor_col = new_col;
    }
}

void editor_enter(Editor *ed) {
    address current_line = editor_get_node(ed, ed->cursor_row);
    if (!current_line) return;

    int chars_to_move = current_line->length - ed->cursor_col;
    address new_line = create_node("");
    if (!new_line) return;

    if (chars_to_move > 0) {
        if (chars_to_move + 1 >= new_line->capacity) {
           new_line->capacity = chars_to_move + INITIAL_LINE_LENGTH;
           char *tmp = (char *)realloc(new_line->data, new_line->capacity);
           if (!tmp) { free_node(new_line); return; }
           new_line->data = tmp;
        }
        strncpy(new_line->data, &current_line->data[ed->cursor_col], chars_to_move);
        new_line->data[chars_to_move] = '\0';
        new_line->length = chars_to_move;

        current_line->data[ed->cursor_col] = '\0';
        current_line->length = ed->cursor_col;
    }

    insert_after(ed, current_line, new_line);
    ed->cursor_row++;
    ed->cursor_col = 0;
}

void editor_move_left(Editor *ed) {
    if (ed->cursor_col > 0) {
        ed->cursor_col--;
    } else if (ed->cursor_row > 0) {
        ed->cursor_row--;
        address current_line = editor_get_node(ed, ed->cursor_row);
        ed->cursor_col = current_line->length;
    }
}

void editor_move_right(Editor *ed) {
    address current_line = editor_get_node(ed, ed->cursor_row);
    if (!current_line) return;

    if (ed->cursor_col < current_line->length) {
        ed->cursor_col++;
    } else if (ed->cursor_row < ed->total_lines - 1) {
        ed->cursor_row++;
        ed->cursor_col = 0;
    }
}

void editor_move_up(Editor *ed) {
    if (ed->cursor_row > 0) {
        ed->cursor_row--;
        address current_line = editor_get_node(ed, ed->cursor_row);
        if (current_line && ed->cursor_col > current_line->length) {
            ed->cursor_col = current_line->length;
        }
    }
}

void editor_move_down(Editor *ed) {
    if (ed->cursor_row < ed->total_lines - 1) {
        ed->cursor_row++;
        address current_line = editor_get_node(ed, ed->cursor_row);
        if (current_line && ed->cursor_col > current_line->length) {
            ed->cursor_col = current_line->length;
        }
    }
}


void editor_append_line(Editor *ed, const infotype text) {
    address newNode = create_node(text);               

    if(!newNode) return;
    if (ed->Head == NULL) {                                    
        ed->Head = newNode;                                  
        ed->Tail = newNode;                                  
    } else {
        newNode->prev = ed->Tail;                            
        ed->Tail->next = newNode;                            
        ed->Tail = newNode;                                  
    }

    ed->total_lines++;                                         
}


const char* editor_get_line_text(const Editor *ed, int row) {
    address node = editor_get_node(ed, row);  
    if (node != NULL) {                        
        return node->data;                       
    }
    return NULL;                                 
}
