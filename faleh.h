#ifndef FALEH_H
#define FALEH_H

//Deklarasi Fungsi

// --Fitur New File--
void New_File(); //-- Membuat file baru, memulai dengan buffer kosong, dan mengatur nama file global menjadi string kosong--

// -- Fitur open file --
void Open_File(char *filename); //-- Mengambil data dari hardisk ke memori(RAM) agar bisa diedit--

// -- Fitur Save As --
void SaveAs(char *filename, char *content); //--Membuat file baru atau menimpa file lama dengan nama spesifik, memindahkan data dari layar ke disk--

// -- Fitur Save --
void Save(char *content); /*save: Memindahkan data dari layar ke Disk*/

// -- Fitur Close File --
void Close_File(); // -- Memutus koneksi antara program dan file --

#endif