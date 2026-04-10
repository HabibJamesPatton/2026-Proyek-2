# *Proyek 2 - Pengembangan aplikasi berbasis library - Kelompok Bingung*

## -Setup Environment-

## -Installasi(Jika ada)-

## -Cara pakai-
  1. habib.h
    spesifikasi function yang terdaftar beserta implementasinya(Cara Pakainya):
    
    editor_init(Editor *ed) : Menyiapkan blok memori awal untuk menampung baris dan karakter.

    editor_free(Editor *ed) : Menghapus semua alokasi memori di RAM. Wajib dipanggil sebelum program ditutup agar tidak terjadi memory leak.

    editor_insert_char(Editor *ed, char ch) : Menyisipkan satu karakter di posisi kursor. Jika memori baris penuh, fungsi ini akan otomatis memperlebar kapasitas menggunakan realloc.

    editor_enter(Editor *ed) : Memecah baris saat ini menjadi dua baris. Teks di sebelah kanan kursor akan dipindahkan ke baris baru di bawahnya.

    editor_backspace(Editor *ed) : Menghapus karakter di kiri kursor. Jika kursor berada di awal baris, fungsi ini akan menggabungkan baris tersebut dengan baris di atasnya.

    editor_move_up(Editor *ed) / editor_move_down(Editor *ed) : Berpindah antar baris. Jika baris tujuan lebih pendek, kursor akan otomatis menyesuaikan ke ujung teks terdekat.

    editor_move_left(Editor *ed) / editor_move_right(Editor *ed) : Berpindah antar karakter. Jika mencapai ujung baris, kursor akan otomatis melompat ke baris sebelum/sesudahnya.

    editor_create_snapshot(const Editor *ed) : Fungsi tidak hanya menyalin alamat memori, tetapi mengalokasikan blok memori baru dan menyalin setiap karakter satu per satu menggunakan strcpy. Hal ini memastikan bahwa data di dalam snapshot tetap aman meskipun data di editor utama berubah.

    editor_load_snapshot(Editor *dest, const Editor *src) : Fungsi ini melakukan pembersihan memori (free) pada editor tujuan untuk mencegah kebocoran RAM. Setelah itu, ia mengalokasikan ulang memori dan menyalin kembali seluruh teks serta posisi kursor dari sumber snapshot ke editor utama.

    editor_append_line(Editor *ed, const char *text) : Digunakan oleh modul File (Faleh) untuk memasukkan teks hasil pembacaan file .txt ke dalam editor.

    editor_get_line_text(const Editor *ed, int row) : Digunakan untuk mengambil string murni dari baris tertentu, baik untuk keperluan rendering (Raka) maupun penyimpanan file (Faleh).

   2. raka.h
   UpdateKanvasArea(KanvasArea *textArea) : Membaca dan memproses input dari user (klik mouse, scroll, keyboard, dan shortcut) untuk memperbarui status kanvas. Menangani logika interaksi visual seperti kedipan kursor, batasan scroll vertikal, integrasi OS Clipboard (Copy/Cut/Paste), serta menyimpan state untuk sistem Undo. Wajib dipanggil setiap frame di dalam loop utama sebelum proses rendering.

   DrawKanvasArea(KanvasArea *textArea) : Merender (menggambar) seluruh elemen visual kanvas ke layar menggunakan fungsi bawaan Raylib. Bertanggung jawab menampilkan kotak background, perubahan warna border saat fokus, memotong teks agar tidak keluar batas kanvas (Scissor Mode) saat di-scroll, dan menggambar kursor indikator teks. Wajib diletakkan di dalam blok BeginDrawing() dan EndDrawing().

   3. faleh.h
   void New_File(Editor *ed) : Membuat file baru, memulai dengan buffer kosong, dan mengatur nama file global menjadi string kosong. Fungsi ini juga memastikan bahwa editor siap untuk menerima input teks baru tanpa sisa data dari file sebelumnya.

   void Open_File(Editor *ed, const char *filename) : Mengambil file yang terismpan di memori program agar bisa diedit, fungsi ini membuka file yang ditentukan dan memuat isinya kedalam editor.

   void SaveAs(const Editor *ed, const char *filename) : Membuat file baru atau menimpa file lama dengan nama spesifik, memindahkan data dari layar ke penyimpanan program.

   void Save(const Editor *ed) : Memindahkan data dari layar ke penyimpanan program,fungsi ini menyimpan perubahan yang telah dilakukan pada file yang sedang dibuka.

   void Close_File(Editor *ed) : Memutus koneksi antara program dan file. Fungsi ini membersihkan buffer editor dan mengatur nama file global menjadi string kosong, sehingga editor kembali ke kondisi awal tanpa file yang terbuka.


## -Identitas kelompok-
  1. Nama       : Habib Burrohman
     NIM        : 251511039
     Id GitHub  : HabibJamesPatton
  2. Nama       : Muhammad faleh alfawwaz
     NIM        : 251511048
     Id Github  : ABLEH25
  3. Nama       : Muhammad Raka Pratama
     NIM        : 251511052
     Id GitHub  : Ub1C1lembu
