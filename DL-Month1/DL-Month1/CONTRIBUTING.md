## 📋 QUY TẮC LÀM VIỆC NHÓM (Dành cho nội bộ Team)

Để dự án diễn ra trơn tru, hạn chế tối đa lỗi khi gộp code, toàn team vui lòng tuân thủ nghiêm ngặt các quy tắc sau:

### 1. Quy tắc quản lý Task (GitHub Projects)

* **Không ôm việc:** Mỗi người chỉ nhận tối đa **1-2 thẻ (Issue)** ở cột `In Progress` cùng một lúc. Xong việc này mới nhận việc khác.
* **Cập nhật liên tục:** Bắt đầu code -> Kéo thẻ sang `In Progress`. Đẩy code lên PR -> Kéo thẻ sang `In Review`.
* **Gắn tag người liên quan:** Nếu code bị kẹt (block) hoặc cần QA vào test, hãy comment trực tiếp vào thẻ Issue và tag tên người đó (VD: `@QA-name vào test giúp mình luồng nhập số nhé`).

### 2. Quy tắc chia nhánh Git (Branching)

**Tuyệt đối không push code trực tiếp lên nhánh `main`.** Mọi tính năng mới đều phải rẽ nhánh.

* **Cú pháp tên nhánh:** `<loại-công-việc>/<tên-ngắn-gọn>`
* **Tính năng mới:** `feature/doc-file-tu-vung`, `feature/random-chu-cai`
* **Sửa lỗi:** `fix/loi-hien-thi-ascii`, `fix/crash-game-khi-nhap-khoang-trang`

### 3. Quy tắc lưu code (Commit Message)

Tin nhắn commit cần ngắn gọn, rõ ràng và **bắt buộc phải gắn với số thứ tự của thẻ Issue**.

* **Cú pháp:** `[Nhãn] Mô tả công việc đã làm - fixes #Số_Issue`
* **Ví dụ đúng:** `[Core] Xử lý khoảng trắng thừa ở cuối file txt - fixes #3`
* **Ví dụ đúng:** `[UI] Cập nhật mảng vẽ giá treo cổ bước 1 - fixes #5`
* **Ví dụ sai:** `update code`, `xong roi ne`, `fix loi` (Sẽ bị yêu cầu commit lại).

### 4. Quy tắc duyệt code (Pull Request - PR)

* **Ai tạo PR, người đó KHÔNG ĐƯỢC bấm Merge:** Để đảm bảo tính khách quan, code của bạn phải được người khác kiểm tra.
* **Quy trình:** Tạo PR -> Gắn thẻ Reviewer (PM Dương hoặc bạn Dev khác) -> Reviewer vào đọc code C/C++, để lại comment nếu cần sửa.
* **Approve:** Chỉ khi Reviewer bấm `Approve` thì code mới chính thức được gộp vào nhánh `main`.

### 5. Quy chuẩn viết code C/C++ (Coding Convention)

Để 8 người code như 1 người, chúng ta thống nhất cách đặt tên:

* **Tên biến và Tên hàm (camelCase):** Chữ cái đầu viết thường, các chữ cái đầu của từ tiếp theo viết hoa.
* *Ví dụ:* `secretWord`, `badGuesses`, `readWordsFromFile()`.


* **Hằng số (MACRO_CASE):** Viết hoa toàn bộ, cách nhau bằng dấu gạch dưới.
* *Ví dụ:* `MAX_ERRORS = 7`, `FILE_PATH`.


* **Tên file:** Viết thường toàn bộ, cách nhau bằng gạch dưới (VD: `game_logic.cpp`, `ascii_art.h`).
* **Comment code:** Phải comment giải thích ý tưởng ở trước những vòng lặp hoặc thuật toán xử lý chuỗi phức tạp. Không cần comment những dòng gán biến cơ bản.
