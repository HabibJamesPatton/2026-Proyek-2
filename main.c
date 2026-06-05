#include "raylib.h"
#include "habib.h"  // Struktur Data
#include "faleh.h"  // Logika File (Save/Open)
#include "raka.h"   // Logika Tampilan/GUI
#include <string.h> // Untuk fungsi string seperti strlen, strcpy, dll.

#define SHOW_TOAST(msg, col) do { \
    strncpy(toastMsg, msg, 127); \
    toastColor = col; \
    toastTimer = 2.5f; \
} while(0)

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

    // 3. Variabel Konfirmasi New File
    bool showNewFileConfirm = false;

    // 4. Variabel Toast Notification
    char toastMsg[128] = "";
    Color toastColor = GREEN;
    float toastTimer = 0.0f;

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
                    int ok = SaveAs(&myEditor, saveFileName);
                    if (ok) SHOW_TOAST("File berhasil disimpan!", GREEN);
                    else    SHOW_TOAST("Gagal menyimpan file!", RED);
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
                    int ok = Open_File(&myEditor, openFileName);
                    if (ok) SHOW_TOAST("File berhasil dibuka!", GREEN);
                    else    SHOW_TOAST("File tidak ditemukan!", RED);
                }
                showOpenDialog = false; 
            }
            
            if (IsKeyPressed(KEY_ESCAPE)) showOpenDialog = false; 
        }

        // --- C. LOGIKA POP-UP KONFIRMASI NEW FILE (Tombol F3) ---
        else if (showNewFileConfirm) {
            if (IsKeyPressed(KEY_ENTER)) {
                New_File(&myEditor);
                SHOW_TOAST("Buffer baru disiapkan.", GREEN);
                showNewFileConfirm = false;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                showNewFileConfirm = false;
            }
        }

        // --- D. JIKA POP-UP TUTUP, JALANKAN EDITOR SEPERTI BIASA ---
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
                    int ok = Save(&myEditor);
                    if (ok) SHOW_TOAST("File berhasil disimpan!", GREEN);
                    else    SHOW_TOAST("Gagal menyimpan file!", RED);
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

            // Tombol F3 (New File) dengan konfirmasi
            if (IsKeyPressed(KEY_F3)) {
                const char* firstLine = editor_get_line_text(&myEditor, 0);
                bool editorEmpty = (myEditor.total_lines == 1 && firstLine && strlen(firstLine) == 0);
                if (editorEmpty) {
                    New_File(&myEditor);
                    SHOW_TOAST("Buffer baru disiapkan.", GREEN);
                } else {
                    showNewFileConfirm = true;
                }
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

            // --- MENGGAMBAR POP-UP KONFIRMASI NEW FILE ---
            if (showNewFileConfirm) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
                
                DrawRectangle(200, 200, 400, 150, RAYWHITE);
                DrawRectangleLines(200, 200, 400, 150, ORANGE); 
                DrawText("⚠ Buat File Baru?", 220, 220, 18, ORANGE);
                
                DrawText("Perubahan yang belum disimpan akan", 220, 255, 16, BLACK);
                DrawText("hilang. Lanjutkan?", 220, 275, 16, BLACK);
                
                DrawText("Tekan ENTER untuk Ya, ESC untuk Batal", 220, 310, 15, GRAY);
            }

            // --- MENGGAMBAR TOAST NOTIFICATION ---
            if (toastTimer > 0) {
                toastTimer -= GetFrameTime();
                
                // Hitung alpha untuk efek fade out (1 detik terakhir)
                float alpha = (toastTimer < 1.0f) ? toastTimer : 1.0f;
                
                int tw = MeasureText(toastMsg, 18) + 20;
                int tx = GetScreenWidth() - tw - 10;
                int ty = GetScreenHeight() - 45;
                
                Color bg = Fade(toastColor, alpha * 0.85f);
                DrawRectangle(tx, ty, tw, 35, bg);
                DrawText(toastMsg, tx + 10, ty + 9, 18, Fade(WHITE, alpha));
            }

        EndDrawing();
    }

    // --- 4. CLEANUP (Bagian Habib) ---
    editor_free(&myEditor);
    clear_stack(&undo_stack);
    clear_stack(&redo_stack);
    CloseWindow();

    return 0;
}