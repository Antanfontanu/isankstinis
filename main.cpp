#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

static inline bool isWordChar(unsigned char c) {

    return std::isalnum(c) || c >= 128;
}

int main() {
    std::ifstream in("tekstas.txt", std::ios::binary);
    if (!in) {
        std::cerr << "Nepavyko atidaryti tekstas.txt\n";
        return 1;
    }

    std::unordered_map<std::string, int> cnt;
    std::string word;
    char ch;

    while (in.get(ch)) {
        unsigned char uc = static_cast<unsigned char>(ch);

        if (isWordChar(uc)) {
            
            if (uc < 128) uc = static_cast<unsigned char>(std::tolower(uc));
            word.push_back(static_cast<char>(uc));
        } else {
            if (!word.empty()) {
                cnt[word]++;
                word.clear();
            }
        }
    }
    if (!word.empty()) cnt[word]++;

   
    std::vector<std::pair<std::string,int>> items;
    items.reserve(cnt.size());
    for (auto &p : cnt) if (p.second > 1) items.push_back(p);

    std::sort(items.begin(), items.end(),
              [](auto &a, auto &b){
                  if (a.second != b.second) return a.second > b.second;
                  return a.first < b.first;
              });

    std::ofstream out("output.txt", std::ios::binary);
    if (!out) {
        std::cerr << "Nepavyko atidaryti output.txt\n";
        return 1;
    }

    for (auto &p : items) {
        out << p.first << " : " << p.second << "\n";
    }

    std::cout << " Rezultatai išvesti output.txt\n";
    return 0;
}
