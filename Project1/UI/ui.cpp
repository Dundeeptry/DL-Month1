#include "ui.h"
#include "ascii_art.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif
#else
#include <csignal>
#include <termios.h>
#include <unistd.h>
#endif

namespace {

// ---------- Bảng màu (đúng 4 màu) ----------
const char* RESET = "\033[0m";
const char* MAIN  = "\033[36m";  // chính: cyan
const char* GOOD  = "\033[32m";  // đúng: xanh lá
const char* BAD   = "\033[31m";  // sai: đỏ
const char* DIM   = "\033[90m";  // mờ: xám
const char* MID   = "\033[33m";  // vàng: chỉ dùng cho độ khó "Thường"
// Highlight khi rê chuột: đảo màu nền/chữ của chính màu đó.
const char* MAIN_HI = "\033[7;36m";
const char* GOOD_HI = "\033[7;32m";
const char* BAD_HI  = "\033[7;31m";
const char* MID_HI  = "\033[7;33m";

// ---------- Kích thước khung: tổng 80 cột ----------
constexpr int W     = 78;      // bề rộng bên trong khung (80 - 2 viền)
constexpr int LEFT  = 38;      // cột trái (hình hangman)
constexpr int RIGHT = W - LEFT - 1;  // cột phải (trừ 1 cho vạch giữa)
constexpr int ROWS  = 11;      // số dòng thân màn chơi

// ---------- Toạ độ để bắt chuột (cột/dòng tính từ 1, màn hình luôn vẽ từ góc trên-trái) ----------
constexpr int BODY_ROW   = 4;                // dòng đầu tiên sau tiêu đề
constexpr int ALPHA_ROW  = BODY_ROW + 9;     // dòng chứa chữ A-M (N-Z nằm dòng kế)
constexpr int ALPHA_COL  = LEFT + 5;         // viền(1) + cột trái + vạch giữa(1) + lề(1)... = cột của chữ A
constexpr int MSG_ROW    = 16;               // dòng thông báo
constexpr int INPUT_ROW  = 17;               // dòng nhập
constexpr int QUIT_ROW   = 18;               // dòng nút thoát
constexpr int MENU_ROW0  = BODY_ROW + 1;     // mục "1. Chơi" của menu
constexpr int RESULT_BTN_ROW = BODY_ROW + 6; // dòng nút của màn kết quả
constexpr int PARK_ROW   = 12;               // nơi đỗ con trỏ ở các màn không có ô nhập

const std::string QUIT_BTN = "[ Thoát game ]";
constexpr int QUIT_COL = 4;                  // viền(1) + lề(2) + 1
const std::string BTN_ENTER = "[ Enter ]";
const std::string BTN_HINT  = "[ Gợi ý ]";   // nằm ngay dưới nút Enter, cùng cột ENTER_COL
constexpr int ENTER_COL = 66;                // cột bắt đầu nút Enter (trên dòng nhập)
const std::string BTN_YES  = "[ Chơi lại (y) ]";
const std::string BTN_NO   = "[ Thoát (n) ]";
constexpr int BTN_GAP = 4;

const char* const MOUSE_ON  = "\033[?1000h\033[?1003h\033[?1006h";
const char* const MOUSE_OFF = "\033[?1000l\033[?1003l\033[?1006l";

// Trạng thái để dọn dẹp khi bị Ctrl+C / đóng cửa sổ (destructor không chạy trong trường hợp này,
// nếu không dọn thì terminal cứ gửi mã chuột vào shell sau khi thoát).
#ifdef _WIN32
HANDLE g_hin = nullptr;
DWORD g_oldMode = 0;
volatile bool g_rawActive = false;

BOOL WINAPI onCtrl(DWORD) {
    if (g_rawActive) {
        DWORD w = 0;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), MOUSE_OFF,
                  static_cast<DWORD>(std::strlen(MOUSE_OFF)), &w, nullptr);
        SetConsoleMode(g_hin, g_oldMode);
    }
    return FALSE;  // để Windows tiếp tục kết thúc chương trình như thường
}
#else
termios g_oldTerm{};
volatile sig_atomic_t g_rawActive = 0;

void onSignal(int sig) {
    if (g_rawActive) {
        ssize_t r = write(1, MOUSE_OFF, std::strlen(MOUSE_OFF));
        (void)r;
        tcsetattr(0, TCSANOW, &g_oldTerm);
    }
    signal(sig, SIG_DFL);
    raise(sig);
}
#endif

// Khởi động một lần: bật ANSI + UTF-8 (Windows), cài bộ dọn dẹp khi bị ngắt đột ngột.
// Windows: trả lại code page cũ khi thoát để không làm đổi console của người dùng.
struct ConsoleInit {
#ifdef _WIN32
    UINT oldIn_ = GetConsoleCP(), oldOut_ = GetConsoleOutputCP();
#endif
    ConsoleInit() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (GetConsoleMode(h, &mode))
            SetConsoleMode(h, mode | 0x0004);  // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleCtrlHandler(onCtrl, TRUE);
#else
        signal(SIGINT, onSignal);
        signal(SIGTERM, onSignal);
        signal(SIGHUP, onSignal);
#endif
    }
#ifdef _WIN32
    ~ConsoleInit() {
        if (oldIn_) SetConsoleCP(oldIn_);
        if (oldOut_) SetConsoleOutputCP(oldOut_);
    }
#endif
};
const ConsoleInit kInit;

// ======================== Đọc phím + chuột ========================

// Trong phạm vi một lần đọc: tắt chế độ gõ-từng-dòng của terminal để nhận từng phím
// và bật báo cáo chuột (SGR). Huỷ đối tượng là trả terminal về như cũ.
class RawMode {
public:
    RawMode() {
        std::cout << MOUSE_ON << std::flush;  // bật chuột
#ifdef _WIN32
        hin_ = GetStdHandle(STD_INPUT_HANDLE);
        ok_ = GetConsoleMode(hin_, &old_) != 0;
        // Tắt QuickEdit, không thì Windows nuốt cú click để "bôi đen chữ".
        if (ok_)
            SetConsoleMode(hin_, (old_ | ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_MOUSE_INPUT |
                                  ENABLE_EXTENDED_FLAGS) &
                                     ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE));
        g_hin = hin_;
        g_oldMode = old_;
        g_rawActive = ok_;
#else
        ok_ = tcgetattr(0, &old_) == 0;
        if (ok_) {
            termios t = old_;
            t.c_lflag &= ~(ICANON | ECHO);
            t.c_cc[VMIN] = 1;
            t.c_cc[VTIME] = 0;
            tcsetattr(0, TCSANOW, &t);
            g_oldTerm = old_;
            g_rawActive = 1;
        }
#endif
    }
    ~RawMode() {
        std::cout << MOUSE_OFF << std::flush;  // tắt chuột
        // Khôi phục chế độ cũ và XOÁ các byte chuột còn tồn trong bộ đệm, nếu không
        // std::getline/cin ở lần đọc sau sẽ nhận phải rác kiểu "ESC[<35;12;7M".
#ifdef _WIN32
        if (ok_) { SetConsoleMode(hin_, old_); FlushConsoleInputBuffer(hin_); }
        g_rawActive = false;
#else
        if (ok_) tcsetattr(0, TCSAFLUSH, &old_);  // TCSAFLUSH: bỏ input chưa đọc
        g_rawActive = 0;
#endif
    }
    RawMode(const RawMode&) = delete;
    RawMode& operator=(const RawMode&) = delete;

private:
    bool ok_ = false;
#ifdef _WIN32
    HANDLE hin_ = nullptr;
    DWORD old_ = 0;
#else
    termios old_{};
#endif
};

// Đọc 1 byte thô từ bàn phím. -1 nghĩa là EOF/lỗi.
int readByte() {
    std::cout.flush();  // đọc thô không tự xả cout như cin
#ifdef _WIN32
    unsigned char c;
    DWORD n = 0;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &c, 1, &n, nullptr) || n != 1) return -1;
    return c;
#else
    unsigned char c;
    return read(0, &c, 1) == 1 ? c : -1;
#endif
}

struct Event {
    enum Kind { Key, Click, Move, Eof } kind = Eof;
    std::string key;  // Key: "\n" = Enter, "\b" = Backspace, còn lại là ký tự gõ
    int x = 0, y = 0; // Click/Move: cột/dòng (tính từ 1)
};

// Đợi sự kiện kế tiếp: một phím, một cú click chuột trái, hoặc EOF.
// shortcut: phím Esc đơn lẻ sẽ chờ thêm 1 byte mới trả về, đủ dùng vì game không dùng Esc.
Event readEvent() {
    for (;;) {
        int c = readByte();
        Event e;
        if (c < 0 || c == 4 || c == 26) return e;  // EOF, Ctrl+D, Ctrl+Z

        if (c == 27) {                             // ESC: chuỗi phím đặc biệt hoặc chuột
            if (readByte() != '[') continue;
            int d = readByte();
            if (d == '<') {                        // chuột SGR: ESC [ < nút ; x ; y M|m
                int v[3] = {0, 0, 0}, i = 0, ch;
                while ((ch = readByte()) >= 0 && ch != 'M' && ch != 'm') {
                    if (ch == ';') { if (i < 2) ++i; }
                    else if (std::isdigit(ch)) v[i] = v[i] * 10 + (ch - '0');
                }
                if (ch < 0) return e;
                if (ch == 'M' && v[0] == 0) {      // 'M' = nhấn xuống, nút 0 = chuột trái
                    e.kind = Event::Click; e.x = v[1]; e.y = v[2];
                    return e;
                }
                if (ch == 'M' && (v[0] & 32) && !(v[0] & 64)) {  // bit 32 = di chuyển chuột
                    e.kind = Event::Move; e.x = v[1]; e.y = v[2];
                    return e;
                }
                continue;                          // nhả chuột, cuộn, chuột phải: bỏ qua
            }
            while (d >= 0 && !(d >= 0x40 && d <= 0x7E)) d = readByte();  // mũi tên...: bỏ qua
            continue;
        }

        e.kind = Event::Key;
        if (c == '\r' || c == '\n') e.key = "\n";
        else if (c == 127 || c == 8) e.key = "\b";
        else {
            e.key = std::string(1, static_cast<char>(c));
            int extra = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : c >= 0xC0 ? 1 : 0;  // ký tự UTF-8 nhiều byte
            while (extra-- > 0) { int b = readByte(); if (b < 0) break; e.key += static_cast<char>(b); }
        }
        return e;
    }
}

// ======================== Vẽ ========================

// Đếm số KÝ TỰ hiển thị của chuỗi UTF-8 (không đếm byte).
// Byte dạng 10xxxxxx là byte nối tiếp của ký tự nhiều byte nên bỏ qua.
int vlen(const std::string& s) {
    int n = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80) ++n;
    return n;
}

// Một đoạn chữ có màu: lưu cả chuỗi (kèm mã ANSI) lẫn độ rộng hiển thị n,
// vì mã màu không chiếm chỗ trên màn hình nhưng làm .size() sai.
struct Txt {
    std::string s;
    int n = 0;
    Txt& add(const std::string& t, const char* color = nullptr) {
        if (color) s += color;
        s += t;
        if (color) s += RESET;
        n += vlen(t);
        return *this;
    }
};

std::string rep(const char* s, int n) {
    std::string r;
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

std::string pad(const Txt& t, int width) {
    return t.s + std::string(std::max(0, width - t.n), ' ');
}

void clearScreen() { std::cout << "\033[2J\033[H"; }

void gotoRC(int row, int col) { std::cout << "\033[" << row << ";" << col << "H"; }

// Vẽ đường ngang. mid != nullptr thì chia làm 2 cột (╦ / ╩).
void edge(const char* l, const char* r, const char* mid = nullptr) {
    std::cout << MAIN << l;
    if (mid) std::cout << rep("═", LEFT) << mid << rep("═", RIGHT);
    else     std::cout << rep("═", W);
    std::cout << r << RESET << "\n";
}

void boxLine(const Txt& t) {
    std::cout << MAIN << "║" << RESET << pad(t, W) << MAIN << "║" << RESET << "\n";
}

void splitLine(const Txt& l, const Txt& r) {
    std::cout << MAIN << "║" << RESET << pad(l, LEFT)
              << MAIN << "║" << RESET << pad(r, RIGHT)
              << MAIN << "║" << RESET << "\n";
}

Txt center(const std::string& t, const char* color = nullptr) {
    Txt r;
    r.add(std::string(std::max(0, (W - vlen(t)) / 2), ' '));
    r.add(t, color);
    return r;
}

Txt indent(const std::string& t, const char* color = nullptr) {
    Txt r{"  ", 2};
    r.add(t, color);
    return r;
}

// Khung cho các màn không phải màn chơi: tiêu đề + các dòng nội dung.
void drawSimple(const std::vector<Txt>& lines) {
    clearScreen();
    edge("╔", "╗");
    boxLine(center(GAME_TITLE, MAIN));
    edge("╠", "╣");
    for (const Txt& t : lines) boxLine(t);
    edge("╚", "╝");
}

std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t") - b + 1);
}

// Dòng nhập của màn chơi (kèm chữ người chơi đang gõ dở).
Txt inputRow(const std::string& buf = "") {
    Txt t = indent("Nhập một chữ cái: ", MAIN);
    t.add(buf);
    return t;
}

void placeCursor(const std::string& buf) {
    gotoRC(INPUT_ROW, 2 + inputRow(buf).n);  // viền(1) + độ rộng chữ + 1, trừ đi 1 cho n tính cả lề = 2 + n
    std::cout << std::flush;
}

// Vẽ lại riêng dòng nhập (khi gõ/xoá), chỉ tới trước nút Enter nên không xoá nút.
void redrawInput(const std::string& buf) {
    gotoRC(INPUT_ROW, 2);
    std::cout << pad(inputRow(buf), ENTER_COL - 2);
    placeCursor(buf);
}

bool onQuitBtn(int x, int y) {
    return y == QUIT_ROW && x >= QUIT_COL && x < QUIT_COL + vlen(QUIT_BTN);
}

bool onEnterBtn(int x, int y) {
    return y == INPUT_ROW && x >= ENTER_COL && x < ENTER_COL + vlen(BTN_ENTER);
}

bool onHintBtn(int x, int y) {
    return y == QUIT_ROW && x >= ENTER_COL && x < ENTER_COL + vlen(BTN_HINT);
}

void drawHint(bool hot) {
    gotoRC(QUIT_ROW, ENTER_COL);
    std::cout << (hot ? GOOD_HI : GOOD) << BTN_HINT << RESET;
}

void drawEnter(bool hot) {
    gotoRC(INPUT_ROW, ENTER_COL);
    std::cout << (hot ? MAIN_HI : MAIN) << BTN_ENTER << RESET;
}

Txt messageRow(const std::string& msg, const char* color = nullptr) {
    Txt t = indent("Thông báo: ", MAIN);
    t.add(msg, color);
    return t;
}

// Chữ c có xuất hiện trong từ đã che không (tức là đoán đúng)?
bool isHit(char c, const std::string& masked) {
    for (char m : masked)
        if (std::tolower(static_cast<unsigned char>(m)) == c) return true;
    return false;
}

// Click vào ô chữ nào của bảng A-Z? Trả 0 nếu không trúng chữ nào.
// Ô tính cả dấu cách phía sau chữ cho dễ bấm.
char letterAt(int x, int y) {
    for (char c = 'a'; c <= 'z'; ++c) {
        int k = c <= 'm' ? c - 'a' : c - 'n';
        int row = ALPHA_ROW + (c <= 'm' ? 0 : 1);
        int col = ALPHA_COL + 2 * k;
        if (y == row && (x == col || x == col + 1)) return c;
    }
    return 0;
}

// Cột bắt đầu của nút "Chơi lại" trong màn kết quả (hai nút căn giữa).
int resultBtnCol() {
    return 2 + (W - (vlen(BTN_YES) + BTN_GAP + vlen(BTN_NO))) / 2;
}

// ---------- Highlight khi rê chuột: chỉ vẽ lại đúng phần tử đổi trạng thái ----------

// Một mục menu: chữ, màu thường, màu khi rê chuột, dòng và cột màn hình (col chỉ dùng khi
// centered), phím tắt, giá trị trả về.
struct MenuItem { const char* text; const char* color; const char* hi; int row; int col; char key; int value; };

// Cột để căn giữa một đoạn rộng w trong khung.
int centerCol(int w) { return 2 + std::max(0, (W - w) / 2); }

// Nội dung dòng màn hình `row` của menu; hot = chỉ số mục đang sáng (-1 = không có).
// centered = false: mục thụt lề, sáng thì tô cả thanh ngang.
// centered = true : các nút đặt đúng cột m.col (nhiều nút trên một dòng được), chỉ tô đúng nút.
Txt menuRow(const std::vector<MenuItem>& items, int row, int hot, bool centered) {
    Txt t;
    int pos = 2;
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        const MenuItem& m = items[i];
        if (m.row != row) continue;
        bool h = i == hot;
        if (centered) {
            t.add(std::string(std::max(0, m.col - pos), ' '));
            t.add(m.text, h ? m.hi : m.color);
            pos = m.col + vlen(m.text);
        } else if (!h) {
            return indent(m.text, m.color);
        } else {
            std::string full = std::string("  ") + m.text;
            full += std::string(std::max(0, W - vlen(full)), ' ');
            return Txt{}.add(full, m.hi);
        }
    }
    return t;
}

void drawMenuItem(const std::vector<MenuItem>& items, int i, bool hot, bool centered) {
    gotoRC(items[i].row, 1);
    boxLine(menuRow(items, items[i].row, hot ? i : -1, centered));
    gotoRC(PARK_ROW, 1);
    std::cout << std::flush;
}

// Mục nào nằm dưới con trỏ chuột (x, y)? -1 nếu không có.
int itemAt(const std::vector<MenuItem>& items, bool centered, int x, int y) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].row != y) continue;
        bool in = centered ? (x >= items[i].col && x < items[i].col + vlen(items[i].text))
                           : (x >= 2 && x <= W + 1);
        if (in) return static_cast<int>(i);
    }
    return -1;
}

// Vẽ lại một chữ trong bảng A-Z (kèm dấu cách sau chữ, khớp với vùng bấm của letterAt).
void drawLetter(char c, bool used, bool hot) {
    int k = c <= 'm' ? c - 'a' : c - 'n';
    gotoRC(ALPHA_ROW + (c <= 'm' ? 0 : 1), ALPHA_COL + 2 * k);
    std::string up(1, static_cast<char>(std::toupper(c)));
    if (hot) std::cout << MAIN_HI << up << " " << RESET;
    else     std::cout << (used ? DIM : MAIN) << up << RESET << " ";
}

void drawQuit(bool hot) {
    gotoRC(QUIT_ROW, QUIT_COL);
    std::cout << (hot ? BAD_HI : BAD) << QUIT_BTN << RESET;
}

// hot: 0 = không nút nào, 1 = "Chơi lại", 2 = "Thoát"
void drawResultBtns(int hot) {
    int c1 = resultBtnCol();
    gotoRC(RESULT_BTN_ROW, c1);
    std::cout << (hot == 1 ? GOOD_HI : GOOD) << BTN_YES << RESET;
    gotoRC(RESULT_BTN_ROW, c1 + vlen(BTN_YES) + BTN_GAP);
    std::cout << (hot == 2 ? BAD_HI : BAD) << BTN_NO << RESET;
    gotoRC(PARK_ROW, 1);
    std::cout << std::flush;
}

// Chờ phím hoặc click (bỏ qua di chuột). Dùng cho các màn "bấm để tiếp tục".
void waitKeyOrClick() {
    Event e;
    do e = readEvent(); while (e.kind == Event::Move);
}

// Menu chọn bằng chuột hoặc phím tắt. items sắp theo dòng tăng dần. EOF: trả eofValue.
int runMenu(const std::vector<MenuItem>& items, bool centered, const char* header,
            const char* hint, int eofValue) {
    RawMode raw;
    std::string err;
    const int last = items.back().row - BODY_ROW;
    for (;;) {
        std::vector<Txt> lines(last + 4);
        lines[0] = centered ? center(header, MAIN) : indent(header, MAIN);
        for (const MenuItem& m : items) lines[m.row - BODY_ROW] = menuRow(items, m.row, -1, centered);
        lines[last + 2] = center(hint, DIM);
        lines[last + 3] = indent(err, BAD);
        drawSimple(lines);

        int hot = -1;  // mục đang được rê chuột (-1 = không có)
        for (;;) {
            Event e = readEvent();
            if (e.kind == Event::Eof) return eofValue;
            if (e.kind == Event::Move || e.kind == Event::Click) {
                int item = itemAt(items, centered, e.x, e.y);
                if (e.kind == Event::Click) {
                    if (item >= 0) return items[item].value;
                    continue;                    // click ra ngoài các mục: bỏ qua
                }
                if (item != hot) {
                    if (hot >= 0) drawMenuItem(items, hot, false, centered);
                    if (item >= 0) drawMenuItem(items, item, true, centered);
                    hot = item;
                }
                continue;
            }
            for (const MenuItem& m : items)
                if (e.key.size() == 1 && e.key[0] == m.key) return m.value;
            if (e.key == "\n" || e.key == "\b") continue;
            err = "Lựa chọn không hợp lệ, hãy thử lại.";
            break;  // vẽ lại màn có dòng lỗi
        }
    }
}

}  // namespace

// ======================== Hàm công khai ========================

void showWelcome() {
    drawSimple({
        Txt{},
        center("CHÀO MỪNG BẠN ĐẾN VỚI HANGMAN", GOOD),
        center("Đoán đúng từ bí mật trước khi hết mạng!"),
        Txt{},
        center("Bấm chuột hoặc nhấn phím bất kỳ để bắt đầu", DIM),
    });
    RawMode raw;
    waitKeyOrClick();  // phím nào / click nào / EOF cũng đi tiếp
}

int showMainMenu() {
    static const std::vector<MenuItem> items = {
        {"[ 1 ]  Chơi",       MAIN, MAIN_HI, MENU_ROW0,     0, '1', 1},
        {"[ 2 ]  Hướng dẫn",  MAIN, MAIN_HI, MENU_ROW0 + 1, 0, '2', 2},
        {"[ 3 ]  Thoát",      MAIN, MAIN_HI, MENU_ROW0 + 2, 0, '3', 3},
    };
    return runMenu(items, false, "", "Rê chuột vào mục để chọn, bấm để xác nhận, hoặc gõ số 1-3", 3);
}

// Ba độ khó dàn hàng ngang (căn giữa cả cụm), nút Quay lại ở dòng dưới.
int showDifficultyMenu() {
    static const std::vector<MenuItem> items = [] {
        const char *t0 = "[ Dễ ]", *t1 = "[ Thường ]", *t2 = "[ Khó ]", *tb = "[ Quay lại ]";
        const int gap = 4;
        int c0 = centerCol(vlen(t0) + vlen(t1) + vlen(t2) + 2 * gap);
        int c1 = c0 + vlen(t0) + gap, c2 = c1 + vlen(t1) + gap;
        return std::vector<MenuItem>{
            {t0, GOOD, GOOD_HI, BODY_ROW + 2, c0, '1', 1},
            {t1, MID,  MID_HI,  BODY_ROW + 2, c1, '2', 2},
            {t2, BAD,  BAD_HI,  BODY_ROW + 2, c2, '3', 3},
            {tb, MAIN, MAIN_HI, BODY_ROW + 4, centerCol(vlen(tb)), '0', 0},
        };
    }();
    return runMenu(items, true, "CHỌN ĐỘ KHÓ", "Bấm vào nút, hoặc gõ số 1-3 (gõ 0 để quay lại)", 0);
}

void showHelp() {
    drawSimple({
        Txt{},
        indent("Cách chơi:", MAIN),
        indent("- Máy chọn một từ bí mật, mỗi chữ cái hiện là một dấu _."),
        indent("- Mỗi lượt bạn đoán MỘT chữ cái: bấm chuột vào bảng A-Z hoặc gõ phím."),
        indent("- Đoán đúng: chữ hiện ra. Đoán sai: mất một mạng, hình vẽ thêm."),
        indent("- Mở hết chữ trước khi hết mạng là thắng."),
        indent("- Bấm nút [ Gợi ý ] hoặc gõ /h khi cần trợ giúp."),
        indent("- Bấm nút [ Thoát game ] hoặc gõ /q để thoát bất cứ lúc nào."),
        Txt{},
        center("Bấm chuột hoặc nhấn phím bất kỳ để quay lại", DIM),
    });
    RawMode raw;
    waitKeyOrClick();
}

void showGameScreen(const std::string& maskedWord,
                    const std::set<char>& guessed,
                    int livesLeft, int maxLives,
                    const std::string& message) {
    // Chọn hình: chia đều số lần sai (0..maxLives) vào 7 hình (0..6).
    int wrong = std::clamp(maxLives - livesLeft, 0, maxLives);
    int stage = maxLives > 0 ? wrong * 6 / maxLives : 0;

    // Cột trái: hình hangman (bắt đầu từ dòng 1 cho cách viền một dòng trống).
    std::vector<Txt> l(ROWS);
    for (int i = 0; i < 7; ++i) l[i + 1].add(HANGMAN_STAGES[stage][i]);

    // Cột phải: thông tin game, mỗi dòng thụt 2 ô.
    std::vector<Txt> r(ROWS, Txt{"  ", 2});
    r[0].add("Từ cần đoán:", MAIN);
    for (char ch : maskedWord) {
        r[1].add(std::string(1, ch), ch == '_' ? DIM : GOOD);
        r[1].add(" ");
    }
    r[3].add("Đã đoán:", MAIN);
    if (guessed.empty()) r[4].add("(chưa có)", DIM);
    // Mỗi dòng chứa tối đa 18 chữ (cột phải rộng 39); chữ thứ 19 trở đi xuống dòng 5.
    int shown = 0;
    for (char c : guessed) {
        Txt& row = r[shown++ < 18 ? 4 : 5];
        row.add(std::string(1, c), isHit(c, maskedWord) ? GOOD : BAD);
        row.add(" ");
    }
    r[6].add("Mạng: ", MAIN);
    for (int i = 0; i < maxLives; ++i)
        r[6].add("♥ ", i < livesLeft ? BAD : DIM);
    r[8].add("Bảng chữ cái (bấm để chọn):", MAIN);
    for (char c = 'a'; c <= 'z'; ++c) {
        // Chữ đã dùng làm mờ, chưa dùng giữ màu chính. Vị trí phải khớp letterAt().
        Txt& row = r[c <= 'm' ? 9 : 10];
        row.add(std::string(1, static_cast<char>(std::toupper(c))),
                guessed.count(c) ? DIM : MAIN);
        row.add(" ");
    }

    clearScreen();
    edge("╔", "╗");
    boxLine(center(GAME_TITLE, MAIN));
    edge("╠", "╣", "╦");
    for (int i = 0; i < ROWS; ++i) splitLine(l[i], r[i]);
    edge("╠", "╣", "╩");
    boxLine(messageRow(message));
    boxLine(inputRow());
    Txt hint = indent(QUIT_BTN, BAD);
    hint.add("  hoặc gõ /q", DIM);
    boxLine(hint);
    edge("╚", "╝");
    drawEnter(false);
    drawHint(false);
    placeCursor("");
}

// Yêu cầu: gọi ngay sau showGameScreen (các toạ độ chuột dựa vào bố cục đó).
char askForLetter(const std::set<char>& guessed) {
    RawMode raw;
    std::string buf;  // chữ người chơi đang gõ dở
    char hotLetter = 0;    // chữ đang được rê chuột (0 = không có)
    bool hotQuit = false;  // đang rê vào nút Thoát?
    bool hotEnter = false; // đang rê vào nút Enter?
    bool hotHint = false;  // đang rê vào nút Gợi ý?
    for (;;) {
        Event e = readEvent();
        if (e.kind == Event::Eof) return '\0';          // EOF/cin lỗi: coi như thoát

        if (e.kind == Event::Move) {                    // highlight, chỉ vẽ lại phần tử đổi trạng thái
            char c = letterAt(e.x, e.y);
            if (c && guessed.count(c)) c = 0;           // chữ đã dùng thì không sáng lên
            bool q = onQuitBtn(e.x, e.y);
            bool en = onEnterBtn(e.x, e.y);
            bool hi = onHintBtn(e.x, e.y);
            if (c != hotLetter || q != hotQuit || en != hotEnter || hi != hotHint) {
                if (hotLetter) drawLetter(hotLetter, false, false);
                if (hotQuit) drawQuit(false);
                if (hotEnter) drawEnter(false);
                if (hotHint) drawHint(false);
                if (c) drawLetter(c, false, true);
                if (q) drawQuit(true);
                if (en) drawEnter(true);
                if (hi) drawHint(true);
                hotLetter = c;
                hotQuit = q;
                hotEnter = en;
                hotHint = hi;
                placeCursor(buf);                       // trả con trỏ về ô nhập
            }
            continue;
        }

        std::string in;                                 // thứ cần kiểm tra
        if (e.kind == Event::Click) {
            if (onQuitBtn(e.x, e.y)) return '\0';
            if (onHintBtn(e.x, e.y)) return HINT_REQUEST;
            if (onEnterBtn(e.x, e.y)) {
                in = trim(buf);                         // nút Enter = phím Enter
            } else {
                char c = letterAt(e.x, e.y);
                if (!c) continue;                       // click ra ngoài: bỏ qua
                in = std::string(1, c);
            }
        } else if (e.key == "\b") {                     // Backspace: xoá 1 ký tự UTF-8 cuối
            while (!buf.empty() && (static_cast<unsigned char>(buf.back()) & 0xC0) == 0x80) buf.pop_back();
            if (!buf.empty()) buf.pop_back();
            redrawInput(buf);
            continue;
        } else if (e.key == "\n") {
            in = trim(buf);
        } else {
            if (vlen(buf) < 20) buf += e.key;           // giới hạn để không tràn khung
            redrawInput(buf);
            continue;
        }

        if (in == "/q" || in == "/Q") return '\0';
        if (in == "/h" || in == "/H") return HINT_REQUEST;

        std::string err;
        if (in.empty()) {
            err = "Bạn chưa nhập gì, hãy gõ hoặc bấm một chữ cái.";
        } else if (vlen(in) > 1) {
            err = "Chỉ nhập MỘT chữ cái mỗi lượt (bạn đã nhập " +
                  std::to_string(vlen(in)) + " ký tự).";
        } else {
            unsigned char c = static_cast<unsigned char>(in[0]);
            if (c > 127 || !std::isalpha(c)) {
                err = "'" + in + "' không phải chữ cái a-z (không dấu).";
            } else if (guessed.count(static_cast<char>(std::tolower(c)))) {
                err = "Chữ '" + in + "' đã đoán rồi, hãy chọn chữ khác.";
            } else {
                return static_cast<char>(std::tolower(c));
            }
        }

        // Báo lỗi bằng cách ghi đè dòng thông báo + dòng nhập, không vẽ lại cả màn
        // nên bố cục đứng yên.
        buf.clear();
        gotoRC(MSG_ROW, 1);
        boxLine(messageRow(err, BAD));
        redrawInput(buf);
    }
}

// Hai nút nằm trong khung ở dòng RESULT_BTN_ROW.
void showResult(bool won, const std::string& secret, const std::string& meaning) {
    Txt buttons;
    buttons.add(std::string(resultBtnCol() - 2, ' '));
    buttons.add(BTN_YES, GOOD).add(std::string(BTN_GAP, ' ')).add(BTN_NO, BAD);
    drawSimple({
        Txt{},
        center(won ? "BẠN ĐÃ THẮNG!" : "BẠN ĐÃ THUA!", won ? GOOD : BAD),
        Txt{},
        center("Từ bí mật là: " + secret, MAIN),
        meaning.empty() ? Txt{} : center("Nghĩa: " + meaning, MAIN),
        Txt{},
        buttons,
    });
}

// Yêu cầu: gọi ngay sau showResult (nút chuột nằm trong khung đó).
bool askPlayAgain() {
    RawMode raw;
    int hot = 0;  // 0 = không nút nào, 1 = Chơi lại, 2 = Thoát
    for (;;) {
        Event e = readEvent();
        if (e.kind == Event::Eof) return false;         // EOF: không chơi lại
        if (e.kind == Event::Move) {
            int c1 = resultBtnCol(), c2 = c1 + vlen(BTN_YES) + BTN_GAP;
            int h = 0;
            if (e.y == RESULT_BTN_ROW) {
                if (e.x >= c1 && e.x < c1 + vlen(BTN_YES)) h = 1;
                else if (e.x >= c2 && e.x < c2 + vlen(BTN_NO)) h = 2;
            }
            if (h != hot) { hot = h; drawResultBtns(hot); }
            continue;
        }
        if (e.kind == Event::Click) {
            int c1 = resultBtnCol(), c2 = c1 + vlen(BTN_YES) + BTN_GAP;
            if (e.y != RESULT_BTN_ROW) continue;
            if (e.x >= c1 && e.x < c1 + vlen(BTN_YES)) return true;
            if (e.x >= c2 && e.x < c2 + vlen(BTN_NO)) return false;
            continue;
        }
        if (e.key == "y" || e.key == "Y") return true;
        if (e.key == "n" || e.key == "N") return false;
        if (e.key != "\n" && e.key != "\b") {
            gotoRC(RESULT_BTN_ROW + 2, 1);
            std::cout << "\033[2K" << BAD << "Hãy bấm vào nút hoặc gõ y / n." << RESET << std::flush;
        }
    }
}