// demo_main.cpp
// ------------------------------------------------------------
// Vi du minh hoa main.cpp se goi module GuessProcessor nhu the nao.
// Phan Issue #4 (srand, chon tu ngau nhien) + phan cua ban
// (xu ly tung luot doan) duoc ghep lai o day de test thu.
// ------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "GuessProcessor.h"
#include "mock_data.h" 
#include <algorithm>

// Day la vi du ve viec UI/data loader tu xu ly chuoi do kho roi
// moi dua enum Difficulty vao GuessProcessor (theo gop y cua team:
// parseDifficulty khong con nam trong GuessProcessor nua).
Difficulty demoParseDifficulty(const std::string& input) {
    std::string s = input;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "easy" || s == "de")  return Difficulty::EASY;
    if (s == "hard" || s == "kho") return Difficulty::HARD;
    return Difficulty::MEDIUM;
}

int main() {
    // ---- Lay danh sach tu gia (mock) de test doc lap, khong can doi branch khac ----
    std::vector<std::string> wordList = loadMockWordList();

    if (wordList.empty()) {
        std::cerr << "Khong doc duoc danh sach tu, thoat chuong trinh.\n";
        return 1;
    }

    // ---- Issue #4: chon secretWord ngau nhien ----
    srand(static_cast<unsigned int>(time(0)));
    int idx = rand() % wordList.size();
    std::string secretWord = wordList[idx];

    // ---- Phan cua ban: lay do kho tu UI (gia lap nhap tu ban phim) ----
    std::string difficultyInput;
    std::cout << "Chon do kho (easy/medium/hard): ";
    std::cin >> difficultyInput;
    Difficulty diff = demoParseDifficulty(difficultyInput);

    GameState state = initGame(secretWord, diff);

    std::cout << "\nTu bi mat co " << secretWord.size() << " ky tu.\n";

    // ---- Vong lap choi ----
    while (!state.isWin && !state.isLose) {
        std::cout << "\nTu: " << state.displayWord << "\n";
        std::cout << "Mang con lai: " << state.livesRemaining
                   << "/" << state.maxLives << "\n";

        std::cout << "Nhap 1 ky tu de doan: ";
        char guess;
        std::cin >> guess;

        state = processGuess(state, guess);
    }

    if (state.isWin) {
        std::cout << "\nBan thang! Tu can tim la: " << state.secretWord << "\n";
    } else {
        std::cout << "\nBan thua! Tu can tim la: " << state.secretWord << "\n";
    }

    return 0;
}
