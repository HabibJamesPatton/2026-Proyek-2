#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "habib.h"

// Mendefinisikan ukuran awal agar alokasi memori tidak terlalu sering dilakukan
#define INITIAL_LINES_CAPACITY 10
#define INITIAL_LINE_LENGTH 64

// INISIALISASI & MANAJEMEN MEMORI

void editor_init(Editor *ed) {
    // Mengalokasikan memori untuk array struct Line sebesar kapasitas awal
    ed->lines = (Line *)malloc(INITIAL_LINES_CAPACITY * sizeof(Line));
    // Error handling: Jika RAM penuh dan malloc gagal, hentikan eksekusi
    if (!ed->lines) {
        fprintf(stderr, "Fatal Error: Gagal alokasi memori awal editor!\n");
        exit(1); 
    }

    ed->total_lines = 1; // Memulai dengan 1 baris kosong
    ed->lines_capacity = INITIAL_LINES_CAPACITY;
    ed->cursor_row = 0;  // Kursor berada di baris pertama
    ed->cursor_col = 0;  // Kursor berada di kolom pertama

    // Menyiapkan baris pertama
    ed->lines[0].capacity = INITIAL_LINE_LENGTH;
    ed->lines[0].length = 0;
    // Mengalokasikan memori untuk menyimpan karakter pada baris pertama
    ed->lines[0].data = (char *)malloc(INITIAL_LINE_LENGTH * sizeof(char));
    
    if (!ed->lines[0].data) {
        fprintf(stderr, "Fatal Error: Gagal alokasi baris pertama!\n");
        exit(1);
    }
    // Memberikan null terminator agar string terbaca kosong
    ed->lines[0].data[0] = '\0'; 
}

void editor_free(Editor *ed) {
    // Looping untuk membebaskan memori string (data) pada setiap baris
    for (int i = 0; i < ed->total_lines; i++) {
        free(ed->lines[i].data);
    }
    // Membebaskan array baris utama
    free(ed->lines);
    // Mereset pointer ke NULL untuk mencegah Dangling Pointer (mengakses memori yang sudah dihapus)
    ed->lines = NULL; 
}

// LOGIKA TEKS (TYPING)

void editor_insert_char(Editor *ed, char ch) {
    // Membuat pointer lokal (shortcut) ke baris tempat kursor berada
    Line *current_line = &ed->lines[ed->cursor_row];

    // Cek apakah kapasitas memori baris sudah penuh (+1 untuk null terminator)
    if (current_line->length + 1 >= current_line->capacity) {
        // Melipatgandakan kapasitas memori baris saat ini
        current_line->capacity *= 2;
        // Realloc: memperluas blok memori tanpa menghilangkan data lama
        char *new_data = (char *)realloc(current_line->data, current_line->capacity * sizeof(char));
        
        // Error handling realloc
        if (!new_data) return; 
        current_line->data = new_data;
    }

    // Memmove digunakan untuk menggeser sisa teks ke kanan untuk memberi ruang pada karakter baru
    // Memmove aman untuk blok memori yang saling tumpang tindih (overlapping)
    memmove(&current_line->data[ed->cursor_col + 1], 
            &current_line->data[ed->cursor_col], 
            current_line->length - ed->cursor_col + 1); // +1 untuk memindahkan null terminator juga

    // Menyisipkan karakter baru di posisi kursor
    current_line->data[ed->cursor_col] = ch;
    current_line->length++;
    ed->cursor_col++;
}

void editor_backspace(Editor *ed) {
    Line *current_line = &ed->lines[ed->cursor_row];

    // Kasus 1: Kursor tidak di awal baris (hapus karakter biasa)
    if (ed->cursor_col > 0) {
        // Menggeser sisa teks di kanan kursor ke arah kiri, menimpa karakter yang dihapus
        memmove(&current_line->data[ed->cursor_col - 1], 
                &current_line->data[ed->cursor_col], 
                current_line->length - ed->cursor_col + 1);
        
        current_line->length--;
        ed->cursor_col--;
    } 
    // Kasus 2: Kursor di awal baris, dan bukan di baris paling atas (gabung baris)
    else if (ed->cursor_row > 0) {
        Line *prev_line = &ed->lines[ed->cursor_row - 1];
        int new_col = prev_line->length; // Menyimpan posisi kursor setelah digabung

        // Memastikan baris atas memiliki memori yang cukup untuk menampung baris saat ini
        if (prev_line->length + current_line->length + 1 > prev_line->capacity) {
            prev_line->capacity = prev_line->length + current_line->length + INITIAL_LINE_LENGTH;
            char *new_data = (char *)realloc(prev_line->data, prev_line->capacity * sizeof(char));
            if (!new_data) return;
            prev_line->data = new_data;
        }

        // Menyalin isi baris saat ini ke ujung baris atas
        strcpy(&prev_line->data[prev_line->length], current_line->data);
        prev_line->length += current_line->length;

        // Membebaskan memori baris yang sudah kosong karena digabung
        free(current_line->data);

        // Menggeser array struct Line ke atas untuk menutup celah baris yang dihapus
        memmove(&ed->lines[ed->cursor_row], 
                &ed->lines[ed->cursor_row + 1], 
                (ed->total_lines - ed->cursor_row - 1) * sizeof(Line));

        ed->total_lines--;
        ed->cursor_row--;
        ed->cursor_col = new_col; // Kursor melompat ke titik penyambungan
    }
}

void editor_enter(Editor *ed) {
    // Cek apakah array struct Line perlu diperbesar
    if (ed->total_lines >= ed->lines_capacity) {
        ed->lines_capacity *= 2;
        Line *new_lines = (Line *)realloc(ed->lines, ed->lines_capacity * sizeof(Line));
        if (!new_lines) return;
        ed->lines = new_lines;
    }

    Line *current_line = &ed->lines[ed->cursor_row];

    // Menggeser struct Line di bawah kursor ke bawah 1 tingkat untuk membuat baris kosong
    memmove(&ed->lines[ed->cursor_row + 2], 
            &ed->lines[ed->cursor_row + 1], 
            (ed->total_lines - ed->cursor_row - 1) * sizeof(Line));

    // Menginisialisasi baris baru hasil Enter
    Line *new_line = &ed->lines[ed->cursor_row + 1];
    int chars_to_move = current_line->length - ed->cursor_col;
    
    // Alokasi memori untuk teks yang tergeser ke baris bawah
    new_line->capacity = chars_to_move + INITIAL_LINE_LENGTH;
    new_line->data = (char *)malloc(new_line->capacity * sizeof(char));
    if (!new_line->data) return;

    // Memindahkan teks dari kanan kursor ke baris baru
    strncpy(new_line->data, &current_line->data[ed->cursor_col], chars_to_move);
    new_line->data[chars_to_move] = '\0';
    new_line->length = chars_to_move;

    // Memotong teks pada baris saat ini
    current_line->data[ed->cursor_col] = '\0';
    current_line->length = ed->cursor_col;

    ed->total_lines++;
    ed->cursor_row++;
    ed->cursor_col = 0; // Kursor otomatis pindah ke ujung kiri baris baru
}

// LOGIKA NAVIGASI KURSOR (BOUNDARY CHECK)

void editor_move_left(Editor *ed) {
    if (ed->cursor_col > 0) {
        ed->cursor_col--;
    } else if (ed->cursor_row > 0) {
        // Jika mentok kiri, kursor melompat ke ujung kanan baris atasnya
        ed->cursor_row--;
        ed->cursor_col = ed->lines[ed->cursor_row].length;
    }
}

void editor_move_right(Editor *ed) {
    if (ed->cursor_col < ed->lines[ed->cursor_row].length) {
        ed->cursor_col++;
    } else if (ed->cursor_row < ed->total_lines - 1) {
        // Jika mentok kanan, kursor melompat ke ujung kiri baris bawahnya
        ed->cursor_row++;
        ed->cursor_col = 0;
    }
}

void editor_move_up(Editor *ed) {
    if (ed->cursor_row > 0) {
        ed->cursor_row--;
        // Menyesuaikan kolom jika baris atas lebih pendek dari posisi kursor saat ini (Snap to End)
        if (ed->cursor_col > ed->lines[ed->cursor_row].length) {
            ed->cursor_col = ed->lines[ed->cursor_row].length;
        }
    }
}

void editor_move_down(Editor *ed) {
    if (ed->cursor_row < ed->total_lines - 1) {
        ed->cursor_row++;
        // Menyesuaikan kolom jika baris bawah lebih pendek (Snap to End)
        if (ed->cursor_col > ed->lines[ed->cursor_row].length) {
            ed->cursor_col = ed->lines[ed->cursor_row].length;
        }
    }
}

// INFRASTRUKTUR UNDO/REDO (SNAPSHOT)

Editor* editor_create_snapshot(const Editor *ed) {
    // Alokasi editor baru untuk dijadikan cadangan (backup)
    Editor *snap = (Editor *)malloc(sizeof(Editor));
    if (!snap) return NULL;

    // Menyalin metadata dasar
    snap->total_lines = ed->total_lines;
    snap->lines_capacity = ed->lines_capacity;
    snap->cursor_row = ed->cursor_row;
    snap->cursor_col = ed->cursor_col;

    // Alokasi array baris baru
    snap->lines = (Line *)malloc(snap->lines_capacity * sizeof(Line));
    if (!snap->lines) { free(snap); return NULL; }

    // Deep copy: menyalin isi teks per baris agar memori tidak bentrok
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

    // Hapus memori editor yang sekarang agar bisa diganti dengan state dari snapshot
    for (int i = 0; i < dest->total_lines; i++) {
        free(dest->lines[i].data);
    }
    free(dest->lines);

    // Salin ulang metadata dari snapshot
    dest->total_lines = src->total_lines;
    dest->lines_capacity = src->lines_capacity;
    dest->cursor_row = src->cursor_row;
    dest->cursor_col = src->cursor_col;

    // Alokasi dan salin isi teks dari snapshot
    dest->lines = (Line *)malloc(dest->lines_capacity * sizeof(Line));
    for (int i = 0; i < dest->total_lines; i++) {
        dest->lines[i].length = src->lines[i].length;
        dest->lines[i].capacity = src->lines[i].capacity;
        dest->lines[i].data = (char *)malloc(dest->lines[i].capacity * sizeof(char));
        strcpy(dest->lines[i].data, src->lines[i].data);
    }
}

// API FILE OPERATIONS (Faleh)

void editor_append_line(Editor *ed, const char *text) {
    // Cek batas array utama
    if (ed->total_lines >= ed->lines_capacity) {
        ed->lines_capacity *= 2;
        ed->lines = (Line *)realloc(ed->lines, ed->lines_capacity * sizeof(Line));
    }

    int len = strlen(text);
    Line *new_line = &ed->lines[ed->total_lines];
    
    // Alokasi sesuai panjang teks yang diberikan Ableh
    new_line->capacity = len + INITIAL_LINE_LENGTH;
    new_line->data = (char *)malloc(new_line->capacity * sizeof(char));
    if (!new_line->data) return;

    strcpy(new_line->data, text);
    new_line->length = len;
    
    ed->total_lines++;
}

const char* editor_get_line_text(const Editor *ed, int row) {
    // Mengembalikan pointer string murni agar Ableh bisa menyimpannya ke file txt
    if (row >= 0 && row < ed->total_lines) {
        return ed->lines[row].data;
    }
    return NULL; // Pengaman jika Ableh meminta baris yang tidak ada
}