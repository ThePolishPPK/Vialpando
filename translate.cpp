#include "translate.hpp"

#include <string>
#include <tinygettext/tinygettext.hpp>

using namespace tinygettext;

void processDicts() { DICTONARIES.add_directory(TRANSLATE_DIRECTORY); }

void setLanguage(const std::string& lang) {
	DICT = &DICTONARIES.get_dictionary(Language::from_name(lang));
}

std::string tr(const std::string& txt) { return DICT->translate(txt); }
std::string tr(const char* txt) { return tr(std::string(txt)); }
