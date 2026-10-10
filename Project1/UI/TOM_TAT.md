# Tóm tắt tiến trình: UI Hangman (C++17, terminal, Windows)

Phạm vi: chỉ phần UI/UX. Phần core (`game.h`, `game.cpp`, `main.cpp`) do bạn khác phụ trách, không đụng tới.
Công nghệ: thư viện chuẩn + ANSI escape code, hình ASCII tách ra file riêng.
Cập nhật lần cuối: cuối phiên 3 (thêm nút Enter, nút Gợi ý, độ khó, nghĩa tiếng Việt; sửa lỗi nút Thoát). **Phần UI đã xong, sẵn sàng ghép với core.**

---

## 1. Đã làm tới đâu

### Các file hiện có

| File | Vai trò | Trạng thái |
|---|---|---|
| `ascii_art.h` | Dữ liệu: tiêu đề + 7 hình hangman | Xong, không đổi |
| `ui.h` | Khai báo hàm UI (hợp đồng với core) | **Đã đổi ở phiên 3** (thêm `showDifficultyMenu`, `HINT_REQUEST`, tham số `meaning`) |
| `ui.cpp` | Cài đặt toàn bộ hàm UI | **Đã sửa ở phiên 3**, test trên Linux |
| `ui_full_test.cpp` | **Mới.** Chạy cả luồng chơi trong một lần: chào → menu → độ khó → ván → kết quả → chơi lại | Xong |
| `ui_test.cpp` | Test từng màn bằng dữ liệu giả (bản cũ) | Còn dùng được, không có tính năng mới |
| `ui_test.exe` | Bản biên dịch cũ | **Lỗi thời**, phải biên dịch lại |
| `main.cpp`, `game.h`, `game.cpp` | Ghép game / core | Không thuộc phần của bạn |

Bản mới nhất của `ui.cpp`, `ui.h`, `ui_full_test.cpp` nằm trong thư mục outputs của phiên 3. Hãy thay vào dự án trước khi làm tiếp.

### Tính năng đã có

- Màn chơi đủ thành phần: tiêu đề, hình hangman 7 giai đoạn, từ đã che, chữ đã đoán (xanh = đúng, đỏ = sai), thanh mạng `♥`, bảng A-Z (chữ đã dùng bị làm mờ), dòng thông báo, ô nhập, nút thoát.
- Xoá màn hình mỗi lượt, bố cục đứng yên 80 cột. Báo lỗi input thân thiện bằng cách ghi đè dòng thông báo (nhiều ký tự, không phải chữ cái, đã đoán rồi, chưa nhập gì).
- Xử lý EOF/lỗi đọc không treo chương trình. Dọn dẹp khi bị ngắt đột ngột, xoá rác chuột trong bộ đệm, trả lại code page cũ (từ phiên 2).
- Điều khiển bằng chuột + highlight khi rê chuột, chỉ vẽ lại đúng phần tử đổi trạng thái. Bàn phím vẫn dùng được cho mọi thứ.
- Đoạn `#ifdef _WIN32` bật ANSI, UTF-8 và tắt QuickEdit để chuột hoạt động.
- **Mới (phiên 3):**
  - Nút `[ Enter ]` trên dòng nhập (dòng 17, cột 66): bấm = gửi chữ đang gõ, giống phím Enter. Gõ phím hay báo lỗi không còn xoá mất nút.
  - Nút `[ Gợi ý ]` (xanh lá) ngay dưới nút Enter (dòng 18, cột 66), hoặc gõ `/h`. UI chỉ báo "xin gợi ý", core quyết định gợi ý là gì.
  - Màn chọn độ khó: `[ Dễ ]` xanh lá, `[ Thường ]` **vàng**, `[ Khó ]` đỏ, dàn hàng ngang căn giữa, kèm nút `[ Quay lại ]` ở dòng dưới. Phím tắt 1-3 và 0.
  - Màn kết quả có thêm dòng "Nghĩa: ..." (tiếng Việt). Nút Chơi lại / Thoát dịch xuống dòng 10.
  - Menu chính và menu độ khó dùng chung một hàm `runMenu`.
  - `showHelp` có thêm dòng nói về nút Gợi ý.
  - `ui_full_test.cpp`: bộ 8 từ có nghĩa, mở sẵn khoảng 1/3 số chữ đầu ván, Gợi ý mở thêm 1 chữ và mất 1 mạng (chỉ là quy tắc của test).

### Hợp đồng với core

```cpp
inline constexpr char HINT_REQUEST = '?';             // MỚI

void showWelcome();
int  showMainMenu();                                  // 1 Chơi, 2 Hướng dẫn, 3 Thoát (EOF -> 3)
int  showDifficultyMenu();                            // MỚI: 1 Dễ, 2 Thường, 3 Khó, 0 = quay lại menu
void showHelp();
void showGameScreen(const std::string& maskedWord, const std::set<char>& guessed,
                    int livesLeft, int maxLives, const std::string& message);
char askForLetter(const std::set<char>& guessed);     // chữ thường | '\0' | HINT_REQUEST
void showResult(bool won, const std::string& secret,
                const std::string& meaning = "");     // ĐÃ ĐỔI: thêm meaning (mặc định rỗng)
bool askPlayAgain();
```

Các quy ước cần báo bạn phụ trách core:

- **Luồng gọi:** `showMainMenu` → nếu chọn 1 thì `showDifficultyMenu` → nếu trả `0` thì quay lại `showMainMenu` → ngược lại vào ván.
- `askForLetter` trả `'\0'` khi người chơi muốn thoát (nút Thoát game, gõ `/q`, hoặc EOF). **`main.cpp` phải thoát khỏi ván và quay lại vòng lặp menu, KHÔNG được `return` khỏi `main`.**
- `askForLetter` trả `HINT_REQUEST` (`'?'`) khi bấm nút Gợi ý hoặc gõ `/h`. Core tự quyết: gợi ý là gì, có tốn mạng không, có giới hạn lần không, rồi truyền kết quả qua tham số `message` của lượt sau.
- **Thứ tự gọi bắt buộc:** `askForLetter` ngay sau `showGameScreen`; `askPlayAgain` ngay sau `showResult` (toạ độ chuột dựa vào bố cục vừa vẽ).
- **Việc của core (UI không làm):** kho từ kèm nghĩa tiếng Việt (truyền vào `showResult`), luật chia độ khó, mở sẵn một số chữ đầu ván. Chữ mở sẵn phải có trong `maskedWord` và nên thêm vào `guessed` (nếu không thì bảng A-Z vẫn sáng chữ đó). Hệ quả: chúng cũng hiện trong danh sách "Đã đoán".
- Từ bí mật là a-z không dấu, `maskedWord` dùng `_` cho ô chưa mở, `maxLives = 6`.

### Bố cục toạ độ (nếu sửa khung thì phải sửa các hằng này trong `ui.cpp`)

| Màn | Dòng | Ghi chú |
|---|---|---|
| Menu chính | 5-7 | 3 mục, `MENU_ROW0` |
| Chọn độ khó | 6 (3 nút ngang), 8 (Quay lại) | Cột tính bằng `centerCol` |
| Màn chơi | 16 thông báo, 17 nhập + Enter (cột 66), 18 Thoát (cột 4) + Gợi ý (cột 66) | `MSG_ROW`, `INPUT_ROW`, `QUIT_ROW`, `ENTER_COL`, `QUIT_COL` |
| Kết quả | 10 | `RESULT_BTN_ROW = BODY_ROW + 6` |

### Cách biên dịch và chạy (Windows Terminal)

```
g++ -std=c++17 ui_full_test.cpp ui.cpp -o ui_full_test
.\ui_full_test
```

Ghép với core: thêm `ui.cpp` vào lệnh biên dịch cùng `main.cpp game.cpp`.

---

## 2. Các lỗi đã xảy ra

### Đã xử lý trước phiên 3

| # | Lỗi | Nguyên nhân | Trạng thái |
|---|---|---|---|
| 1 | Chạy trong terminal của VS Code không chạy được | **Chưa xác định**, chưa có thông báo lỗi. Giả thuyết: nút ▶ / Code Runner chỉ biên dịch một file nên thiếu `ui.cpp`; quên `.\`; chưa cài g++ hoặc chưa có trong PATH; lỗi encoding | **Còn mở** (do môi trường người dùng, tạm bỏ qua) |
| 2-7 | Đáy giá treo lệch, các lỗi của file test, lỗi sandbox (chi tiết ở bản tóm tắt cũ) | Dữ liệu / test / môi trường | Đã sửa hoặc không phải lỗi sản phẩm |
| 8 | Rác chuột `ESC[<35;12;7M` nằm lại bộ đệm | `~RawMode` xoá bộ đệm | Linux: pass. Windows: chưa |
| 9 | Ctrl+C / đóng cửa sổ để lại chế độ chuột và chế độ thô | Bộ xử lý ngắt (`SetConsoleCtrlHandler`, `SIGINT/SIGTERM/SIGHUP`) | Linux: pass. Windows: chưa |
| 10 | Windows không trả lại code page cũ | `ConsoleInit` lưu và khôi phục trong destructor | Chưa test. Ctrl+C chưa khôi phục code page |
| 11 | `♡` rộng hơn `♥` tuỳ font | Mạng đã mất dùng `♥` màu xám | Linux: đúng 80 cột. Font Windows: chưa |
| 12 | "Đã đoán" tràn khung khi hơn 18 chữ | Chữ thứ 19 trở đi xuống dòng 5 | Đã thử 24 chữ |

### Đã sửa ở phiên 3

| # | Lỗi | Nguyên nhân | Cách sửa | Kiểm tra |
|---|---|---|---|---|
| 13 | Nút Thoát game hiện `[ Thoát game ]]` và lệch trái một ô sau khi rê chuột qua | Lệch một cột có từ trước: nút vẽ ở cột 4 nhưng hàm highlight vẽ lại ở cột 3, dấu `]` cũ còn sót. Vùng bấm cũng lệch một cột | Thêm `QUIT_COL = 4`, dùng chung cho vẽ và vùng bấm (hàm `onQuitBtn`) | Linux: pass. Windows: chưa |
| 14 | Bấm Thoát game giữa ván làm cả chương trình thoát về PowerShell | Lỗi ở `ui_full_test.cpp` (`return 0` thay vì quay lại menu), không phải `ui.cpp` | Đổi thành thoát ván rồi quay lại menu | Linux: pass cả bấm chuột và gõ `/q` |
| 15 | Gõ phím hay báo lỗi sẽ xoá mất nút Enter (phát hiện khi thêm nút) | Dòng nhập vẽ lại cả hàng | `redrawInput` chỉ vẽ tới trước nút | Linux: pass |

### Rủi ro / điểm còn nghi ngờ (chưa xác nhận là lỗi)

- **Toàn bộ phần chuột, highlight và các nút mới (Enter, Gợi ý, độ khó, Quay lại) mới chỉ được test trên Linux** bằng input giả lập (đưa chuỗi byte qua pipe). Chưa nhìn bằng mắt, chưa chạy trên Windows thật: `SetConsoleMode`, `ReadFile` với VT input, và việc Windows Terminal có gửi sự kiện di chuột (mode 1003) hay không.
- `readByte` trên Windows dùng `ReadFile` đọc từng byte. Nếu trả 0 byte khi gõ chữ có dấu trên một số bản console cũ, chương trình sẽ coi là EOF và thoát. Chưa xác nhận.
- Rê chuột nhanh sinh nhiều sự kiện, mỗi byte một lệnh đọc, có thể giật trên Windows.
- Toạ độ chuột là hằng số cứng. Cửa sổ nhỏ hơn 80x24 hoặc màn hình bị cuộn thì bấm trúng nhầm ô (đã quyết định bỏ qua).
- **Màu vàng** của độ khó Thường là màu thứ 5, ngoài 4 màu ban đầu. Một số theme terminal hiển thị vàng hơi ngả cam/nâu.
- Phím Esc đơn lẻ làm chương trình chờ thêm 1 byte mới trả về (game không dùng Esc). Ctrl+C trên Windows chưa khôi phục code page.
- Nút Gợi ý luôn hiện, UI không biết còn gợi ý hay không. Hết lượt thì core phải báo qua `message`.
- Chưa có bản `ui_test.exe` mới. Bản hiện có là bản rất cũ.

---

## 3. Hướng làm tiếp theo

### Việc cần làm ở phiên sau, theo thứ tự ưu tiên

1. **Gửi cho bạn phụ trách core** hợp đồng ở mục 1 (đặc biệt: `'\0'` = thoát ván về menu, `HINT_REQUEST`, `showDifficultyMenu` trả 0 = quay lại, tham số `meaning`, thứ tự gọi bắt buộc, những việc core phải tự làm).
2. **Ghép với core trong `main.cpp`:** kiểm tra vòng lặp menu → độ khó → ván, các nhánh `'\0'`, `HINT_REQUEST`, truyền nghĩa vào `showResult`, chơi thử trọn một ván bằng chuột lẫn bàn phím.
3. **Biên dịch lại bằng bản mới và chạy `ui_full_test` trên Windows Terminal**, xác nhận:
   - Nút `[ Thoát game ]` hiện đúng, rê chuột qua rồi ra không còn `]]`.
   - Nút `[ Enter ]` và `[ Gợi ý ]` nằm đẹp, sáng lên khi rê, bấm trúng đúng nút.
   - Màn độ khó: ba nút ngang đúng màu (xanh lá / vàng / đỏ), bấm đúng, Quay lại về menu chính.
   - Màn kết quả hiện nghĩa tiếng Việt đúng dấu, nút Chơi lại / Thoát bấm trúng.
   - Mục 8 của `ui_test`: `♥ ╔═╗ ă ư` hiện đúng, dòng `Mạng:` thẳng hàng với viền phải.
   - Gõ chữ có dấu vào ô nhập: báo lỗi "không phải chữ cái a-z", không thoát chương trình.
   - Ctrl+Z rồi Enter ở ô nhập: thoát êm. Ctrl+C giữa màn chơi: shell sau đó không có rác chuột, gõ chữ vẫn hiện.
4. **Xử lý lỗi VS Code còn mở.** Gửi nguyên văn thông báo lỗi (chữ đỏ hoặc ảnh chụp) và cho biết dùng PowerShell hay cmd.

### Đề xuất cho các phiên làm việc sau

**Cách bắt đầu phiên mới cho nhanh**
- Gửi lại các file: `TOM_TAT.md`, `ui.h`, `ui.cpp` (bản mới), `ui_full_test.cpp`, `ascii_art.h`, và `main.cpp` / `game.*` nếu đã có. Nhắc mình đọc `ui.cpp` đầy đủ trước khi sửa, vì file dài hơn 700 dòng nên hay bị cắt trong ngữ cảnh.
- Nếu có lỗi khi chạy, gửi kèm: hệ điều hành, terminal đang dùng (Windows Terminal / PowerShell / cmd / VS Code), nguyên văn thông báo lỗi hoặc ảnh chụp.
- Nên làm mỗi phiên một việc rõ ràng (ví dụ "chỉ test Windows" hoặc "chỉ ghép core").

**Cách báo kết quả test Windows để mình sửa nhanh**
- Ghi theo từng mục ở danh sách kiểm tra trên: mục nào đúng, mục nào sai, sai thế nào (ảnh chụp khung nếu lệch).

**Việc nên làm thêm (chưa ai yêu cầu, chỉ làm khi cần)**
- Nếu rê chuột bị giật: đọc nhiều byte một lần hoặc gộp các sự kiện di chuột liên tiếp, chỉ xử lý sự kiện cuối.
- Nếu chữ có dấu làm thoát nhầm: chuyển Windows sang `ReadConsoleInputW` thay cho `ReadFile`.
- Báo thân thiện khi cửa sổ terminal nhỏ hơn 80x24 (kiểm tra bằng `GetConsoleScreenBufferInfo`).
- Nếu cần hiển thị số lần gợi ý còn lại hoặc làm mờ nút Gợi ý khi hết lượt: thêm tham số cho `showGameScreen` (đổi hợp đồng, phải báo core).
- Nếu muốn nút Quay lại ở các màn khác (hướng dẫn, kết quả) hoặc menu chính cũng căn giữa/xếp ngang: `runMenu` đã hỗ trợ sẵn kiểu căn giữa.
- Nếu cần chạy cả Linux/macOS: kiểm tra lại phần `#else` của `RawMode` trên máy thật.
- Khi core xong: viết thêm một ván chơi tự động (đưa input giả qua pipe) để kiểm tra toàn bộ luồng không treo.
