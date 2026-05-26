#ifndef FALEH_H
#define FALEH_H
#include "habib.h"

//Deklarasi variabel global untuk nama file extern char global_filename[256];
extern char global_filename[256];
//Deklarasi Fungsi

// --Fitur New File--
void New_File(Editor *ed); //-- Membuat file baru, memulai dengan buffer kosong, dan mengatur nama file global menjadi string kosong--

// -- Fitur open file --
void Open_File(Editor *ed, const char *filename); //-- Mengambil data dari hardisk ke memori(RAM) agar bisa diedit--

// -- Fitur Save As --
void SaveAs(const Editor *ed, const char *filename); //--Membuat file baru atau menimpa file lama dengan nama spesifik, memindahkan data dari layar ke disk--

// -- Fitur Save --
void Save(const Editor *ed); // -- Memindahkan data dari layar ke Disk --

// -- Fitur Rename File --
void Rename_File(); // -- Mengubah nama file yang sedang aktif --

// -- Fitur Delete File --
void Delete_File(Editor *ed); // -- Menghapus file yang sedang aktif dari disk dan membersihkan buffer --

// -- Fitur Close File --
void Close_File(Editor *ed); // -- Memutus koneksi antara program dan file --

#endif