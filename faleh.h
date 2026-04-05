#ifndef FALEH_H
#define FALEH_H

//Deklarasi Fungsi

// --Fitur New File--
void newFile(); //-- Membuat file baru, memulai dengan buffer kosong, dan mengatur nama file global menjadi string kosong--

// -- Fitur open file --
void openFile(char *filename); //-- Mengambil data dari hardisk ke memori(RAM) agar bisa diedit--

// -- Fitur Save As --
void saveAs(char *filename, char *content); //--Membuat file baru atau menimpa file lama dengan nama spesifik, memindahkan data dari layar ke disk--

// -- Fitur Save --
void save(char *content); /*save: Memindahkan data dari layar ke Disk*/

// -- Fitur Close File --
void closeFile(); // -- Memutus koneksi antara program dan file --

#endif