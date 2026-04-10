#include "raka.h"
#include <string.h>
#include <stdlib.h>

void SaveUndoState(KanvasArea *textArea) {
    // Jika tumpukan penuh, geser data ke kiri (buang foto yang paling usang)
    if (textArea->undoCount >= MAX_UNDO_STEPS) {
        editor_free(textArea->undoStack[0]);
        free(textArea->undoStack[0]);
        
        for (int i = 1; i < MAX_UNDO_STEPS; i++) {
            textArea->undoStack[i - 1] = textArea->undoStack[i];
        }
        textArea->undoCount--;
    }
    
    // Tambahkan foto baru di posisi paling atas tumpukan
    textArea->undoStack[textArea->undoCount] = editor_create_snapshot(textArea->editor);
    textArea->undoCount++;
}

void UpdateKanvasArea(KanvasArea *textArea) {
    Vector2 mousePoint = GetMousePosition();

    // 1. Integrasi Mouse Click (Fokus & Pindah Kursor)
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePoint, textArea->Kotak)) {
            textArea->isFocused = true;
            
            // Konversi posisi klik Y (mouse) menjadi indeks baris kursor Habib
            int relY = mousePoint.y - textArea->Kotak.y - 5 + textArea->scrollY;
            int targetRow = relY / 20; // 20 adalah tinggi satu baris teks
            
            if (targetRow < 0) targetRow = 0;
            if (targetRow >= textArea->editor->total_lines) targetRow = textArea->editor->total_lines - 1;
            
            textArea->editor->cursor_row = targetRow;
            textArea->editor->cursor_col = textArea->editor->lines[targetRow].length;
            
        } else {
            textArea->isFocused = false;
        }
    }

    if (textArea->isFocused) {
        // 2. Logika Kedipan Kursor
        textArea->blinkTimer += GetFrameTime();
        if (textArea->blinkTimer >= 1.0f) textArea->blinkTimer = 0.0f;

        // 3. Implementasi Scroll Vertikal (Mouse Wheel)
        textArea->scrollY -= GetMouseWheelMove() * 30;
        if (textArea->scrollY < 0) textArea->scrollY = 0;

        int tinggiTeksTotal = textArea->editor->total_lines * 20;
        int maxScrollY = tinggiTeksTotal - textArea->Kotak.height + 20;

        if (maxScrollY < 0) maxScrollY = 0;
        if (textArea->scrollY > maxScrollY) textArea->scrollY = maxScrollY;

        // 4. Implementasi Hotkeys (Ctrl+C, Ctrl+X, Ctrl+V, Ctrl+Z)
        bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        if (ctrlDown && IsKeyPressed(KEY_C)) {
            // COPY: Menyalin seluruh baris aktif ke Clipboard OS (Windows)
            const char* line_text = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
            if (line_text) {
                SetClipboardText(line_text); // 
            }
        }
        else if (ctrlDown && IsKeyPressed(KEY_X)) {
            const char* line_text = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
            if (line_text) {
                SetClipboardText(line_text); 
                
                // PENGAMANAN: Pindahkan kursor ke paling kanan baris sebelum menghapus
                // Agar backspace tidak memakan baris yang ada di atasnya
                textArea->editor->cursor_col = textArea->editor->lines[textArea->editor->cursor_row].length;
                
                while(textArea->editor->lines[textArea->editor->cursor_row].length > 0) {
                    editor_backspace(textArea->editor);
                }
            }
        }
        else if (ctrlDown && IsKeyPressed(KEY_V)) {
            // PASTE: Mengambil teks dari Clipboard OS (Windows)
            const char* clipboardText = GetClipboardText(); // <- Ambil dari OS
            
            if (clipboardText != NULL) {
                for (int i = 0; i < strlen(clipboardText); i++) {
                    char charToPaste = clipboardText[i];
                    
                    // Filter 1: Abaikan karakter Carriage Return bawaan Windows (\r)
                    if (charToPaste == '\r') continue;
                    
                    // Filter 2: Jika dari teks luar ada Enter, buat baris baru
                    if (charToPaste == '\n') {
                        editor_enter(textArea->editor);
                        continue;
                    }
                    
                    // Logika boundary check (Auto-Enter)
                    const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
                    int currentWidth = 0;
                    if (currentLine) {
                        currentWidth = MeasureText(currentLine, 20); 
                    }

                    if (currentWidth + 15 < textArea->Kotak.width - 20) {
                        editor_insert_char(textArea->editor, charToPaste);
                    } else {
                        editor_enter(textArea->editor);
                        editor_insert_char(textArea->editor, charToPaste);
                    }
                }
            }
        }
        if (ctrlDown && IsKeyPressed(KEY_Z)) {
            // UNDO: Kembalikan keadaan dari snapshot
            if (textArea->undoCount > 0) {
                // Turunkan index ke posisi snapshot terakhir
                textArea->undoCount--;

                Editor *snapshot = textArea->undoStack[textArea->undoCount];
                
                editor_load_snapshot(textArea->editor, snapshot);

                editor_free(snapshot);
                free(snapshot);
            }
        }

        // 5. Integrasi Navigasi Kursor (Arrow Keys)
        if (IsKeyPressed(KEY_UP)) editor_move_up(textArea->editor);
        if (IsKeyPressed(KEY_DOWN)) editor_move_down(textArea->editor);
        if (IsKeyPressed(KEY_LEFT)) editor_move_left(textArea->editor);
        if (IsKeyPressed(KEY_RIGHT)) editor_move_right(textArea->editor);

        // 6. Logika Menghapus (Backspace)
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            SaveUndoState(textArea);
            editor_backspace(textArea->editor);
        }

        // 7. Logika Baris Baru (Enter)
        if (IsKeyPressed(KEY_ENTER)) {
            SaveUndoState(textArea);
            editor_enter(textArea->editor);
        }

        // 8. Logika Mengetik Teks (Huruf, Angka, Spasi, Simbol)
        int charPressed = GetCharPressed();
        while (charPressed > 0) {
            if ((charPressed >= 32) && (charPressed <= 125)) {
                
                if (charPressed == 32) {
                    SaveUndoState(textArea); // Simpan riwayat undo saat tekan Spasi
                }

                const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
                int currentWidth = 0;
                if (currentLine) {
                    currentWidth = MeasureText(currentLine, 20); 
                }

                // Ukur lebar huruf yang mau diketik
                char tempStr[2] = { (char)charPressed, '\0' };
                int charWidth = MeasureText(tempStr, 20);

                // --- FITUR SMART WORD WRAP ---
                // Jika baris ini ditambah huruf baru akan menabrak batas kanan (margin - 20)
                if (currentWidth + charWidth >= textArea->Kotak.width - 20) {
                    
                    int col = textArea->editor->cursor_col;
                    int stepsBack = 0;
                    
                    // 1. Mundur untuk mencari spasi terakhir di baris ini
                    while (col > 0 && currentLine[col - 1] != ' ') {
                        col--;
                        stepsBack++;
                    }
                    
                    // 2. Jika ketemu spasi, turunkan kata tersebut secara utuh
                    if (col > 0 && stepsBack > 0) {
                        // Geser kursor ke depan kata
                        for (int i = 0; i < stepsBack; i++) editor_move_left(textArea->editor);
                        
                        // Enter! (Kata akan terdorong ke bawah)
                        editor_enter(textArea->editor);
                        
                        // Kembalikan kursor ke ujung kata di baris yang baru
                        for (int i = 0; i < stepsBack; i++) editor_move_right(textArea->editor);
                    } 
                    // 3. Jika ini kata super panjang tanpa spasi sama sekali, potong paksa karakternya
                    else {
                        editor_enter(textArea->editor);
                    }
                }

                // Masukkan hurufnya setelah dipastikan aman dari batas margin
                editor_insert_char(textArea->editor, (char)charPressed);
            }
            charPressed = GetCharPressed();
        }
    }
}

void DrawKanvasArea(KanvasArea *textArea) {
    // 1. Gambar kotak dasar GUI
    DrawRectangleRec(textArea->Kotak, textArea->bgColor);
    Color currentBorder = textArea->isFocused ? BLUE : textArea->borderColor;
    DrawRectangleLinesEx(textArea->Kotak, 2.0f, currentBorder);

    int startY = textArea->Kotak.y + 5;
    int startX = textArea->Kotak.x + 5;
    int lineHeight = 20;

    // SCISSOR MODE: Memotong area agar teks tidak keluar batas kotak saat di-scroll
    BeginScissorMode(textArea->Kotak.x, textArea->Kotak.y, textArea->Kotak.width, textArea->Kotak.height);

    // 2. Rendering Teks Multi-baris
    for (int i = 0; i < textArea->editor->total_lines; i++) {
        int posY = startY + (i * lineHeight) - textArea->scrollY;
        
        // Optimasi: Hanya gambar baris teks yang benar-benar ada di dalam jangkauan layar
        if (posY + lineHeight > textArea->Kotak.y && posY < textArea->Kotak.y + textArea->Kotak.height) {
            const char* textToDraw = editor_get_line_text(textArea->editor, i);
            if (textToDraw) {
                DrawText(textToDraw, startX - textArea->scrollX, posY, 20, textArea->textColor);
            }
        }
    }

    // 3. Visualisasi Kursor
    if (textArea->isFocused && textArea->blinkTimer < 0.5f) {
        int cursorPosY = startY + (textArea->editor->cursor_row * lineHeight) - textArea->scrollY;
        
        // Menghitung lebar teks untuk menentukan posisi X kursor
        char temp[1024] = {0};
        const char* currentLineText = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
        
        if (currentLineText && textArea->editor->cursor_col > 0) {
            strncpy(temp, currentLineText, textArea->editor->cursor_col);
        }
        
        int textWidth = MeasureText(temp, 20);
        int cursorPosX = startX + textWidth - textArea->scrollX;

        // Gambar kursor hanya jika posisinya berada di dalam jangkauan visual kanvas
        if (cursorPosY >= textArea->Kotak.y && cursorPosY <= textArea->Kotak.y + textArea->Kotak.height) {
            DrawRectangle(cursorPosX, cursorPosY, 2, lineHeight, textArea->textColor);
        }
    }

    EndScissorMode();
}