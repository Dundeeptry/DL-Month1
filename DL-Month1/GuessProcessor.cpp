#include "GuessProcessor.h"
#include <cctype>
#include <algorithm>

Difficulty parseDifficulty(const std::string& difficultyInput) {
    std::string s = difficultyInput;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (s == "easy" || s == "de" || s == "1") {
        return Difficulty::EASY;
    }
    if (s == "hard" || s == "kho" || s == "3") {
        return Difficulty::HARD;
    }
    // mac dinh: medium
    return Difficulty::MEDIUM;
}

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
    GameState state;
    state.secretWord    = secretWord;
    state.maxLives      = getMaxLivesByDifficulty(diff);
    state.livesRemaining = state.maxLives;
    state.isWin  = false;
    state.isLose = false;
    state.displayWord = buildDisplayWord(secretWord, state.guessedLetters);
    return state;
}

GameState processGuess(GameState state, char inputChar) {
    char c = static_cast<char>(std::tolower(inputChar));

    // Neu game da ket thuc, khong xu ly them
    if (state.isWin || state.isLose) {
        return state;
    }

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

    // Kiem tra thang: khong con ky tu '_' nao trong displayWord
    if (state.displayWord.find('_') == std::string::npos) {
        state.isWin = true;
    }

    // Kiem tra thua: het mang
    if (state.livesRemaining <= 0) {
        state.isLose = true;
        state.livesRemaining = 0;
    }

    return state;
}
