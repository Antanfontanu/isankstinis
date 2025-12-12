#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>  

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
    std::unordered_map<std::string, std::vector<int>> lines;

    std::string word;
    char ch;
    int lineNo = 1;

    auto flush_word = [&]() {
        if (!word.empty()) {
            cnt[word]++;
            lines[word].push_back(lineNo);
            word.clear();
        }
    };

    while (in.get(ch)) {
        unsigned char uc = static_cast<unsigned char>(ch);

        if (ch == '\n') {
            flush_word();
            lineNo++;
            continue;
        }

        if (isWordChar(uc)) {
            if (uc < 128) uc = static_cast<unsigned char>(std::tolower(uc));
            word.push_back(static_cast<char>(uc));
        } else {
            flush_word();
        }
    }
    flush_word();

    struct Row {
        std::string w;
        int c;
        std::vector<int> ln;
    };

    std::vector<Row> items;
    items.reserve(cnt.size());
    size_t maxWordLen = 4; 

    for (auto &p : cnt) {
        if (p.second > 1) {
            auto ln = lines[p.first];
            std::sort(ln.begin(), ln.end());
            ln.erase(std::unique(ln.begin(), ln.end()), ln.end()); 
            maxWordLen = std::max(maxWordLen, p.first.size());
            items.push_back({p.first, p.second, std::move(ln)});
        }
    }

    std::sort(items.begin(), items.end(),
              [](const Row &a, const Row &b) {
                  if (a.c != b.c) return a.c > b.c;
                  return a.w < b.w;
              });

    std::ofstream out("output.txt", std::ios::binary);
    if (!out) {
        std::cerr << "Nepavyko atidaryti output.txt\n";
        return 1;
    }

   
    out << std::left
        << std::setw((int)maxWordLen + 2) << "Word"
        << std::setw(8) << "Count"
        << "Lines\n";

    out << std::string(maxWordLen + 2 + 8 + 5, '-') << "\n";


    for (const auto &it : items) {
        out << std::left
            << std::setw((int)maxWordLen + 2) << it.w
            << std::setw(8) << it.c;

        for (size_t i = 0; i < it.ln.size(); i++) {
            if (i) out << ", ";
            out << it.ln[i];
        }
        out << "\n";
    }

    std::cout << "Rezultatai isvesti i output.txt\n";
    return 0;
}
