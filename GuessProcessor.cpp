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
        unsigned char rawC = static_cast<unsigned char>(c);

        if (!std::isalpha(rawC)) {
            // Khong phai chu cai (dau cach, '-', '\'', ...) -> tu hien
            // thi luon, khong bat nguoi choi phai doan ky tu nay.
            display += c;
        } else if (guessedLetters.count(c)) {
            // secretWord va guessedLetters da duoc chuan hoa thanh CHU HOA
            // tu luc initGame/processGuess, nen o day khong can toupper lai.
            display += c;
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

    // Chuan hoa secretWord thanh CHU HOA ngay tu dau (thay vi chu
    // thuong nhu truoc). Lam 1 lan duy nhat o day, de ve sau khoi
    // phai doi qua lai giua hoa/thuong moi khi so sanh hay hien thi -
    // UI/UX lay du lieu ve la xu ly duoc luon, khong can tu convert.
    std::string upperSecret = secretWord;
    std::transform(upperSecret.begin(), upperSecret.end(), upperSecret.begin(),
                   [](unsigned char ch) { return std::toupper(ch); });

    state.secretWord     = upperSecret;
    state.maxLives       = (maxLives > 0) ? maxLives : 1; // chan so mang <= 0
    state.livesRemaining = state.maxLives;
    state.isWin  = false;
    state.isLose = false;
    state.lastGuessInvalid = false;
    state.lastGuessWasRepeat = false;
    state.displayWord = buildDisplayWord(secretWord, state.guessedLetters);
    return state;
}

GameState processGuess(GameState state, char inputChar) {
    // Neu game da ket thuc, khong xu ly them
    if (state.isWin || state.isLose) {
        return state;
    }

    state.lastGuessInvalid = false;
    state.lastGuessWasRepeat = false;

    // ep ve unsigned char truoc khi dua vao isalpha/toupper de tranh
    // undefined behavior voi cac ky tu ngoai ASCII (vd nhap nham ky tu lạ)
    unsigned char raw = static_cast<unsigned char>(inputChar);

    // Xac nhan ky tu phai la chu cai (a-z hoac A-Z).
    // So, dau cach, ky tu dac biet (vd nhap du thua dau cach o cuoi
    // chuoi) deu bi loai o day, khong duoc tinh la 1 luot doan.
    // (Dau cach/'-' trong tu bi mat da duoc tu dong hien thi san trong
    // buildDisplayWord, nen nguoi choi khong can va khong the doan no.)
    if (!std::isalpha(raw)) {
        state.lastGuessInvalid = true;
        return state;
    }

    // Chuan hoa ve CHU HOA (thay vi chu thuong nhu truoc) de khop voi
    // secretWord da duoc chuan hoa hoa tu initGame.
    char c = static_cast<char>(std::toupper(raw));

    // Neu ky tu nay da doan roi (dung hoac sai) -> bo qua, khong tru mang.
    // Khac voi truong hop invalid o tren: day la chu cai HOP LE, chi la
    // da doan roi, nen bat flag rieng de UI phan biet duoc 2 truong hop.
    if (state.guessedLetters.count(c)) {
        state.lastGuessWasRepeat = true;
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