#include "raylib.h"
#include "habib.h"  // Struktur Data
#include "faleh.h"  // Logika File (Save/Open)
#include "raka.h"   // Logika Tampilan/GUI
#include <string.h> // Untuk fungsi string seperti strlen, strcpy, dll.

int main() {
    // --- 1. INISIALISASI (Bagian Habib & Raka) ---
    InitWindow(800, 600, "Text Editor Tim Kelompok");
    SetExitKey(0); 
    SetTargetFPS(60);

    // Siapkan Mesin Editor (Habib)
    Editor myEditor;
    editor_init(&myEditor);
    init_stacks(); 

    // Siapkan Area Tampilan (Raka)
    KanvasArea myCanvas = {
        .Kotak = { 50, 50, 700, 450 },
        .bgColor = RAYWHITE,
        .borderColor = DARKGRAY,
        .textColor = BLACK,
        .isFocused = false,
        .editor = &myEditor, // Hubungkan kanvas Raka ke mesin Habib
        .scrollY = 0,
        .blinkTimer = 0
    };

    // --- VARIABEL UNTUK FITUR POP-UP ---
    // 1. Variabel Save As
    bool showSaveDialog = false;
    char saveFileName[256] = "";
    int saveLetterCount = 0;

    // 2. Variabel Open File
    bool showOpenDialog = false;
    char openFileName[256] = "";
    int openLetterCount = 0;

    // --- 2. LOOP UTAMA ---
    while (!WindowShouldClose()) {
        
        // --- A. LOGIKA POP-UP SAVE AS (Tombol F1) ---
        if (showSaveDialog) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (saveLetterCount < 250)) {
                    saveFileName[saveLetterCount] = (char)key;
                    saveFileName[saveLetterCount+1] = '\0'; 
                    saveLetterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                saveLetterCount--;
                if (saveLetterCount < 0) saveLetterCount = 0;
                saveFileName[saveLetterCount] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (saveLetterCount > 0) { 
                    SaveAs(&myEditor, saveFileName);
                }
                showSaveDialog = false; 
            }
            
            if (IsKeyPressed(KEY_ESCAPE)) showSaveDialog = false; 
        } 
        
        // --- B. LOGIKA POP-UP OPEN FILE (Tombol F2) ---
        else if (showOpenDialog) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (openLetterCount < 250)) {
                    openFileName[openLetterCount] = (char)key;
                    openFileName[openLetterCount+1] = '\0'; 
                    openLetterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                openLetterCount--;
                if (openLetterCount < 0) openLetterCount = 0;
                openFileName[openLetterCount] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (openLetterCount > 0) { 
                    Open_File(&myEditor, openFileName);
                }
                showOpenDialog = false; 
            }
            
            if (IsKeyPressed(KEY_ESCAPE)) showOpenDialog = false; 
        }

        // --- C. JIKA POP-UP TUTUP, JALANKAN EDITOR SEPERTI BIASA ---
        else {
            UpdateKanvasArea(&myCanvas);

            bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
            if (ctrlDown && IsKeyPressed(KEY_S)) {
                // Cek apakah file sudah punya nama
                if (strlen(global_filename) == 0) {
                    // Jika file baru, panggil GUI Pop-up Save As (seperti F1)
                    showSaveDialog = true;
                    saveFileName[0] = '\0'; 
                    saveLetterCount = 0;
                } else {
                    // Jika sudah punya nama, langsung eksekusi mesin Save
                    Save(&myEditor);
                }
            }
            
            // Buka Pop-up Save As jika F1 ditekan
            if (IsKeyPressed(KEY_F1)) {
                showSaveDialog = true;
                saveFileName[0] = '\0'; 
                saveLetterCount = 0;
            }
            
            // Buka Pop-up Open File jika F2 ditekan
            if (IsKeyPressed(KEY_F2)) {
                showOpenDialog = true;
                openFileName[0] = '\0'; 
                openLetterCount = 0;
            }

            // Tombol F3 biarkan (New File)
            if (IsKeyPressed(KEY_F3)) {
                New_File(&myEditor);
            }

            // Tombol Ctrl+S untuk Save
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
                Save(&myEditor);
            }
        }

        // --- 3. RENDERING (Tampilan) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("F1: Save As | F2: Open | F3: New", 50, 20, 20, GRAY);
            
            // Gambar Editor (Bagian Raka)
            DrawKanvasArea(&myCanvas);

            // --- MENGGAMBAR POP-UP SAVE AS ---
            if (showSaveDialog) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
                
                DrawRectangle(200, 200, 400, 150, RAYWHITE);
                DrawRectangleLines(200, 200, 400, 150, DARKGRAY);
                DrawText("Masukkan Nama File untuk Disimpan", 220, 220, 18, DARKGRAY);
                
                DrawRectangle(220, 250, 360, 40, LIGHTGRAY);
                DrawText(saveFileName, 230, 260, 20, BLACK);
                
                if (((int)(GetTime() * 2)) % 2 == 0) {
                    DrawText("_", 230 + MeasureText(saveFileName, 20), 260, 20, DARKGRAY);
                }

                DrawText("Tekan ENTER untuk Simpan, ESC untuk Batal", 220, 310, 15, GRAY);
            }

            // --- MENGGAMBAR POP-UP OPEN FILE ---
            if (showOpenDialog) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
                
                DrawRectangle(200, 200, 400, 150, RAYWHITE);
                // Dikasih warna biru biar kelihatan bedanya dengan Save As
                DrawRectangleLines(200, 200, 400, 150, BLUE); 
                DrawText("Masukkan Nama File untuk Dibuka", 220, 220, 18, BLUE);
                
                DrawRectangle(220, 250, 360, 40, LIGHTGRAY);
                DrawText(openFileName, 230, 260, 20, BLACK);
                
                if (((int)(GetTime() * 2)) % 2 == 0) {
                    DrawText("_", 230 + MeasureText(openFileName, 20), 260, 20, BLUE);
                }

                DrawText("Tekan ENTER untuk Buka, ESC untuk Batal", 220, 310, 15, GRAY);
            }

        EndDrawing();
    }

    // --- 4. CLEANUP (Bagian Habib) ---
    editor_free(&myEditor);
    CloseWindow();

    return 0;
}