#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "faleh.h"
#include "habib.h"

char global_filename[256] = "";

// Fungsi mengambil lokasi folder Documents Windows //
void GetDocumentsPath(char *path)
{
    char *userProfile = getenv("USERPROFILE");

    if(userProfile != NULL)
    {
        snprintf(path, MAX_PATH, "%s\\Documents", userProfile);
    }
    else
    {
        strncpy(path, ".", MAX_PATH -1);
    }
}

static void BuildFullPath(char *FullPath, const char *docPath, const char *filename)
{
    size_t fnlen = strlen(filename);
    int already_txt = (fnlen >= 4 && strcmp(filename + fnlen -4, ".txt") == 0);

    if(already_txt)
    {
        snprintf(fullPath, MAX_PATH, "%s\\%s", docPath, filename);
    }
    else
    {
        snprintf(fullPath, MAX_PATH, "%s\\%s.txt", docPath, filename);
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
void Open_File(Editor *ed, const char *filename)
    {
        if (ed == NULL || filename == NULL || filename[0] == '\0')
        {
            printf("[Error] Parameter tidak valid.\n");
            return;
        }

        char docPath[MAX_PATH];
        char fullPath[MAX_PATH];
        char buffer[2408];

        GetDocumentsPath(docPath);
        BuildFullPath(fullPath, docPath, filename);

        FILE *fptr = fopen(fullPath, "r");
        if (fptr == NULL) {
            printf("[Error] File tidak ditemukan:\n%s\n" , fullPath);
            return;
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
        
    }

// Fungsi Save As //
void SaveAs(const Editor *ed, const char *filename)
{
    if (ed == NULL || filename == NUL || filename[0] == '\0')
    {
        printf("[Error] Parameter tidak valid.\n");
        return;
    }
    char docPath[MAX_PATH];
    char fullPath[MAX_PATH];

    GetDocumentsPath(docPath);
    BuildFullPath(fullPath, docPath, filename);

    FILE *fptr = fopen(fullPath, "w");

    if (fptr == NULL) 
    {
        printf("[Error] Tidak dapat membuat file: \n%s\n", fullPath);
        return;
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
    
}

// Fungsi Save File //
void Save(const Editor *ed)
{
    if (ed == NULL)
    {
        printf("[Error] Editor tidak valid.\n");
        return;
    }

    if (strlen(global_filename) == 0)
    {
        printf("[System] Gunakan Save As terlebih dahulu.\n");
        return;
    }
        

    FILE *fptr = fopen(global_filename, "w");

    if (fptr == NULL)
    {
        printf("[Error] Gagal menyimpan file .\n");
        return;
        
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





