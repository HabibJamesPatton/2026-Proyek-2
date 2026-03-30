#ifndef FALEH_H
#define FALEH_H

//Deklarasi Fungsi

/*new: Mengosongkan layar editor*/
void newFile()

/*open: Mengambil data dari hardisk ke memori(RAM) agar bisa diedit*/
void openFile(char *filename)

/*saveAs: Membuat file baru atau menimpa file lama dengan nama spesifik, memindahkan data dari layar ke disk*/
void saveAs(char *filename, char *content)

/*save: Memindahkan data dari layar ke Disk*/
void save(char *content)

/*Memutus koneksi antara program dan file*/
void closeFile()

#endif