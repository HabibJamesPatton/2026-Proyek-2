#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "faleh.h"
#include "habib.h"

char global_filename[256] = "";

// Fungsi New File //
void New_File()
    {
        strcpy(global_filename, "");
        printf("[System] Buffer disiapkan untuk file baru (Nama file dikosongkan).\n");
    }

// Fungsi Open File //
void Open_File(char *filename)
    {
        FILE *fptr = fopen(filename, "r");
        if (fptr == NULL) {
            printf("[Error] Gagal membuka file '%s'. Tidak ditemukan!\n", filename);
            return;
        }

        strcpy(global_filename, filename);
        printf("[System] Membuka file: %s\n", filename);

        int c;
        while ((c = fgetc(fptr)) != EOF) 
        {
            printf("%c", (char)c);
        }
        printf("\n-------------\n");

        fclose(fptr);
    }

// Fungsi Save As //
void SaveAs(char *filename, char *content)
{
    FILE *fptr = fopen(filename, "w");
    if (fptr == NULL) 
    {
        printf("[Error] Tidak bisa membuat file '%s'!\n", filename);
        return;
    }

    fprintf(fptr, "%s", content);
    strcpy(global_filename, filename);
    fclose(fptr);
    printf("[System] File berhasil disimpan sebagai: %s\n", filename);

    
}

// Fungsi Save File //
void Save(char *content)
{
    if (strlen(global_filename) == 0)
    {
        printf("[warning] Gunakan SaveAs terlebih dahulu untuk memberi nama file!\n");
        return;
    }

    FILE *fptr = fopen(global_filename, "w");
    if (fptr == NULL)
    {
        printf("[Error] Gagal menyimpan file!\n");
        return;
        
    }

    fprintf(fptr, "%s", content);
    fclose(fptr);
    printf("[System] Perubahan pada '%s' berhasil disimpan. \n", global_filename);
}



// Fungsi Close File //
void Close_File()
{
    strcpy(global_filename, "");
    printf("[System] koneksi file diputus, Editor ditutup.\n", global_filename);
}