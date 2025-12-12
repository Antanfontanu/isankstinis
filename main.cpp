#include "funkcijos.h"

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
