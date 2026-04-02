#include "raka.h"
#include <stddef.h> // Untuk NULL

// Fungsi untuk mengecek interaksi mouse dengan kanvas teks
void UpdateKanvasArea(KanvasArea *textArea) {
    // Dapatkan posisi koordinat mouse saat ini
    Vector2 mousePoint = GetMousePosition();

    // Cek apakah tombol kiri mouse baru saja diklik
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Logika Bounding Box: Cek apakah titik mouse berada di dalam kotak kanvas
        if (CheckCollisionPointRec(mousePoint, textArea->Kotak)) {
            textArea->isFocused = true;  // Aktifkan kanvas
        } else {
            textArea->isFocused = false; // Nonaktifkan jika klik di luar kanvas
        }
    }
}

// Fungsi untuk menggambar kanvas teks ke layar
void DrawKanvasArea(KanvasArea *textArea) {
    // 1. Gambar kotak latar belakang kanvas
    DrawRectangleRec(textArea->Kotak, textArea->bgColor);

    // 2. Gambar garis tepi (border)
    // Jika kanvas sedang 'fokus', ubah warna border menjadi biru agar terlihat aktif
    Color currentBorder = textArea->isFocused ? BLUE : textArea->borderColor;
    DrawRectangleLinesEx(textArea->Kotak, 2.0f, currentBorder);

    // 3. Gambar teks sementara di dalam kanvas (padding 5px dari kiri dan atas)
    if (textArea->text != NULL) {
        DrawText(textArea->text, (int)textArea->Kotak.x + 5, (int)textArea->Kotak.y + 5, 20, textArea->textColor);
    }
}