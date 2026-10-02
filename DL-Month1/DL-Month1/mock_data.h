#ifndef MOCK_DATA_H
#define MOCK_DATA_H

#include <string>
#include <vector>

// Du lieu gia (mock) de test GuessProcessor doc lap, khong phu thuoc
// vao branch feature/data-loader cua teammate. Khi nao branch do
// merge vao main, chi can doi loadMockWordList() thanh loadWordList()
// (tu docdulieu.h) la xong - khong phai sua GuessProcessor.
std::vector<std::string> loadMockWordList();

#endif // MOCK_DATA_H
