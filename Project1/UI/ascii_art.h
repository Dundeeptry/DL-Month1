#pragma once
// Chỉ chứa DỮ LIỆU hình vẽ, không có logic. Muốn đổi hình thì sửa file này.

// Tiêu đề hiện ở đầu mọi màn hình.
inline constexpr const char* GAME_TITLE = "TRÒ CHƠI ĐOÁN TỪ - HANGMAN";

// HANGMAN_STAGES[i] là hình khi sai i lần (i = 0..6), mỗi hình gồm 7 dòng.
// Lưu ý: ký tự '\\' trong chuỗi C++ là MỘT dấu gạch chéo ngược.
inline constexpr const char* HANGMAN_STAGES[7][7] = {
    // 0 lần sai: chỉ có giá treo
    {"  +-----+", "  |     |", "  |",        "  |",         "  |",        "  |", "  ========="},
    // 1: đầu
    {"  +-----+", "  |     |", "  |     O",  "  |",         "  |",        "  |", "  ========="},
    // 2: thân
    {"  +-----+", "  |     |", "  |     O",  "  |     |",   "  |",        "  |", "  ========="},
    // 3: tay trái
    {"  +-----+", "  |     |", "  |     O",  "  |    /|",   "  |",        "  |", "  ========="},
    // 4: tay phải
    {"  +-----+", "  |     |", "  |     O",  "  |    /|\\", "  |",        "  |", "  ========="},
    // 5: chân trái
    {"  +-----+", "  |     |", "  |     O",  "  |    /|\\", "  |    /",   "  |", "  ========="},
    // 6: chân phải (thua)
    {"  +-----+", "  |     |", "  |     O",  "  |    /|\\", "  |    / \\", "  |", "  ========="},
};
