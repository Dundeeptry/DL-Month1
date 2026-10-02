#include "GuessProcessor.h"
#include <cctype>
#include <algorithm>

int getMaxLivesByDifficulty(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return 10;
        case Difficulty::MEDIUM: return 6;
        case Difficulty::HARD:   return 4;
    }
    return 6;
}

std::string buildDisplayWord(const std::string& secretWord,
                              const std::set<char>& guessedLetters) {
    std::string display;
    display.reserve(secretWord.size() * 2);

    for (size_t i = 0; i < secretWord.size(); ++i) {
        char c = secretWord[i];

        if (guessedLetters.count(c)) {
            display += static_cast<char>(std::toupper(c));
        } else {
            display += '_';
        }

        if (i + 1 < secretWord.size()) {
            display += ' '; // khoang trang cho UI thoang mat hon (theo issue #4)
        }
    }
    return display;
}

GameState initGame(const std::string& secretWord, Difficulty diff) {
    return initGame(secretWord, getMaxLivesByDifficulty(diff));
}

GameState initGame(const std::string& secretWord, int maxLives) {
    GameState state;
    state.secretWord     = secretWord;
    state.maxLives       = (maxLives > 0) ? maxLives : 1; // chan so mang <= 0
    state.livesRemaining = state.maxLives;
    state.isWin  = false;
    state.isLose = false;
    state.lastGuessInvalid = false;
    state.displayWord = buildDisplayWord(secretWord, state.guessedLetters);
    return state;
}

GameState processGuess(GameState state, char inputChar) {
    // Neu game da ket thuc, khong xu ly them
    if (state.isWin || state.isLose) {
        return state;
    }

    state.lastGuessInvalid = false;

    // ep ve unsigned char truoc khi dua vao isalpha/tolower de tranh
    // undefined behavior voi cac ky tu ngoai ASCII (vd nhap nham ky tu lạ)
    unsigned char raw = static_cast<unsigned char>(inputChar);

    // Xac nhan ky tu phai la chu cai (a-z hoac A-Z).
    // Dau cach, so, ky tu dac biet (vd nhap du thua dau cach o cuoi
    // chuoi) deu bi loai o day, khong duoc tinh la 1 luot doan.
    if (!std::isalpha(raw)) {
        state.lastGuessInvalid = true;
        return state;
    }

    char c = static_cast<char>(std::tolower(raw));

    // Neu ky tu nay da doan roi (dung hoac sai) -> bo qua, khong tru mang
    if (state.guessedLetters.count(c)) {
        return state;
    }

    state.guessedLetters.insert(c);

    bool isCorrect = (state.secretWord.find(c) != std::string::npos);

    if (!isCorrect) {
        state.wrongLetters.insert(c);
        state.livesRemaining--;
    }

    // Cap nhat chuoi hien thi
    state.displayWord = buildDisplayWord(state.secretWord, state.guessedLetters);

    // Kiem tra thang/thua - dung else-if de 2 co nay KHONG BAO GIO
    // cung true trong 1 lan goi (sua loi isWin/isLose cung = true).
    // Uu tien thang: neu vua doan dung va hoan thanh tu, tinh la thang
    // ngay ca khi mang da ve 0 tu truoc do.
    if (state.displayWord.find('_') == std::string::npos) {
        state.isWin = true;
    } else if (state.livesRemaining <= 0) {
        state.isLose = true;
        state.livesRemaining = 0;
    }

    return state;
}
