// Chương trình thử riêng phần UI bằng dữ liệu giả (không cần core).
// Biên dịch: g++ -std=c++17 ui_test.cpp ui.cpp -o ui_test
#include "ui.h"

#include <iostream>
#include <set>
#include <string>

namespace {

struct Step {
    std::string masked;
    std::set<char> guessed;
    int lives;
    std::string msg;
};

// 7 trạng thái giả của từ "banana", đi từ 6 mạng xuống 0 mạng.
const Step kSteps[] = {
    {"______", {},                        6, ""},
    {"_a_a_a", {'a'},                     6, "Đúng rồi! Chữ 'a' có trong từ."},
    {"_a_a_a", {'a', 'z'},                5, "Sai rồi! Không có chữ 'z'."},
    {"ba_a_a", {'a', 'b', 'z'},           5, "Đúng rồi! Chữ 'b' có trong từ."},
    {"ba_a_a", {'a', 'b', 'x', 'z'},      4, "Sai rồi! Không có chữ 'x'."},
    {"ba_a_a", {'a', 'b', 'q', 'x', 'z'}, 3, "Sai rồi! Không có chữ 'q'."},
    {"ba_a_a", {'a', 'b', 'k', 'q', 'x', 'z'}, 0, "Sai rồi! Hết mạng."},
};

// Duyệt qua các màn chơi; ở mỗi màn thử gõ: nhiều ký tự, số, chữ đã đoán, /q.
void testGameScreens() {
    for (const Step& s : kSteps) {
        showGameScreen(s.masked, s.guessed, s.lives, 6, s.msg);
        char c = askForLetter(s.guessed);
        std::cout << "\033[21;1H(test) askForLetter trả về: "
                  << (c ? std::string(1, c) : std::string("'\\0' = thoát"))
                  << "\nEnter để sang màn tiếp... " << std::flush;
        std::string dummy;
        if (c == '\0' || !std::getline(std::cin, dummy)) break;
    }
}

}  // namespace

int main() {
    std::string line;
    for (;;) {
        std::cout << "\n=== TEST UI ===\n"
                     "1 showWelcome   2 showMainMenu   3 showHelp\n"
                     "4 màn chơi (7 trạng thái + nhập liệu)\n"
                     "5 showResult thắng   6 showResult thua   7 showResult + askPlayAgain\n"
                     "8 kiểm tra ký tự: ♥ ╔═╗ ă ư\n"
                     "0 thoát\nChọn: " << std::flush;
        if (!std::getline(std::cin, line) || line == "0") break;

        if      (line == "1") showWelcome();
        else if (line == "2") { int m = showMainMenu(); std::cout << "\033[14;1H(test) menu trả về: " << m << "\n"; }
        else if (line == "3") showHelp();
        else if (line == "4") testGameScreens();
        else if (line == "5") showResult(true, "banana");
        else if (line == "6") showResult(false, "banana");
        else if (line == "7") { showResult(true, "banana"); bool again = askPlayAgain(); std::cout << "\033[13;1H(test) askPlayAgain trả về: " << again << "\n"; }
        else if (line == "8") std::cout << "\033[31m♥\033[0m \033[90m♥\033[0m ╔═╗ ă ư\n";
    }
    return 0;
}