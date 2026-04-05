#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "faleh.h"

char global_filename[256] = "";

// Fungsi new file //
void newFile()
    {
        strcpy(global_filename, "");
        printf("[System] Buffer disiapkan untuk file baru (Nama file dikosongkan).\n");
    }

// Fungsi open file //
void open_file(char *filename)
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

// Fungsi saveAs //
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

// Fungsi Save file //
void save_file(char *content)
{
    if (strlen(global_filename) == 0)
    {
        printf("[warning] Gunakan saveAs terlebih dahulu untuk memberi nama file!\n");
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



// Fungsi close file //
void close_file()
{
    strcpy(global_filename, "");
    printf("[System] koneksi file diputus, Editor ditutup.\n", global_filename);
}