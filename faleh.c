#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "faleh.h"
#include "habib.h"

char global_filename[256] = "";

// Fungsi New File //
void New_File(Editor *ed)
    {
        editor_free(ed);
        editor_init(ed);

        global_filename[0]='\0';
        printf("[System] Buffer disiapkan untuk file baru.\n");
    }

// Fungsi Open File //
void Open_File(Editor *ed, const char *filename)
    {
        FILE *fptr = fopen(filename, "r");
        if (fptr == NULL) {
            printf("[Error] Gagal membuka file '%s'!\n", filename);
            return;
        }

        editor_free(ed);
        editor_init(ed);
        ed->total_lines = 0;

        strncpy(global_filename, filename, sizeof(global_filename)-1);
        global_filename[sizeof(global_filename)-1] = '\0';
        printf("[System] Membuka file: %s\n", filename);

        char buffer[2048];
        while (fgets(buffer, sizeof(buffer), fptr))
        {
            buffer[strcspn(buffer, "\n")] = 0;
            editor_append_line(ed, buffer);
        }

        if (ed->total_lines == 0)
        {
            editor_append_line(ed, "");
        }

        fclose(fptr);
        printf("[System] Berhasil memuat file: %s\n", filename);
        
    }

// Fungsi Save As //
void SaveAs(const Editor *ed, const char *filename)
{
    FILE *fptr = fopen(filename, "w");
    if (fptr == NULL) 
    {
        printf("[Error] Tidak bisa membuat file '%s'!\n", filename);
        return;
    }

    for(int i = 0; i < ed->total_lines; i++)
    {
        const char *line_text = editor_get_line_text(ed, i);
        if(line_text != NULL)
        {
            fprintf(fptr, "%s\n", line_text);
        }
    }
    strcpy(global_filename, filename);
    fclose(fptr);
    printf("[System] File berhasil disimpan sebagai: %s\n", filename);
    
}

// Fungsi Save File //
void Save(const Editor *ed)
{
    if (strlen(global_filename) == 0)
    {
        char input_nama[256];
        printf("[System] File belum memiliki nama.\n");
        printf(">> Masukkan nama file untuk menyimpan:");

        if (scanf("%255s", input_nama) == 1)
        {
            SaveAs(ed, input_nama);
        }
        else
        {
            printf("[Error] Input nama file tidak valid!\n");
        }
        return;
    }

    FILE *fptr = fopen(global_filename, "w");
    if (fptr == NULL)
    {
        printf("[Error] Gagal menyimpan file '%s'!\n", global_filename);
        return;
        
    }

    for (int i=0; i< ed -> total_lines: i++)
    {
        const char *line_text = editor_get_line_text(ed, i);
        if (line_text != NULL)
        {
            fprintf(fptr, "%s\n", line-text);
        }
    }

    fclose(fptr);
    printf("[System] File berhasil disimpan: %s\n", global_filename);
}


void Rename_File() {
    if (strlen(global_filename) == 0) {
        printf("[System] Tidak ada file aktif untuk di-rename.\n");
        return;
    }
    char input_nama[256];
    printf("\n>> Masukkan nama file baru: ");
    if (scanf("%255s", input_nama) == 1) {
        if (rename(global_filename, input_nama) == 0) {
            printf("[System] File berhasil di-rename menjadi: %s\n", input_nama);
            strcpy(global_filename, input_nama);
        } else {
            printf("[Error] Gagal me-rename file.\n");
        }
    }
}

void Delete_File(Editor *ed) {
    if (strlen(global_filename) == 0) {
        printf("[System] Tidak ada file aktif untuk dihapus.\n");
        return;
    }
    printf("\n>> Anda yakin ingin menghapus '%s' secara permanen? (y/n): ", global_filename);
    char konfirmasi;
    if (scanf(" %c", &konfirmasi) == 1 && (konfirmasi == 'y' || konfirmasi == 'Y')) {
        if (remove(global_filename) == 0) {
            printf("[System] File '%s' berhasil dihapus permanen.\n", global_filename);
            New_File(ed); // Bersihkan editor karena file sudah tidak eksis
        } else {
            printf("[Error] Gagal menghapus file.\n");
        }
    } else {
         printf("[System] Penghapusan file dibatalkan.\n");
    }
}

void Close_File(Editor *ed) {
    editor_free(ed);
    strcpy(global_filename, "");
    printf("[System] koneksi file diputus, Editor ditutup.\n");
}




// Fungsi Close File //
