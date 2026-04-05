#include "raka.h"
#include <string.h>

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
        textArea->scrollY -= GetMouseWheelMove() * 30; // 30px per scroll
        if (textArea->scrollY < 0) textArea->scrollY = 0;

        // 4. Implementasi Hotkeys (Ctrl+C, Ctrl+X, Ctrl+V)
        bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        if (ctrlDown && IsKeyPressed(KEY_C)) {
            // COPY
            const char* line_text = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
            if (line_text) {
                strcpy(textArea->clipboard, line_text);
            }
        }
        else if (ctrlDown && IsKeyPressed(KEY_X)) {
            // CUT
            const char* line_text = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
            if (line_text) {
                strcpy(textArea->clipboard, line_text);
                while(textArea->editor->lines[textArea->editor->cursor_row].length > 0) {
                    editor_backspace(textArea->editor);
                }
            }
        }
        else if (ctrlDown && IsKeyPressed(KEY_V)) {
            // PASTE
            for (int i = 0; i < strlen(textArea->clipboard); i++) {
                editor_insert_char(textArea->editor, textArea->clipboard[i]);
            }
        }

        // 5. Integrasi Navigasi Kursor (Arrow Keys)
        if (IsKeyPressed(KEY_UP)) editor_move_up(textArea->editor);
        if (IsKeyPressed(KEY_DOWN)) editor_move_down(textArea->editor);
        if (IsKeyPressed(KEY_LEFT)) editor_move_left(textArea->editor);
        if (IsKeyPressed(KEY_RIGHT)) editor_move_right(textArea->editor);

        // 6. Logika Menghapus (Backspace)
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            editor_backspace(textArea->editor);
        }

        // 7. Logika Baris Baru (Enter)
        if (IsKeyPressed(KEY_ENTER)) {
            editor_enter(textArea->editor);
        }

        // 8. Logika Mengetik Teks (Huruf, Angka, Spasi, Simbol)
        int charPressed = GetCharPressed();
        while (charPressed > 0) {
            // Pastikan karakter adalah ASCII yang bisa diprint (32 sampai 125)
            if ((charPressed >= 32) && (charPressed <= 125)) {
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