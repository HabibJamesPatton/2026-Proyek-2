#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "faleh.h"
#include "habib.h"

char global_filename[FILENAME_MAXLEN] = "";

// Fungsi mengambil lokasi folder Documents Windows //
void GetDocumentsPath(char *path)
{
    GetCurrentDirectory(MAX_PATH, path);
}

static void BuildFullPath(char *FullPath, const char *docPath, const char *filename)
{
    size_t fnlen = strlen(filename);
    int already_txt = (fnlen >= 4 && strcmp(filename + fnlen -4, ".txt") == 0);

    if(already_txt)
    {
        snprintf(FullPath, MAX_PATH, "%s\\%s", docPath, filename);
    }
    else
    {
        snprintf(FullPath, MAX_PATH, "%s\\%s.txt", docPath, filename);
    }
}

// Fungsi New File //
void New_File(Editor *ed)
    {
        if (ed == NULL)return;

        editor_free(ed);
        editor_init(ed);
        clear_stack(&undo_stack);
        clear_stack(&redo_stack);

        global_filename[0]='\0';
        printf("[System] Buffer disiapkan untuk file baru.\n");
    }

// Fungsi Open File //
int Open_File(Editor *ed, const char *filename)
    {
        if (ed == NULL || filename == NULL || filename[0] == '\0')
        {
            printf("[Error] Parameter tidak valid.\n");
            return 0; // Gagal (parameter null atau kosong)
        }

        char docPath[MAX_PATH];
        char fullPath[MAX_PATH];
        char buffer[2408];

        GetDocumentsPath(docPath);
        BuildFullPath(fullPath, docPath, filename);

        FILE *fptr = fopen(fullPath, "r");
        if (fptr == NULL) {
            printf("[Error] File tidak ditemukan:\n%s\n" , fullPath);
            return 0;  // Gagal(file tidak ada/tidak bisa dibaca)
        }

        editor_free(ed);
        clear_stack(&undo_stack);
        clear_stack(&redo_stack);

        ed->Head = NULL;
        ed->Tail = NULL;
        ed->total_lines = 0;
        ed->cursor_row = 0;
        ed->cursor_col = 0;

        strncpy(global_filename, fullPath, MAX_PATH - 1);
        global_filename[MAX_PATH - 1] = '\0';

        while (fgets(buffer, sizeof(buffer), fptr))
        {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            editor_append_line(ed, buffer);
        }

        if (ed->total_lines == 0)
        {
            editor_append_line(ed, "");
        }

        fclose(fptr);
        printf("[System] Berhasil memuat file:\n%s\n", fullPath);
        return 1; // sukses membuat file!
        
    }

// Fungsi Save As //
int SaveAs(const Editor *ed, const char *filename)
{
    if (ed == NULL || filename == NULL || filename[0] == '\0')
    {
        printf("[Error] Parameter tidak valid.\n");
        return 0; // Gagagl (parameter null atau kosong)
    }
    char docPath[MAX_PATH];
    char fullPath[MAX_PATH];

    GetDocumentsPath(docPath);
    BuildFullPath(fullPath, docPath, filename);

    FILE *fptr = fopen(fullPath, "w");

    if (fptr == NULL) 
    {
        printf("[Error] Tidak dapat membuat file: \n%s\n", fullPath);
        return 0; // Gagal (tidak punya izin tulis atau path salah)
    }

    for (int i = 0; i < ed->total_lines; i++)
    {
        const char *line_text = editor_get_line_text(ed, i);
        if(line_text != NULL)
        {
            fprintf(fptr, "%s\n", line_text);
        }
    }
     fclose(fptr);
     strncpy(global_filename, fullPath, MAX_PATH - 1);
     global_filename[MAX_PATH - 1] = '\0';

     printf("[System] File berhasil disimpan:\n%s\n", fullPath);
     return 1; // sukses menyimpan file baru!
}

// Fungsi Save File //
int Save(const Editor *ed)
{
    if (ed == NULL)
    {
        printf("[Error] Editor tidak valid.\n");
        return 0; // Gagal
    }

    if (strlen(global_filename) == 0)
    {
        printf("[System] Gunakan Save As terlebih dahulu.\n");
        return 0; // Gagal
    }
        

    FILE *fptr = fopen(global_filename, "w");

    if (fptr == NULL)
    {
        printf("[Error] Gagal menyimpan file .\n");
        return 0; // Gagal
        
    }

    for (int i = 0; i < ed->total_lines; i++)
    {
        const char *line_text = editor_get_line_text(ed, i);

        if (line_text != NULL)
        {
            fprintf(fptr, "%s\n", line_text);
        }
    }

    fclose(fptr);
    printf("[System] File berhasil disimpan.\n");
    return 1; // Sukses!
}

// Fungsi Close File //
void Close_File(Editor *ed) 
{
    if (ed == NULL) return;

    editor_free(ed);
    editor_init(ed);
    clear_stack(&undo_stack);
    clear_stack(&redo_stack);

    global_filename[0] = '\0';
    printf("[System] koneksi file diputus, Editor ditutup.\n");
}





