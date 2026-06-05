# *Proyek 2 - Pengembangan aplikasi berbasis library - Kelompok Bingung*

## -Setup Environment-
Aplikasi ini dibangun menggunakan bahasa C dan bergantung pada lingkungan serta library berikut:
- **Compiler:** GCC (MinGW-w64).
- **Library Eksternal Utama:** **Raylib** (versi 5.5) untuk menangani rendering grafis, antarmuka pengguna (GUI), dan pembacaan input (keyboard/mouse).
- **Library Sistem (OS):** **Windows API (`<windows.h>`)** digunakan secara khusus di modul sistem file untuk berinteraksi dengan OS (seperti mengambil letak direktori dengan fungsi *GetCurrentDirectory*).
- **Sistem Operasi:** Windows (konfigurasi *linker* membutuhkan library spesifik bawaan Windows seperti `-lopengl32 -lgdi32 -lwinmm`).

## -Installasi (Jika ada)-
Berikut adalah langkah-langkah untuk menginstal dan menyiapkan library agar program dapat dikompilasi:
1. **Install Compiler C (MinGW)**: Unduh dan instal GCC compiler. Pastikan direktori `bin` dari compiler telah terdaftar di *Environment Variables* (Path) Windows Anda.
2. **Install Library Raylib**: Unduh *pre-compiled library* Raylib versi 5.5 untuk Windows. Ekstrak filenya, lalu letakkan file *header* ke dalam folder `raylib/include` dan file binari *library* (`libraylib.a`) ke dalam folder `raylib/lib` tepat di dalam direktori proyek ini.

Setelah proses instalasi dependensi di atas selesai, Anda dapat melakukan kompilasi program melalui terminal (PowerShell/CMD) dengan perintah:
```bash
gcc -o program.exe main.c habib.c faleh.c raka.c -Iraylib/include -Lraylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
```
Lalu jalankan aplikasi dengan perintah:
```bash
./program.exe
```

## -Cara pakai-
Secara keseluruhan, aplikasi adalah sebuah Text Editor interaktif dengan antarmuka grafis. Fitur-fitur utama di-handle oleh beberapa modul yang terpisah:

### 1. habib.h (Struktur Data & Mesin Editor)
Spesifikasi fungsi yang terdaftar beserta implementasinya:
- `editor_init(Editor *ed)` : Menyiapkan blok memori awal untuk menampung baris dan karakter.
- `editor_free(Editor *ed)` : Menghapus semua alokasi memori di RAM. Wajib dipanggil sebelum program ditutup agar tidak terjadi memory leak.
- `editor_insert_char(Editor *ed, char ch)` : Menyisipkan satu karakter di posisi kursor. Jika memori baris penuh, fungsi ini akan otomatis memperlebar kapasitas menggunakan `realloc`.
- `editor_enter(Editor *ed)` : Memecah baris saat ini menjadi dua baris. Teks di sebelah kanan kursor akan dipindahkan ke baris baru di bawahnya.
- `editor_backspace(Editor *ed)` : Menghapus karakter di kiri kursor. Jika kursor berada di awal baris, fungsi ini akan menggabungkan baris tersebut dengan baris di atasnya.
- `editor_move_up(...)` / `editor_move_down(...)` : Berpindah antar baris. Jika baris tujuan lebih pendek, kursor akan otomatis menyesuaikan ke ujung teks terdekat.
- `editor_move_left(...)` / `editor_move_right(...)` : Berpindah antar karakter. Jika mencapai ujung baris, kursor akan otomatis melompat ke baris sebelum/sesudahnya.
- `editor_create_snapshot(const Editor *ed)` : Mengalokasikan blok memori baru dan menyalin setiap karakter secara utuh (`strcpy`). Memastikan data snapshot aman untuk Undo/Redo meskipun editor utama berubah.
- `editor_load_snapshot(Editor *dest, const Editor *src)` : Melakukan pembersihan memori pada editor tujuan, mengalokasikan ulang, lalu menyalin teks beserta posisi kursor dari snapshot.
- `editor_append_line(Editor *ed, const char *text)` : Digunakan oleh modul File untuk memasukkan teks hasil pembacaan `.txt` ke dalam editor.
- `editor_get_line_text(const Editor *ed, int row)` : Mengambil string murni dari baris tertentu untuk keperluan rendering (Raka) maupun penyimpanan file (Faleh).

### 2. raka.h (GUI & Interaksi Tampilan)
- `UpdateKanvasArea(KanvasArea *textArea)` : Membaca dan memproses input dari user (klik mouse, scroll, keyboard, dan shortcut Ctrl+C/X/V/Z/Y) untuk memperbarui status kanvas. Menangani logika interaksi visual seperti kedipan kursor, batasan scroll vertikal, integrasi OS Clipboard (Copy/Cut/Paste), serta menyimpan state untuk sistem Undo. Wajib dipanggil setiap frame di dalam loop utama sebelum proses rendering.
- `DrawKanvasArea(KanvasArea *textArea)` : Merender seluruh elemen visual kanvas ke layar menggunakan Raylib. Menampilkan kotak background, perubahan warna saat fokus, memotong teks agar tidak keluar batas kanvas (Scissor Mode), dan menggambar kursor. Wajib diletakkan di dalam blok `BeginDrawing()` dan `EndDrawing()`.

### 3. faleh.h (Manajemen File I/O)
- `void New_File(Editor *ed)` : Membuat file baru, membersihkan buffer, dan mengatur nama file global menjadi kosong. Memastikan editor siap menerima input teks baru tanpa sisa data dari file sebelumnya.
- `int Open_File(Editor *ed, const char *filename)` : Mengambil file yang tersimpan di hardisk ke memori program agar bisa diedit. Mengembalikan nilai `1` jika berhasil memuat file, dan `0` jika gagal (untuk trigger notifikasi pop-up).
- `int SaveAs(const Editor *ed, const char *filename)` : Membuat file baru atau menimpa file lama dengan nama spesifik, lalu memindahkan data dari layar ke disk. Mengembalikan nilai `1` jika berhasil disimpan, dan `0` jika gagal.
- `int Save(const Editor *ed)` : Menyimpan perubahan pada file yang sedang dibuka secara instan. Mengembalikan nilai `1` jika sukses, dan `0` jika gagal.
- `void Close_File(Editor *ed)` : Memutus koneksi antara program dan file. Membersihkan buffer editor dan nama file global, mengembalikan editor ke kondisi awal.

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
