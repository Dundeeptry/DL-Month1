// Giao diện Terminal để chạy thử các module Core: Types, DataLoader, GameLogic
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

#include "../include/Types.hpp"
#include "../include/DataLoader.hpp"
#include "../include/GameLogic.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

constexpr int kMaxLives = 6;  // Số mạng tối đa mỗi ván

// Các đường dẫn thử lần lượt khi nạp dữ liệu
// (chạy từ thư mục gốc project hoặc từ thư mục build/)
const char* const kDataPaths[] = {"data/data.json", "../data/data.json"};

// ---------------------------------------------------------------------------
// Tiện ích chung
// ---------------------------------------------------------------------------

// Bật hiển thị UTF-8 (tiếng Việt, emoji) trên console Windows
void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

// Xóa màn hình, tự chọn lệnh theo hệ điều hành
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// Chuyển chuỗi sang chữ thường
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// Cắt khoảng trắng ở hai đầu chuỗi
std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

// In lời nhắc và đọc một dòng từ bàn phím.
// Trả về false nếu luồng nhập bị đóng (Ctrl+D / Ctrl+Z).
bool readLine(const std::string& prompt, std::string& out) {
    std::cout << prompt << std::flush;
    if (!std::getline(std::cin, out)) {
        return false;
    }
    out = trim(out);
    return true;
}

// Chèn khoảng trắng giữa các ký tự để dễ nhìn: "a__r" -> "a _ _ r"
std::string spaced(const std::string& s) {
    std::string result;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (i > 0) {
            result += ' ';
        }
        result += s[i];
    }
    return result;
}

// Tạo chuỗi hiển thị số mạng, ví dụ "❤️ ❤️ 🖤 (4/6)"
std::string livesBar(int remaining, int maxLives) {
    std::string bar;
    for (int i = 0; i < maxLives; ++i) {
        bar += (i < remaining) ? "❤️ " : "🖤 ";
    }
    bar += "(" + std::to_string(remaining) + "/" + std::to_string(maxLives) + ")";
    return bar;
}

// Nối danh sách ký tự đã đoán thành chuỗi "a, e, z"
std::string joinGuessed(const std::vector<char>& letters) {
    if (letters.empty()) {
        return "(chua co)";
    }
    std::string result;
    for (std::size_t i = 0; i < letters.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += letters[i];
    }
    return result;
}

// ---------------------------------------------------------------------------
// Nạp dữ liệu và chọn độ khó
// ---------------------------------------------------------------------------

// Thử nạp data.json từ các đường dẫn khả dĩ; trả về false nếu đều thất bại
bool loadData(DataLoader& loader) {
    for (const char* path : kDataPaths) {
        if (loader.loadFromFile(path)) {
            return true;
        }
    }
    return false;
}

// Cho người chơi chọn độ khó. Trả về false nếu luồng nhập bị đóng.
bool chooseDifficulty(const DataLoader& loader, std::string& difficulty) {
    (void)loader;
    std::string notice;
    while (true) {
        clearScreen();
        std::cout << "=====================================\n"
                  << "   HANGMAN - HOC TU VUNG TIENG ANH   \n"
                  << "=====================================\n\n"
                  << "Chon do kho:\n"
                  << "  1. easy   (A1)\n"
                  << "  2. medium (A2)\n"
                  << "  3. hard   (B1)\n\n";
        if (!notice.empty()) {
            std::cout << notice << "\n\n";
        }

        std::string input;
        if (!readLine("Nhap lua chon (1/2/3 hoac easy/medium/hard): ", input)) {
            return false;
        }
        input = toLower(input);

        if (input == "1" || input == "easy") {
            difficulty = "easy";
        } else if (input == "2" || input == "medium") {
            difficulty = "medium";
        } else if (input == "3" || input == "hard") {
            difficulty = "hard";
        } else {
            notice = "Lua chon khong hop le, vui long nhap lai.";
            continue;
        }
        return true;
    }
}

// ---------------------------------------------------------------------------
// Hiển thị và chơi một ván
// ---------------------------------------------------------------------------

// In toàn bộ thông tin ván chơi hiện tại
void printGameScreen(const GameLogic& game, const std::string& difficulty) {
    std::cout << "=====================================\n"
              << "          HANGMAN - DOAN CHU         \n"
              << "=====================================\n\n"
              << "Do kho    : " << difficulty << '\n'
              << "So mang   : " << livesBar(game.getRemainingLives(), kMaxLives) << '\n'
              << "Da doan   : " << joinGuessed(game.getGuessedLetters()) << '\n'
              << "Goi y     : " << game.getHint() << "\n\n"
              << "      " << spaced(game.getDisplayWord()) << "\n\n";
}

// In kết quả khi ván kết thúc
void printResult(const GameLogic& game) {
    if (game.getGameState() == GameState::WON) {
        std::cout << "🎉🎉🎉  CHUC MUNG! BAN DA THANG!  🎉🎉🎉\n\n";
    } else {
        std::cout << "💀💀💀  RAT TIEC, BAN DA THUA!  💀💀💀\n\n";
    }
    std::cout << "Tu dung   : " << game.getSecretWord() << '\n'
              << "Nghia     : " << game.getMeaning() << "\n\n";
}

// Chơi một ván hoàn chỉnh. Trả về false nếu luồng nhập bị đóng giữa chừng.
bool playRound(DataLoader& loader, const std::string& difficulty) {
    const WordItem item = loader.getRandomWordByDifficulty(difficulty);
    if (item.word.empty()) {
        std::cout << "Khong co tu nao thuoc do kho \"" << difficulty << "\".\n";
        return true;
    }

    GameLogic game;
    game.initGame(item, kMaxLives);

    std::string notice;  // Thông báo hiển thị ở lượt kế tiếp
    while (game.getGameState() == GameState::PLAYING) {
        clearScreen();
        printGameScreen(game, difficulty);
        if (!notice.empty()) {
            std::cout << notice << "\n\n";
            notice.clear();
        }

        std::string input;
        if (!readLine("Nhap 1 chu cai: ", input)) {
            return false;
        }

        // Chỉ chấp nhận đúng một ký tự
        if (input.size() != 1) {
            notice = "Vui long nhap dung 1 ky tu.";
            continue;
        }

        const int livesBefore = game.getRemainingLives();
        if (!game.processGuess(input[0])) {
            notice = "Ky tu khong hop le hoac da doan roi, hay thu chu khac.";
        } else if (game.getRemainingLives() < livesBefore) {
            notice = "Sai roi! Ban bi tru 1 mang.";
        } else {
            notice = "Chinh xac!";
        }
    }

    // Ván kết thúc: hiển thị lần cuối cùng kèm kết quả
    clearScreen();
    printGameScreen(game, difficulty);
    printResult(game);
    return true;
}

// Hỏi người chơi có muốn chơi tiếp không (y/n)
bool askPlayAgain() {
    while (true) {
        std::string input;
        if (!readLine("Choi van moi? (y/n): ", input)) {
            return false;
        }
        input = toLower(input);
        if (input == "y" || input == "yes") {
            return true;
        }
        if (input == "n" || input == "no") {
            return false;
        }
        std::cout << "Vui long nhap y hoac n.\n";
    }
}

}  // namespace

int main() {
    setupConsole();

    // 1. Nạp dữ liệu từ vựng và kiểm tra lỗi
    DataLoader loader;
    if (!loadData(loader) || loader.isEmpty()) {
        std::cerr << "Loi: khong the nap du lieu tu data/data.json.\n"
                  << "Hay chac chan file ton tai va ban chay chuong trinh tu thu muc goc cua project.\n";
        return 1;
    }

    // 2. Vòng lặp nhiều ván chơi
    do {
        std::string difficulty;
        if (!chooseDifficulty(loader, difficulty)) {
            break;  // Luồng nhập bị đóng
        }
        if (!playRound(loader, difficulty)) {
            break;
        }
    } while (askPlayAgain());

    std::cout << "\nCam on ban da choi. Hen gap lai!\n";
    return 0;
}