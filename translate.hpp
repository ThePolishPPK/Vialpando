#ifndef TRANSLATE_H
#define TRANSLATE_H

#define TRANSLATE_DIRECTORY "./po/"

#include <string>
#include <tinygettext/tinygettext.hpp>

using namespace tinygettext;

static DictionaryManager DICTONARIES;
static Dictionary* DICT;

void processDicts();
void setLanguage(const std::string& lang);
std::string tr(const std::string& txt);
std::string tr(const char* txt);

#endif