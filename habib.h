#ifndef HABIB_H
#define HABIB_H

// --- Struktur Baris Dinamis ---
typedef struct {
    char *data;         // Pointer ke array karakter (isi teks per baris)
    int length;         // Jumlah karakter yang terisi saat ini
    int capacity;       // Total memori yang disiapkan untuk baris ini
} Line;

// --- Struktur Utama Editor ---
typedef struct {
    Line *lines;        // Array dinamis dari kumpulan baris (buffer utama)
    int total_lines;    // Jumlah baris yang ada di editor
    int lines_capacity; // Kapasitas alokasi baris (untuk penambahan baris)
    int cursor_row;     // Posisi baris kursor (indeks array)
    int cursor_col;     // Posisi kolom kursor (indeks karakter)
} Editor;

// --- Inisialisasi & Memori ---
void editor_init(Editor *ed);   // Menyiapkan memori awal saat aplikasi dibuka
void editor_free(Editor *ed);   // Menghapus semua alokasi memori (cegah memory leak)

// --- Logika Pengetikan (Tugas Utama Kamu) ---
void editor_insert_char(Editor *ed, char ch); // Menambah satu huruf di posisi kursor
void editor_backspace(Editor *ed);            // Menghapus huruf atau menggabung baris
void editor_enter(Editor *ed);                // Memecah satu baris menjadi dua baris baru

// ---  Navigasi Untuk (Raka)  ---
void editor_move_up(Editor *ed);    // Geser kursor ke atas dengan validasi batas
void editor_move_down(Editor *ed);  // Geser kursor ke bawah dengan validasi batas
void editor_move_left(Editor *ed);  // Geser kursor ke kiri (bisa pindah baris ke atas)
void editor_move_right(Editor *ed); // Geser kursor ke kanan (bisa pindah baris ke bawah)

// --- Fitur Undo/Redo (Pertemuan 6-7) ---
Editor* editor_create_snapshot(const Editor *ed);        // Salin state saat ini ke Stack
void editor_load_snapshot(Editor *dest, const Editor *src); // Ambil state dari Stack ke Editor

// ---  File Operations (Faleh) ---
void editor_append_line(Editor *ed, const char *text);      // Masukkan teks hasil Load File
const char* editor_get_line_text(const Editor *ed, int row); // Ambil teks untuk proses Save File

#endif