#pragma once
#include <set>
#include <string>

// UI chỉ HIỂN THỊ dữ liệu được đưa vào và TRẢ VỀ input của người chơi.
// Không có logic game ở đây (không tự xét thắng/thua, không chọn từ).
// Quy ước với core: từ bí mật là a-z không dấu; maskedWord dùng '_' cho ô chưa mở.

// Mã trả về của askForLetter khi người chơi xin gợi ý ('?' không bao giờ là chữ cái hợp lệ).
inline constexpr char HINT_REQUEST = '?';

// Màn hình chào, chờ người chơi nhấn Enter.
void showWelcome();

// Hiện menu, trả về 1 = Chơi, 2 = Hướng dẫn, 3 = Thoát.
// Nhập sai thì hỏi lại; gặp EOF thì trả 3 để chương trình thoát êm.
int showMainMenu();

// Chọn độ khó, trả về 1 = Dễ (xanh lá), 2 = Thường (vàng), 3 = Khó (đỏ);
// 0 = quay lại menu chính (bấm nút Quay lại, gõ 0, hoặc EOF).
// Chỉ là lựa chọn của người chơi, luật chia độ khó do core quyết định.
int showDifficultyMenu();

// Màn hướng dẫn chơi, chờ Enter để quay lại.
void showHelp();

// Vẽ lại toàn bộ màn chơi (xoá màn hình trước khi vẽ).
//  maskedWord : vd "_a__"
//  guessed    : mọi chữ đã đoán (đúng lẫn sai); UI tự phân màu bằng cách
//               xem chữ đó có trong maskedWord hay không
//  livesLeft / maxLives : chọn hình hangman và vẽ thanh ♥ ♡
//  message    : thông báo của lượt trước, "" nếu không có
void showGameScreen(const std::string& maskedWord,
                    const std::set<char>& guessed,
                    int livesLeft, int maxLives,
                    const std::string& message);

// Đọc một chữ cái, tự kiểm tra cú pháp và hỏi lại khi sai
// (nhiều ký tự / không phải chữ cái / đã đoán rồi).
// Trả về chữ THƯỜNG, '\0' nếu người chơi muốn thoát (nút Thoát, gõ /q hoặc EOF),
// hoặc HINT_REQUEST nếu người chơi xin gợi ý (nút Gợi ý hoặc gõ /h). Core tự quyết định gợi ý là gì.
// [Đổi so với hợp đồng cũ] thêm tham số guessed để báo lỗi "đã đoán rồi".
char askForLetter(const std::set<char>& guessed);

// Màn kết quả thắng/thua, hiện từ bí mật và nghĩa tiếng Việt (meaning = "" thì bỏ qua dòng nghĩa).
void showResult(bool won, const std::string& secret, const std::string& meaning = "");

// Hỏi chơi lại không (y/n). Sai thì hỏi lại; EOF thì trả false.
bool askPlayAgain();
