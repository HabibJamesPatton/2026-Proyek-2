#ifndef RAKA_H
#define RAKA_H

#include "raylib.h"
#include "habib.h" 

// Struktur data untuk text selection
typedef struct {
    bool isSelecting;       // Currently dragging to select
    int startRow, startCol; // Selection start position
    int endRow, endCol;     // Selection end position
    bool isActive;          // Whether a selection exists
} TextSelection;

// Struktur data untuk elemen kanvas teks editor
typedef struct {
    Rectangle Kotak;    // Menyimpan x, y, width, dan height
    Color bgColor;      // Warna latar belakang kanvas
    Color borderColor;  // Warna garis tepi
    Color textColor;    // Warna teks
    bool isFocused;     // Status apakah kanvas sedang aktif/diklik

    Editor *editor;     // Pointer ke mesin teks utama

    float blinkTimer;   // Timer untuk efek kursor berkedip
    int scrollY;        // Posisi scroll vertikal (atas-bawah)
    bool lastWasSeparator;

    TextSelection selection;    // Text selection state
    Color selectionBgColor;     // Background color for selection
} KanvasArea;
    
// Deklarasi fungsi-fungsi GUI
void UpdateKanvasArea(KanvasArea *textArea);
void DrawKanvasArea(KanvasArea *textArea);

#endif