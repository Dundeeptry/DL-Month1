# Project 1 - Hangman Game (Học từ vựng tiếng Anh)

Dự án này là một trò chơi Hangman dạng console, giúp người chơi luyện từ vựng tiếng Anh theo từng cấp độ khó: easy, medium, hard. Người dùng sẽ đoán từng chữ cái của từ tiếng Anh, xem gợi ý, và có tối đa 6 mạng mỗi ván.

## Mục tiêu dự án

- Học từ vựng tiếng Anh qua trò chơi tương tác
- Cung cấp nhiều mức độ khó khác nhau
- Tạo một ứng dụng C++ đơn giản nhưng có cấu trúc rõ ràng
- Dùng dữ liệu JSON để quản lý từ vựng và gợi ý

## Tính năng chính

- Chọn độ khó: easy, medium, hard
- Lấy từ ngẫu nhiên từ file `data/data.json`
- Hiển thị từ ẩn dạng `_ _ _ _ _`
- Ghi nhận các chữ cái đã đoán
- Hiển thị số mạng còn lại bằng biểu tượng cảm xúc
- Hiển thị gợi ý và nghĩa tiếng Việt của từ
- Chơi nhiều ván liên tiếp nếu người dùng muốn
- Hỗ trợ hiển thị UTF-8 trên Windows cho tiếng Việt và emoji

## Cấu trúc thư mục

```text
Project1/
├── data/
│   └── data.json              # Danh sách từ vựng theo độ khó
├── include/
│   ├── Types.hpp              # Định nghĩa enum và struct WordItem
│   ├── DataLoader.hpp         # Interface đọc dữ liệu JSON
│   └── GameLogic.hpp          # Interface xử lý luật game
├── lib/
│   └── json.hpp               # Thư viện nlohmann/json (single header)
├── src/
│   ├── main.cpp               # Giao diện console và vòng lặp game
│   ├── DataLoader.cpp         # Đọc file JSON, chọn từ ngẫu nhiên
│   └── GameLogic.cpp          # Logic đoán chữ, tính điểm và mạng
├── hangman.exe                # File thực thi đã build sẵn (nếu có)
├── README.md                  # Tài liệu dự án
└── ...
```

## Yêu cầu hệ thống

- Trình biên dịch C++ hỗ trợ chuẩn C++17 trở lên
- Compiler: `g++`, `clang++`, hoặc MinGW trên Windows
- Thiết bị chạy hỗ trợ đầu ra UTF-8 (đặc biệt trên Windows)

## Cách build

Tại thư mục gốc của project, chạy lệnh:

### Linux/macOS

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/DataLoader.cpp src/GameLogic.cpp -o hangman
```

### Windows (MinGW / g++)

```bash
g++ -std=c++17 -Iinclude src\main.cpp src\DataLoader.cpp src\GameLogic.cpp -o hangman.exe
```

## Cách chạy

Sau khi build xong:

### Linux/macOS

```bash
./hangman
```

### Windows

```bash
hangman.exe
```

Lưu ý: chương trình cố gắng đọc dữ liệu từ `data/data.json` theo các đường dẫn tương đối. Vì vậy nên chạy từ thư mục gốc của project để tránh lỗi file không tìm thấy.

## Cấu trúc dữ liệu trong `data.json`

Các mục từ vựng được lưu dưới dạng mảng JSON, mỗi phần tử có 4 trường:

```json
[
  {
    "word": "apple",
    "meaning": "quả táo",
    "hint": "A red or green fruit that grows on trees",
    "difficulty": "easy"
  }
]
```

Trong đó:

- `word`: từ tiếng Anh cần đoán
- `meaning`: nghĩa tiếng Việt
- `hint`: gợi ý bằng tiếng Anh
- `difficulty`: mức độ khó (`easy`, `medium`, `hard`)

## Luật chơi

1. Người chơi chọn mức độ dễ, trung bình hoặc khó.
2. Hệ thống chọn ngẫu nhiên một từ theo độ khó.
3. Màn hình hiển thị từ ẩn và gợi ý.
4. Người chơi nhập một chữ cái.
5. Nếu đúng, chữ cái xuất hiện ở các vị trí tương ứng.
6. Nếu sai, số mạng giảm đi 1.
7. Người chơi thắng khi đoán hết chữ.
8. Người chơi thua khi số mạng về 0.
9. Sau mỗi ván, chương trình hỏi người chơi có muốn tiếp tục hay không.

## Ghi chú

- Dự án dùng thư viện `nlohmann/json` trong `lib/json.hpp` để xử lý dữ liệu JSON.
- Mã nguồn được tổ chức theo mô hình tách biệt UI, logic game và tải dữ liệu để dễ mở rộng.
- Đây là một dự án học tập phù hợp để tìm hiểu về C++, đọc file JSON, xử lý chuỗi và thiết kế game console.

## Tác giả / Mục đích học tập

Dự án này phù hợp cho việc luyện tập lập trình C++ cơ bản và nâng cao, đặc biệt là:

- thao tác với chuỗi
- cấu trúc dữ liệu
- đọc/parse JSON
- xây dựng game console
- phân chia code theo module

Nếu cần, bạn có thể tiếp tục mở rộng dự án bằng cách thêm:

- hệ thống điểm số
- bảng xếp hạng
- giao diện đồ họa
- nhiều chủ đề từ vựng khác nhau
- lưu trạng thái người chơi
