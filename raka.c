#include "raka.h"
#include <string.h>
#include <stdlib.h>

void UpdateKanvasArea(KanvasArea *textArea) {
    if (!textArea || !textArea->editor) return;

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
            address clickedNode = editor_get_node(textArea->editor, targetRow);
            if (clickedNode != NULL) {
                int relX = mousePoint.x - textArea->Kotak.x - 5;
                int bestCol = 0;
                char temp[1024] = {0};

                for (int c = 0; c <= clickedNode->length; c++) {
                    if (c > 0 && c <= 1023) strncpy(temp, clickedNode->data, c);
                    else if (c == 0) temp[0] = '\0';
                    int w = MeasureText(temp, 20);

                    if (w <= relX) bestCol = c;
                    else break;
                }
                textArea->editor->cursor_col = bestCol;
            }
            
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
                push_undo(textArea->editor); 
                // PENGAMANAN: Pindahkan kursor ke paling kanan baris sebelum menghapus
                // Agar backspace tidak memakan baris yang ada di atasnya
                address currentNode = editor_get_node(textArea->editor, textArea->editor->cursor_row);
                if (currentNode != NULL) {
                    textArea->editor->cursor_col = currentNode->length;
                // Lakukan backspace selama panjang teks di baris (node) ini masih lebih dari 0
                    while(currentNode->length > 0) {
                        editor_backspace(textArea->editor);
                    }
                    if (textArea->editor->cursor_row > 0) {
                        editor_backspace(textArea->editor);
                    }
                }
            }
        }
        else if (ctrlDown && IsKeyPressed(KEY_V)) {
            // PASTE: Mengambil teks dari Clipboard OS (Windows)
            const char* clipboardText = GetClipboardText(); // <- Ambil dari OS

            if (clipboardText != NULL) {
                push_undo(textArea->editor);

                size_t len = strlen(clipboardText);
                for (size_t i = 0; i < len; i++) {
                    char ch = clipboardText[i];

                    // Abaikan CR (\r)
                    if (ch == '\r') continue;

                    // Jika ada newline, buat baris baru
                    if (ch == '\n') {
                        editor_enter(textArea->editor);
                        continue;
                    }

                    // Boundary check (Auto-Enter) berdasarkan lebar teks
                    const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
                    int currentWidth = 0;
                    if (currentLine) currentWidth = MeasureText(currentLine, 20);

                    char tempStr[2] = { ch, '\0' };
                    int charWidth = MeasureText(tempStr, 20);

                    if (currentWidth + charWidth < textArea->Kotak.width - 20) {
                        editor_insert_char(textArea->editor, ch);
                    } else {
                        editor_enter(textArea->editor);
                        editor_insert_char(textArea->editor, ch);
                    }
                }
            }
        }
        if (ctrlDown && IsKeyPressed(KEY_Z)) {
            perform_undo(textArea->editor);
        }

        if (ctrlDown && IsKeyPressed(KEY_Y)){
            perform_redo(textArea->editor);
        }

        // 5. Integrasi Navigasi Kursor (Arrow Keys)
        if (IsKeyPressed(KEY_UP)) editor_move_up(textArea->editor);
        if (IsKeyPressed(KEY_DOWN)) editor_move_down(textArea->editor);
        if (IsKeyPressed(KEY_LEFT)) editor_move_left(textArea->editor);
        if (IsKeyPressed(KEY_RIGHT)) editor_move_right(textArea->editor);

        // Auto-scroll: pastikan kursor selalu terlihat
        {
            int cursorScreenY = 5 + (textArea->editor->cursor_row * 20) - textArea->scrollY;
            if (cursorScreenY + 20 > textArea->Kotak.height) {
                textArea->scrollY = (textArea->editor->cursor_row * 20) - textArea->Kotak.height + 25;
            }
            if (cursorScreenY < 5) {
                textArea->scrollY = textArea->editor->cursor_row * 20;
            }
        }
        
        // 6. Logika Menghapus (Backspace)
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (IsKeyPressed(KEY_BACKSPACE))  push_undo(textArea->editor);
            bool wasMerge = (textArea->editor->cursor_col == 0 && textArea->editor->cursor_row > 0);
            editor_backspace(textArea->editor);
            textArea->lastWasSeparator = true;

            // Boundary check setelah backspace
        if (!wasMerge){

            const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
            int currentWidth = 0;
            if (currentLine) currentWidth = MeasureText(currentLine, 20);

            // Jika baris meluap setelah merge → re-wrap dari titik overflow
            if (currentWidth >= textArea->Kotak.width - 20) {
                address currentNode = editor_get_node(textArea->editor, textArea->editor->cursor_row);
                if (currentNode) {
                    // Cari kolom di mana teks mulai melebihi batas lebar
                    char temp[1024] = {0};
                    int overflowCol = currentNode->length;
                    for (int c = 1; c <= currentNode->length && c <= 1023; c++) {
                        strncpy(temp, currentNode->data, c);
                        temp[c] = '\0';
                        if (MeasureText(temp, 20) >= textArea->Kotak.width - 20) {
                            overflowCol = c - 1;
                            if (overflowCol < 1) overflowCol = 1;
                            break;
                        }
                    }

                    // Cari spasi terakhir sebelum titik overflow untuk word-wrap
                    int spacePos = overflowCol;
                    while (spacePos > 0 && currentNode->data[spacePos - 1] != ' ') {
                        spacePos--;
                    }
                    int breakCol = (spacePos > 0) ? spacePos : overflowCol;

                    // Simpan posisi kursor, lalu potong baris di breakCol
                    int savedCol = textArea->editor->cursor_col;
                    textArea->editor->cursor_col = breakCol;
                    editor_enter(textArea->editor);

                    // Kembalikan kursor ke posisi yang benar
                    if (savedCol >= breakCol) {
                        // Kursor setelah titik potong → pindah ke baris baru
                        textArea->editor->cursor_col = savedCol - breakCol;
                    } else {
                        // Kursor sebelum titik potong → kembali ke baris atas
                        textArea->editor->cursor_row--;
                        textArea->editor->cursor_col = savedCol;
                    }
                }
            }
        
            // Jika baris punya ruang → coba tarik baris bawah ke atas
            else {
                address currentNode = editor_get_node(textArea->editor, textArea->editor->cursor_row);
                if (currentNode && currentNode->next) {
                    address nextNode = currentNode->next;
                    char combined[2048] = {0};
                    int combinedLen = currentNode->length + nextNode->length;
                    if (combinedLen < 2047) {
                        if (currentNode->length > 0) memcpy(combined, currentNode->data, currentNode->length);
                        memcpy(combined + currentNode->length, nextNode->data, nextNode->length);
                        combined[combinedLen] = '\0';

                        int combinedWidth = MeasureText(combined, 20);
                        if (combinedWidth < textArea->Kotak.width - 20) {
                            int savedCol = textArea->editor->cursor_col;
                            textArea->editor->cursor_row++;
                            textArea->editor->cursor_col = 0;
                            editor_backspace(textArea->editor);
                            textArea->editor->cursor_col = savedCol;
                        }
                    }
                }
            }
        }

            int cursorScreenY = 5 + (textArea->editor->cursor_row * 20) - textArea->scrollY;
            if (cursorScreenY + 20 > textArea->Kotak.height) {
                textArea->scrollY = (textArea->editor->cursor_row * 20) - textArea->Kotak.height + 25;
            }
            if (cursorScreenY < 5) {
                textArea->scrollY = textArea->editor->cursor_row * 20;
            }
        }

        // 7. Logika Baris Baru (Enter)
        if (IsKeyPressed(KEY_ENTER)) {
            push_undo(textArea->editor);
            editor_enter(textArea->editor);
            textArea->lastWasSeparator = true;

            int cursorScreenY = 5 + (textArea->editor->cursor_row * 20) - textArea->scrollY;
            if (cursorScreenY + 20 > textArea->Kotak.height) {
                textArea->scrollY = (textArea->editor->cursor_row * 20) - textArea->Kotak.height + 25;
            }
            if (cursorScreenY < 5) {
                textArea->scrollY = textArea->editor->cursor_row * 20;
            }
        }

        // 8. Logika Mengetik Teks (Huruf, Angka, Spasi, Simbol)
        int charPressed = GetCharPressed();

        while (charPressed > 0) {
            if ((charPressed >= 32) && (charPressed <= 126)) {

                bool isSeparator = (charPressed == ' ' || charPressed == '\t' || charPressed == '\n' || charPressed == '.' || charPressed == ',' || charPressed == ';' || charPressed == ':' || charPressed == '!' || charPressed == '?');
                if (isSeparator){
                    if (!textArea->lastWasSeparator){
                        push_undo(textArea->editor);
                    }
                    textArea->lastWasSeparator = true;
                }else{
                    if (textArea->lastWasSeparator){
                        push_undo(textArea->editor);
                    }
                    textArea->lastWasSeparator = false;
                }

                const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
                int currentWidth = 0;
                if (currentLine) currentWidth = MeasureText(currentLine, 20);

                char tempStr[2] = { (char)charPressed, '\0' };
                int charWidth = MeasureText(tempStr, 20);

                if (currentWidth + charWidth >= textArea->Kotak.width - 20) {

                    int col = textArea->editor->cursor_col;
                    int stepsBack = 0;
                    
                    const char* currentLine = editor_get_line_text(textArea->editor, textArea->editor->cursor_row);
                    // 1. Mundur untuk mencari spasi terakhir di baris ini
                    while (col > 0 && currentLine[col - 1] != ' ') {
                        col--;
                        stepsBack++;
                    }

                    // 2. Jika ketemu spasi, turunkan kata tersebut secara utuh
                    if (col > 0 && stepsBack > 0) {
                        // Geser kursor ke depan kata
                        for (int i = 0; i < stepsBack; i++) editor_move_left(textArea->editor);
                        // Enter!
                        editor_enter(textArea->editor);
                        
                        // Kembalikan kursor ke ujung kata di baris yang baru
                        for (int i = 0; i < stepsBack; i++) editor_move_right(textArea->editor);
                    }
                    // 3. Jika ini kata panjang tanpa spasi sama sekali, potong paksa karakternya
                    else {
                        editor_enter(textArea->editor);
                    }
                }

                editor_insert_char(textArea->editor, (char)charPressed);

                // Auto-scroll saat pengetikan
                {
                    int cursorScreenY = 5 + (textArea->editor->cursor_row * 20) - textArea->scrollY;
                    if (cursorScreenY + 20 > textArea->Kotak.height) {
                        textArea->scrollY = (textArea->editor->cursor_row * 20) - textArea->Kotak.height + 25;
                    }
                    if (cursorScreenY < 5) {
                        textArea->scrollY = textArea->editor->cursor_row * 20;
                    }
                }
            }
            charPressed = GetCharPressed();
        }
    }
}

void DrawKanvasArea(KanvasArea *textArea) {
    if (!textArea || !textArea->editor) return;

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
                DrawText(textToDraw, startX, posY, 20, textArea->textColor);
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
            int colToCopy = textArea->editor->cursor_col;
            if (colToCopy > 1023) colToCopy = 1023;  // ← BATAS AMAN
            
            strncpy(temp, currentLineText, colToCopy);
            temp[colToCopy] = '\0';
        }
        
        int textWidth = MeasureText(temp, 20);
        int cursorPosX = startX + textWidth;

        // Gambar kursor hanya jika posisinya berada di dalam jangkauan visual kanvas
        if (cursorPosY >= textArea->Kotak.y && cursorPosY <= textArea->Kotak.y + textArea->Kotak.height) {
            DrawRectangle(cursorPosX, cursorPosY, 2, lineHeight, textArea->textColor);
        }
    }

    EndScissorMode();
}   