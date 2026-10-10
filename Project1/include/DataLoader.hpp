#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include <cstddef>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "Types.hpp"

// Đọc dữ liệu từ vựng từ file JSON và cung cấp từ ngẫu nhiên theo độ khó
class DataLoader {
public:
    DataLoader();

    // Đọc file JSON và lưu vào bộ nhớ nội bộ.
    // Trả về true nếu thành công, false nếu file không mở được
    // hoặc cấu trúc JSON không hợp lệ (dữ liệu cũ được giữ nguyên khi thất bại).
    bool loadFromFile(const std::string& filepath);

    // Trả về ngẫu nhiên một từ có độ khó "easy", "medium" hoặc "hard".
    // Nếu không có từ nào phù hợp, trả về WordItem rỗng (word == "").
    WordItem getRandomWordByDifficulty(const std::string& difficulty);

    // Kiểm tra bộ nhớ dữ liệu có trống hay không
    bool isEmpty() const;

private:
    std::vector<WordItem> words_;  // Toàn bộ từ vựng đã đọc
    // Chỉ số (index) trong words_ được nhóm sẵn theo độ khó,
    // giúp việc chọn ngẫu nhiên nhanh mà không cần lọc lại mỗi lần
    std::unordered_map<std::string, std::vector<std::size_t>> indexByDifficulty_;
    std::mt19937 rng_;             // Bộ sinh số ngẫu nhiên, khởi tạo một lần
};

#endif // DATALOADER_HPP