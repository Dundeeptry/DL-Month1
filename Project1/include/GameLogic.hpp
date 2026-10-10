#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include <string>
#include <vector>

#include "Types.hpp"

// Quản lý trạng thái và quy tắc của một ván Hangman, không phụ thuộc UI
class GameLogic {
public:
    // Khởi tạo ván mới với từ cho trước và số mạng tối đa
    void initGame(const WordItem& item, int maxLives = 6);

    // Xử lý một ký tự người chơi nhập vào (không phân biệt hoa/thường).
    // Trả về true nếu ký tự hợp lệ và chưa được đoán trước đó
    // (kể cả khi đoán sai); trả về false nếu bị bỏ qua.
    bool processGuess(char letter);

    // Kiểm tra ký tự (không phân biệt hoa/thường) đã được đoán chưa
    bool isLetterGuessed(char letter) const;

    // ----- Getters để UI hiển thị trạng thái -----
    std::string getDisplayWord() const;                 // Từ dạng ẩn, ví dụ "a__or__h_"
    std::string getSecretWord() const;                  // Từ gốc (chữ thường)
    std::string getHint() const;                        // Gợi ý tiếng Anh
    std::string getMeaning() const;                     // Nghĩa tiếng Việt
    int getRemainingLives() const;                      // Số mạng còn lại
    const std::vector<char>& getGuessedLetters() const; // Các ký tự đã đoán, theo thứ tự nhập
    GameState getGameState() const;                     // PLAYING, WON hoặc LOST

private:
    WordItem currentWordItem;           // Từ vựng của ván hiện tại (word đã chuẩn hóa chữ thường)
    std::string displayWord;            // Từ dạng ẩn: chữ chưa đoán là '_'
    std::vector<char> guessedLetters;   // Lịch sử các ký tự đã đoán (chữ thường)
    int remainingLives = 0;             // Số mạng còn lại
    GameState state = GameState::PLAYING;
};

#endif // GAMELOGIC_HPP