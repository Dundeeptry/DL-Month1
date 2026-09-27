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

int main() {
    // ---- Issue #4: chon secretWord ngau nhien ----
    std::vector<std::string> wordList = {"frog", "apple", "tiger", "orange"};

    srand(static_cast<unsigned int>(time(0)));
    int idx = rand() % wordList.size();
    std::string secretWord = wordList[idx];

    // ---- Phan cua ban: lay do kho tu UI (gia lap nhap tu ban phim) ----
    std::string difficultyInput;
    std::cout << "Chon do kho (easy/medium/hard): ";
    std::cin >> difficultyInput;
    Difficulty diff = parseDifficulty(difficultyInput);

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
