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
        printf("[warning] Gunakan SaveAs terlebih dahulu untuk memberi nama file!\n");
        return;
    }

    FILE *fptr = fopen(global_filename, "w");
    if (fptr == NULL)
    {
        printf("[Error] Gagal menyimpan file!\n");
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

    fclose(fptr);
    printf("[System] Perubahan pada '%s' berhasil disimpan. \n", global_filename);
}



// Fungsi Close File //
void Close_File(Editor *ed)
{
    editor_free(ed);
    strcpy(global_filename, "");
    printf("[System] koneksi file diputus, Editor ditutup.\n");
}