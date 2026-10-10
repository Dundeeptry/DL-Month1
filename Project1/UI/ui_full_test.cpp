// Chơi thử TOÀN BỘ luồng UI trong một lần chạy: chào -> menu -> hướng dẫn -> ván chơi -> kết quả -> chơi lại.
// Logic game tối giản nằm ngay đây (chỉ để có dữ liệu thật cho UI, core thật do bạn khác làm):
//  - mở sẵn vài chữ đầu ván, nút Gợi ý mở thêm 1 chữ nhưng mất 1 mạng, kết quả hiện nghĩa tiếng Việt.
// Biên dịch: g++ -std=c++17 ui_full_test.cpp ui.cpp -o ui_full_test
// Tuỳ chọn: ui_full_test <từ> để ép từ bí mật (dùng khi kiểm tra tự động).
#include "ui.h"

#include <algorithm>
#include <ctime>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace {

struct Word { const char* en; const char* vi; };
const Word kWords[] = {
    {"banana", "quả chuối"}, {"orange", "quả cam"},   {"keyboard", "bàn phím"},
    {"window", "cửa sổ"},    {"computer", "máy tính"}, {"teacher", "giáo viên"},
    {"bridge", "cây cầu"},   {"garden", "khu vườn"},
};

std::mt19937 rng{static_cast<unsigned>(std::time(nullptr))};

// Các chữ khác nhau của từ, xáo ngẫu nhiên.
std::vector<char> shuffledLetters(const std::string& w) {
    std::set<char> s(w.begin(), w.end());
    std::vector<char> v(s.begin(), s.end());
    std::shuffle(v.begin(), v.end(), rng);
    return v;
}

// Trả true nếu người chơi thoát giữa chừng (bấm Thoát / gõ /q / EOF).
bool playRound(const Word& w, int difficulty) {
    const std::string secret = w.en;
    const int maxLives = 6;
    int lives = maxLives;
    std::set<char> guessed;
    static const char* const names[] = {"", "Dễ", "Thường", "Khó"};
    std::string msg = std::string("Độ khó: ") + names[difficulty];  // luật độ khó chưa có, chỉ hiện tên

    // Mở sẵn len/3 chữ (luôn chừa lại ít nhất 1 chữ chưa mở).
    std::vector<char> letters = shuffledLetters(secret);
    size_t pre = std::min(secret.size() / 3, letters.size() - 1);
    for (size_t i = 0; i < pre; ++i) guessed.insert(letters[i]);

    for (;;) {
        std::string masked;
        for (char c : secret) masked += guessed.count(c) ? c : '_';

        if (masked == secret) { showResult(true, secret, w.vi); return false; }
        if (lives == 0)       { showResult(false, secret, w.vi); return false; }

        showGameScreen(masked, guessed, lives, maxLives, msg);
        char c = askForLetter(guessed);
        if (c == '\0') return true;

        if (c == HINT_REQUEST) {   // gợi ý: mở 1 chữ chưa mở, trả giá 1 mạng
            if (lives <= 1) { msg = "Còn 1 mạng, không thể dùng gợi ý."; continue; }
            for (char h : shuffledLetters(secret))
                if (!guessed.count(h)) {
                    guessed.insert(h);
                    --lives;
                    msg = std::string("Gợi ý: chữ '") + h + "' có trong từ (mất 1 mạng).";
                    break;
                }
            continue;
        }

        guessed.insert(c);
        if (secret.find(c) != std::string::npos) {
            msg = std::string("Đúng rồi! Chữ '") + c + "' có trong từ.";
        } else {
            --lives;
            msg = std::string("Sai rồi! Không có chữ '") + c + "'.";
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    showWelcome();
    for (;;) {
        int choice = showMainMenu();
        if (choice == 3) break;
        if (choice == 2) { showHelp(); continue; }

        int diff = showDifficultyMenu();
        if (diff == 0) continue;                  // Quay lại: về menu chính
        do {
            Word w = kWords[rng() % std::size(kWords)];
            if (argc > 1) {                       // ép từ: tìm nghĩa nếu có trong danh sách
                w = {argv[1], ""};
                for (const Word& k : kWords) if (std::string(k.en) == argv[1]) w = k;
            }
            if (playRound(w, diff)) break;              // thoát giữa ván: quay lại menu
        } while (askPlayAgain());
    }
    return 0;
}
