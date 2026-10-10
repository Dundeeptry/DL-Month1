#include "DataLoader.hpp"

#include <fstream>
#include <iostream>
#include <utility>

#include "../lib/json.hpp"  // Thư viện nlohmann/json (single-header) đặt trong lib/

using json = nlohmann::json;

// Khởi tạo bộ sinh số ngẫu nhiên bằng seed lấy từ phần cứng (random_device)
DataLoader::DataLoader() : rng_(std::random_device{}()) {}

bool DataLoader::loadFromFile(const std::string& filepath) {
    // Mở file JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] Khong mo duoc file: " << filepath << '\n';
        return false;
    }

    try {
        // Parse toàn bộ nội dung file thành đối tượng JSON
        json data = json::parse(file);

        // File phải là một mảng các object
        if (!data.is_array()) {
            std::cerr << "[DataLoader] JSON khong phai la mot mang.\n";
            return false;
        }

        // Đọc vào biến tạm để nếu lỗi giữa chừng thì dữ liệu cũ không bị hỏng
        std::vector<WordItem> loaded;
        std::unordered_map<std::string, std::vector<std::size_t>> index;
        loaded.reserve(data.size());

        for (const auto& item : data) {
            WordItem w;
            // at() ném exception nếu thiếu key, get<std::string>() ném nếu sai kiểu
            w.word       = item.at("word").get<std::string>();
            w.meaning    = item.at("meaning").get<std::string>();
            w.hint       = item.at("hint").get<std::string>();
            w.difficulty = item.at("difficulty").get<std::string>();

            // Ghi nhận vị trí của từ vào nhóm độ khó tương ứng
            index[w.difficulty].push_back(loaded.size());
            loaded.push_back(std::move(w));
        }

        // Mọi thứ hợp lệ: thay thế dữ liệu cũ
        words_ = std::move(loaded);
        indexByDifficulty_ = std::move(index);
        return true;

    } catch (const json::exception& e) {
        // Lỗi cú pháp JSON, thiếu key hoặc sai kiểu dữ liệu
        std::cerr << "[DataLoader] Loi JSON: " << e.what() << '\n';
        return false;
    } catch (const std::exception& e) {
        // Các lỗi chuẩn khác (ví dụ hết bộ nhớ)
        std::cerr << "[DataLoader] Loi: " << e.what() << '\n';
        return false;
    }
}

WordItem DataLoader::getRandomWordByDifficulty(const std::string& difficulty) {
    // Tìm nhóm từ theo độ khó
    const auto it = indexByDifficulty_.find(difficulty);
    if (it == indexByDifficulty_.end() || it->second.empty()) {
        return WordItem{};  // Không có từ phù hợp
    }

    // Chọn đều ngẫu nhiên một vị trí trong nhóm
    const auto& indices = it->second;
    std::uniform_int_distribution<std::size_t> dist(0, indices.size() - 1);
    return words_[indices[dist(rng_)]];
}

bool DataLoader::isEmpty() const {
    return words_.empty();
}