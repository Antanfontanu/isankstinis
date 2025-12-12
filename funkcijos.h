#ifndef FUNKCIJOS_H
#define FUNKCIJOS_H

#include "mylib.h"

std::unordered_set<std::string> loadTlds(const std::string& filename);


void wordCrossRef(const std::string& filename, const std::string& outFile);

void findUrls(const std::string& filename,
              const std::string& outFile,
              bool printToScreen,
              const std::unordered_set<std::string>& tlds);

#endif
