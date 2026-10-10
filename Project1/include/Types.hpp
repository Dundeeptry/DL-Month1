#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

// Trạng thái hiện tại của ván chơi
enum class GameState {
    PLAYING, // Đang chơi
    WON,     // Thắng (đoán đúng toàn bộ từ)
    LOST     // Thua (hết lượt đoán sai)
};

// Một mục từ vựng, tương ứng với một object trong data.json
struct WordItem {
    std::string word;       // Từ tiếng Anh (chữ thường)
    std::string meaning;    // Nghĩa tiếng Việt
    std::string hint;       // Gợi ý/định nghĩa bằng tiếng Anh
    std::string difficulty; // Độ khó: "easy", "medium" hoặc "hard"
};

#endif // TYPES_HPP