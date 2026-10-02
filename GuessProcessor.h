#ifndef GUESS_PROCESSOR_H
#define GUESS_PROCESSOR_H

#include <string>
#include <set>

// ================================================================
// GuessProcessor
// ------------------------------------------------------------
// Module xu ly logic doan tu (ham con) de main.cpp goi.
// Input: secretWord (tu bi mat, lay tu data loader), do kho,
//        va ky tu nguoi choi nhap.
// Output: GameState chua
//    - danh sach ky tu da doan
//    - chuoi hien thi dang "F _ _ K" (dung/sai)
//    - so mang con lai
//    - trang thai thang / thua
// ================================================================

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

struct GameState {
    std::string secretWord;          // tu bi mat (chu thuong, khong dau)
    std::string displayWord;         // vd: "F _ _ K" (co khoang trang giua)
    std::set<char> guessedLetters;   // tat ca ky tu da doan (dung + sai)
    std::set<char> wrongLetters;     // rieng cac ky tu sai (de UI hien thi)
    int livesRemaining;
    int maxLives;
    bool isWin;
    bool isLose;
    bool lastGuessInvalid; // true neu luot vua roi nhap ky tu khong hop le (bi bo qua)
};

// LUU Y: viec parse chuoi do kho (vd "easy"/"kho") tu UI thanh enum
// la viec cua UI hoac data loader, KHONG con nam trong module nay nua.
// Module nay chi can nhan thang enum Difficulty (hoac so mang cu the).

// Tra ve so mang tuong ung voi tung do kho
int getMaxLivesByDifficulty(Difficulty diff);

// Khoi tao trang thai game moi tu 1 secretWord + do kho (enum co san)
// (secretWord nen duoc lower-case hoa truoc khi truyen vao)
GameState initGame(const std::string& secretWord, Difficulty diff);

// Qua tai: khoi tao truc tiep bang so mang cu the, neu noi khac
// (UI/data loader) da tu tinh san so mang thay vi dung enum.
GameState initGame(const std::string& secretWord, int maxLives);

// Xu ly 1 luot doan cua nguoi choi.
// - inputChar: ky tu nguoi choi vua nhap (tu UI)
// Ham tu kiem tra ky tu co hop le khong (phai la chu cai a-z/A-Z).
// Neu khong hop le (vd dau cach, so, ky tu dac biet do nhap du thua),
// ham se BO QUA, khong tru mang, va bat co lastGuessInvalid = true
// de UI biet ma bao loi cho nguoi choi nhap lai.
// Neu ky tu da duoc doan roi truoc do, ham cung bo qua (khong tru mang).
GameState processGuess(GameState state, char inputChar);

// Dung de in ra Terminal / hoac tra chuoi cho UI hien thi
// Vd: secret = "frog" -> sau khi doan dung 'f' va 'k' (sai)
// -> "F _ _ _"  (cac ky tu chua doan la '_')
std::string buildDisplayWord(const std::string& secretWord,
                              const std::set<char>& guessedLetters);

#endif // GUESS_PROCESSOR_H
