#include "raylib.h"
#include "habib.h"  // Struktur Data
#include "faleh.h"  // Logika File (Save/Open)
#include "raka.h"   // Logika Tampilan/GUI

int main() {
    // --- 1. INISIALISASI (Bagian Habib & Raka) ---
    InitWindow(800, 600, "Text Editor Tim Kelompok");
    SetTargetFPS(60);

    // Siapkan Mesin Editor (Habib)
    Editor myEditor;
    editor_init(&myEditor);

    // Siapkan Area Tampilan (Raka)
    KanvasArea myCanvas = {
        .Kotak = { 50, 50, 700, 450 },
        .bgColor = RAYWHITE,
        .borderColor = DARKGRAY,
        .textColor = BLACK,
        .isFocused = false,
        .editor = &myEditor, // Hubungkan kanvas Raka ke mesin Habib
        .scrollY = 0,
        .blinkTimer = 0,
        .undoCount = 0
    };

    // --- 2. LOOP UTAMA ---
    while (!WindowShouldClose()) {
        
        // A. Update Logic (Bagian Raka)
        UpdateKanvasArea(&myCanvas);

        // B. Contoh Pemanggilan Fitur Kamu (Bagian Faleh)
        // Misal: Tekan F1 untuk Save, F2 untuk Open
        if (IsKeyPressed(KEY_F1)) {
            SaveAs(&myEditor, "hasil_kerja.txt");
        }
        if (IsKeyPressed(KEY_F2)) {
            Open_File(&myEditor, "hasil_kerja.txt");
        }
        if (IsKeyPressed(KEY_F3)) {
            New_File(&myEditor);
        }

        // --- 3. RENDERING (Tampilan) ---
        BeginDrawing();
            ClearBackground(WHITE);
            DrawText("F1: Save | F2: Open | F3: New", 50, 20, 20, GRAY);
            
            // Gambar Editor (Bagian Raka)
            DrawKanvasArea(&myCanvas);
        EndDrawing();
    }

    // --- 4. CLEANUP (Bagian Habib) ---
    editor_free(&myEditor);
    CloseWindow();

    return 0;
}