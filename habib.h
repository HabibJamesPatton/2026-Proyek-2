#ifndef HABIB_H
#define HABIB_H


// Deklrasi konstanta yang akan dipakai array untuk menyimpan buffer
#define MAX_ROWS 100
#define MAX_COLS 256

// Deklarasi konstanta untuk hotkey
// Dipanggil dengan 'getch();' untuk membaca input tombol dari user
#define KEY_ENTER 13 // Angka yang diinisialisasi adalah ASCII
#define KEY_BACKSPACE 8 
#define KEY_ESCAPE 27


/*buffer[i][j] -> i mewakili baris dan j mewakili kolom*/
typedef struct {
    char buffer[MAX_ROWS][MAX_COLS];  /* Array 2D utama        */
    int  line_length[MAX_ROWS];       /* Panjang tiap baris    */
    int  total_lines;                 /* Total baris terpakai  */
    int  cursor_row;                  /* Posisi kursor (baris) */
    int  cursor_col;                  /* Posisi kursor (kolom) */
} Editor;

// Deklarasi Fungsi

/* Inisialisasi editor — reset semua buffer dan state */
void editor_init(Editor *ed);

/* Render/tampilkan isi editor ke layar(sementara) */
void editor_render(const Editor *ed);

/* INSERT: Sisipkan karakter di posisi kursor
 * → menggeser elemen array 2D ke kanan pada baris aktif */
void editor_insert_char(Editor *ed, char ch);

/* BACKSPACE: Hapus karakter sebelum kursor
 * → menggeser elemen array 2D ke kiri, atau gabung baris */
void editor_backspace(Editor *ed);

/* ENTER: Pecah baris di posisi kursor
 * → menggeser baris-baris di array 2D ke bawah */
void editor_enter(Editor *ed);

/* Navigasi kursor (arrow keys) */
void editor_move_up(Editor *ed);
void editor_move_down(Editor *ed);
void editor_move_left(Editor *ed);
void editor_move_right(Editor *ed);

#endif
