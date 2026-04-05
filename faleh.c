#include "faleh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fungsi new file //
void new_file(char buffer[MAX_ROWS][MAX_COLS], int *row_count)
    {
        *row_count = 0;
        strcpy(global_filename, "");

        for(int i = 0; i < MAX_ROWS; i++) {
            buffer[i][0] = '\0';
        }
        printf("[system] Buffer dikosongkan, siap membuat file baru.\n")
    }

// Fungsi open file //
void open_file(char buffer[MAX_ROWS][MAX_COLS], int *row_count, char *target_file)
    {
        FILE *fptr = fopen(target_file, "r");
        if (fptr == NULL) {
            printf("[ERROR] Gagal membuka file. File tidak ditemukan!\n");
            return;
        }

        int current_row = 0;
        while (fgets(buffer[current_row], MAX_COLS, fptr) && current_row < MAX_ROWS) 
        {
            buffer[current_row][strcspn(buffer[current_row], "\n")] = 0;
        }

        *row_count = current_row;
        strcpy(global_filename, target_file);
        fclose(fptr);
        printf("[system] File '%s' berhasil dimuat ke buffer.\n", target_file);
    }

// Fungsi Save file //
void save_file(char buffer[MAX_ROWS][MAX_COLS], int row_count)
    {
        if (strlen(global_filename) == 0) 
        {
            printf("[System] File belum memiliki nama. Gunakan Save As!\n");
            return;
        }

        FILE *fptr = fopen(global_filename, "w");
        if (fptr == NULL)
        {
            printf("[ERROR] Gagal menyimpan file!\n");
            return;
        }

        for (int i = 0; i < row_count; i++)
        {
            fprintf(fptr, "%s\n", buffer[i]);
        }

        fclose(fptr);
        printf("[system] Perubahan berhasil disimpan ke '%s'.\n", global_filename);
    }

// Fungsi saveAs //
void save_as_file(char buffer[MAX_ROWS][MAX_COLS], int row_count, char *new_name)
    {
        strcpy(global_filename, new_name);
        save_file(buffer, row_count);
    }

// Fungsi close file //
void close_file(int *row_count)
    {
        *row_count =0;
        strcpy(global_filename, "");
        printf("[system] File ditutup. Buffer dibersihkan.\n");
    }