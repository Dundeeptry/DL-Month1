#include "GameLogic.hpp"

#include <algorithm>
#include <cctype>

namespace {

// Chuyển một ký tự sang chữ thường an toàn
// (ép về unsigned char để tránh hành vi không xác định của std::tolower)
char toLowerChar(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

// Kiểm tra ký tự có phải chữ cái hay không
bool isLetter(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

}  // namespace

void GameLogic::initGame(const WordItem& item, int maxLives) {
    currentWordItem = item;

    // Chuẩn hóa từ gốc thành chữ thường
    std::transform(currentWordItem.word.begin(), currentWordItem.word.end(),
                   currentWordItem.word.begin(), toLowerChar);

    // Tạo từ dạng ẩn: chữ cái -> '_', ký tự khác (khoảng trắng, dấu gạch nối...) giữ nguyên
    displayWord = currentWordItem.word;
    for (char& c : displayWord) {
        if (isLetter(c)) {
            c = '_';
        }
    }

    // Reset lịch sử đoán, thiết lập số mạng (tối thiểu 1) và trạng thái
    guessedLetters.clear();
    remainingLives = std::max(1, maxLives);
    state = GameState::PLAYING;

    // Trường hợp từ không chứa chữ cái nào thì thắng ngay
    if (displayWord.find('_') == std::string::npos) {
        state = GameState::WON;
    }
}

bool GameLogic::processGuess(char letter) {
    // Chỉ nhận đoán khi ván đang diễn ra
    if (state != GameState::PLAYING) {
        return false;
    }

    // Bỏ qua ký tự không phải chữ cái
    if (!isLetter(letter)) {
        return false;
    }

    // Chuyển về chữ thường để không phân biệt hoa/thường
    const char ch = toLowerChar(letter);

    // Bỏ qua nếu ký tự đã được đoán trước đó
    if (isLetterGuessed(ch)) {
        return false;
    }

    // Ghi nhận vào lịch sử
    guessedLetters.push_back(ch);

    const std::string& secret = currentWordItem.word;
    if (secret.find(ch) != std::string::npos) {
        // Đoán đúng: hiện tất cả vị trí xuất hiện của ký tự
        for (std::size_t i = 0; i < secret.size(); ++i) {
            if (secret[i] == ch) {
                displayWord[i] = ch;
            }
        }
        // Không còn '_' nghĩa là đã đoán hết từ -> thắng
        if (displayWord.find('_') == std::string::npos) {
            state = GameState::WON;
        }
    } else {
        // Đoán sai: trừ một mạng, hết mạng thì thua
        --remainingLives;
        if (remainingLives <= 0) {
            remainingLives = 0;
            state = GameState::LOST;
        }
    }

    return true;
}

bool GameLogic::isLetterGuessed(char letter) const {
    const char ch = toLowerChar(letter);
    return std::find(guessedLetters.begin(), guessedLetters.end(), ch) != guessedLetters.end();
}

std::string GameLogic::getDisplayWord() const { return displayWord; }

std::string GameLogic::getSecretWord() const { return currentWordItem.word; }

std::string GameLogic::getHint() const { return currentWordItem.hint; }

std::string GameLogic::getMeaning() const { return currentWordItem.meaning; }

int GameLogic::getRemainingLives() const { return remainingLives; }

const std::vector<char>& GameLogic::getGuessedLetters() const { return guessedLetters; }

GameState GameLogic::getGameState() const { return state; }