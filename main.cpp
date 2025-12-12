#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <unordered_set>

static inline bool isIgnoredUtf8(unsigned char c) {
    return c == 0xE2 || c == 0x80 || c == 0x9E || c == 0x9C || c == 0x93 || c == 0x94;
}

static inline bool isWordChar(unsigned char c) {
    if (isIgnoredUtf8(c)) return false;
    return std::isalnum(c) || c >= 128;
}

static inline bool isUrlEnd(unsigned char c) {
    return std::isspace(c) ||
           c == ',' || c == ';' || c == '"' || c == '\'' ||
           c == '(' || c == ')' || c == '[' || c == ']' ||
           c == '{' || c == '}' || c == '<' || c == '>' ||
           c == '\n' || c == '\r';
}

static inline std::string trimUrlPunct(std::string s) {
    while (!s.empty()) {
        char c = s.back();
        if (c == '.' || c == '!' || c == '?' || c == ':' || c == ')')
            s.pop_back();
        else
            break;
    }
    return s;
}

static inline bool looksLikeDomain(const std::string& s) {
    size_t dot = s.find('.');
    if (dot == std::string::npos) return false;
    if (dot == 0 || dot + 1 >= s.size()) return false;

    auto isDomChar = [](unsigned char c) {
        return std::isalnum(c) || c == '-' || c == '.';
    };

    for (unsigned char c : s) {
        if (!isDomChar(c)) return false;
    }

    size_t lastDot = s.rfind('.');
    if (lastDot == std::string::npos) return false;
    if (s.size() - lastDot - 1 < 2) return false;

    return true;
}

static std::unordered_set<std::string> loadTlds(const std::string& filename) {
    std::unordered_set<std::string> tlds;
    std::ifstream in(filename);
    if (!in) return tlds;

    std::string tld;
    while (in >> tld) {
        std::transform(tld.begin(), tld.end(), tld.begin(),
                       [](unsigned char c){ return (char)std::tolower(c); });
        tlds.insert(tld);
    }
    return tlds;
}

static inline bool hasValidTld(const std::string& domain, const std::unordered_set<std::string>& tlds) {
    if (tlds.empty()) return true;
    size_t pos = domain.rfind('.');
    if (pos == std::string::npos) return false;

    std::string tld = domain.substr(pos + 1);
    std::transform(tld.begin(), tld.end(), tld.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    return tlds.count(tld) > 0;
}

void wordCrossRef(const std::string& filename, const std::string& outFile) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Nepavyko atidaryti " << filename << "\n";
        return;
    }

    std::unordered_map<std::string, int> cnt;
    std::unordered_map<std::string, std::vector<int>> lines;

    std::string word;
    char ch;
    int lineNo = 1;

    auto flush_word = [&]() {
        if (word.size() >= 2) {
            cnt[word]++;
            lines[word].push_back(lineNo);
        }
        word.clear();
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

    struct Row { std::string w; int c; std::vector<int> ln; };
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

    std::ofstream out(outFile, std::ios::binary);
    if (!out) {
        std::cerr << "Nepavyko atidaryti " << outFile << "\n";
        return;
    }

    out << std::left
        << std::setw((int)maxWordLen + 2) << "Word"
        << std::setw(8) << "Count"
        << "Lines\n";
    out << std::string(maxWordLen + 2 + 8 + 20, '-') << "\n";

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

    std::cout << "Zodziu lentele issaugota: " << outFile << "\n";
}

void findUrls(const std::string& filename,
              const std::string& outFile,
              bool printToScreen,
              const std::unordered_set<std::string>& tlds) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Nepavyko atidaryti " << filename << "\n";
        return;
    }

    std::unordered_map<std::string, std::vector<int>> urls;

    std::string line;
    int lineNo = 1;

    while (std::getline(in, line)) {
        for (size_t i = 0; i < line.size(); ++i) {
            if (line.compare(i, 8, "https://") == 0 || line.compare(i, 7, "http://") == 0) {
                size_t start = i, j = i;
                while (j < line.size() && !isUrlEnd((unsigned char)line[j])) j++;
                std::string url = trimUrlPunct(line.substr(start, j - start));
                if (!url.empty()) urls[url].push_back(lineNo);
                i = (j == 0 ? 0 : j - 1);
                continue;
            }

            if (line.compare(i, 4, "www.") == 0) {
                size_t start = i, j = i;
                while (j < line.size() && !isUrlEnd((unsigned char)line[j])) j++;
                std::string url = trimUrlPunct(line.substr(start, j - start));
                if (!url.empty()) urls[url].push_back(lineNo);
                i = (j == 0 ? 0 : j - 1);
                continue;
            }

            if (std::isalnum((unsigned char)line[i])) {
                size_t start = i, j = i;
                while (j < line.size() && !isUrlEnd((unsigned char)line[j])) j++;
                std::string token = trimUrlPunct(line.substr(start, j - start));
                if (looksLikeDomain(token) && hasValidTld(token, tlds)) {
                    urls[token].push_back(lineNo);
                }
                i = (j == 0 ? 0 : j - 1);
            }
        }
        lineNo++;
    }

    std::vector<std::pair<std::string, std::vector<int>>> items;
    items.reserve(urls.size());
    size_t maxLen = 3;

    for (auto &p : urls) {
        auto ln = p.second;
        std::sort(ln.begin(), ln.end());
        ln.erase(std::unique(ln.begin(), ln.end()), ln.end());
        maxLen = std::max(maxLen, p.first.size());
        items.push_back({p.first, std::move(ln)});
    }

    std::sort(items.begin(), items.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    std::ofstream out(outFile);
    if (!out) {
        std::cerr << "Nepavyko atidaryti " << outFile << "\n";
        return;
    }

    out << std::left << std::setw((int)maxLen + 2) << "URL" << "Lines\n";
    out << std::string(maxLen + 2 + 20, '-') << "\n";
    for (const auto& it : items) {
        out << std::left << std::setw((int)maxLen + 2) << it.first;
        for (size_t i = 0; i < it.second.size(); i++) {
            if (i) out << ", ";
            out << it.second[i];
        }
        out << "\n";
    }

    std::cout << "URL lentele issaugota: " << outFile << "\n";

    if (printToScreen) {
        std::cout << "\nSURASTI URL ADRESAI\n\n";
        std::cout << std::left << std::setw((int)maxLen + 2) << "URL" << "Lines\n";
        std::cout << std::string(maxLen + 2 + 20, '-') << "\n";
        for (const auto& it : items) {
            std::cout << std::left << std::setw((int)maxLen + 2) << it.first;
            for (size_t i = 0; i < it.second.size(); i++) {
                if (i) std::cout << ", ";
                std::cout << it.second[i];
            }
            std::cout << "\n";
        }
        if (items.empty()) std::cout << "(URL nerasta.)\n";
        std::cout << "\n";
    }
}

int main() {
    const std::string inputFile = "tekstas.txt";
    const std::string tldFile = "visi_url.txt";
    auto tlds = loadTlds(tldFile);

    std::cout << "1 - Zodziu pasikartojimai, eilutes (output_words.txt)\n";
    std::cout << "2 - URL paieska (output_urls.txt + ekranas)\n";
    std::cout << "3 - Abi uzduotys\n";
    std::cout << "Ivesk pasirinkima: ";

    int choice = 0;
    std::cin >> choice;

    if (choice == 1) {
        wordCrossRef(inputFile, "output_words.txt");
    } else if (choice == 2) {
        findUrls(inputFile, "output_urls.txt", true, tlds);
    } else if (choice == 3) {
        wordCrossRef(inputFile, "output_words.txt");
        findUrls(inputFile, "output_urls.txt", true, tlds);
    } else {
        std::cout << "Neteisingas pasirinkimas.\n";
    }

    return 0;
}
