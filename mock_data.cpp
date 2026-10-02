#include "mock_data.h"

std::vector<std::string> loadMockWordList() {
    // Danh sach tu gia dinh, chi dung de test cuc bo (local testing).
    // Khong lien quan gi den file words.txt / branch feature/data-loader.
    return {
        "frog",
        "apple",
        "tiger",
        "orange",
        "guitar",
        "elephant",
        "mountain",
        "keyboard"
    };
}
