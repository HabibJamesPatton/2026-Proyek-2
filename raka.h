#ifndef RAKA_H
#define RAKA_H

#include "raylib.h"

// Struktur data untuk elemen kanvas teks editor
typedef struct {
    Rectangle Kotak;   // Menyimpan x, y, width, dan height
    Color bgColor;      // Warna latar belakang kanvas
    Color borderColor;  // Warna garis tepi
    Color textColor;    // Warna teks
    bool isFocused;     // Status apakah kanvas sedang aktif/diklik
    char* text;         // Pointer ke isi teks (untuk sementara)
} KanvasArea;

// Deklarasi fungsi-fungsi GUI
void UpdateKanvasArea(KanvasArea *textArea);
void DrawKanvasArea(KanvasArea*textArea);

#endif