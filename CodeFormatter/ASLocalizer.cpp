/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ASLocalizer.cpp
 *
 *   Copyright (C) 2006-2011 by Jim Pattee <jimp03@email.com>
 *   Copyright (C) 1998-2002 by Tal Davidson
 *   <http://www.gnu.org/licenses/lgpl-3.0.html>
 *
 *   This file is a part of Artistic Style - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   <http://astyle.sourceforge.net>
 *
 *   Artistic Style is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Artistic Style is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Artistic Style.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __DMC__
#include <locale.h>
#endif

#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <typeinfo>

#include "astyle_main.h"
#include "ASLocalizer.h"

namespace astyle
{

#ifndef ASTYLE_LIB

//----------------------------------------------------------------------------
// ASLocalizer class methods.
//----------------------------------------------------------------------------

ASLocalizer::ASLocalizer()
// Set the locale information.
{
	// set language default values to english (ascii)
	// this will be used if a locale or a language cannot be found
	m_localeName = "UNKNOWN";
	m_langID = "en";
	m_lcid = 0;
	m_subLangID.clear();
	m_translation = NULL;

	// Not all compilers support the C++ function locale::global(locale(""));
	// For testing on Windows change the "Region and Language" settings or use AppLocale.
	// For testing on Linux change the LANG environment variable: LANG=fr_FR.UTF-8.
	// setlocale() will use the LANG environment valiable on Linux.

	char* localeName = setlocale(LC_ALL, "");
	if (localeName == NULL)		// use the english (ascii) defaults
	{
		fprintf(stderr, "\n%s\n\n", "Cannot set native locale, reverting to English");
		setTranslationClass();
		return;
	}
	// set the class variables
#ifdef _WIN32
	size_t lcid = GetUserDefaultLCID();
	setLanguageFromLCID(lcid);
#else
	setLanguageFromName(localeName);
#endif
}

ASLocalizer::~ASLocalizer()
// Delete dynamically allocated memory.
{
	delete m_translation;
}

#ifdef _WIN32

#ifdef __DMC__
// digital mars doesn't have these
const size_t SUBLANG_CHINESE_MACAU = 5;
const size_t LANG_HINDI = 57;
#endif

struct WinLangCode
{
	size_t winLang;
	char canonicalLang[3];
};

static WinLangCode wlc[] =
// primary language identifier http://msdn.microsoft.com/en-us/library/aa912554.aspx
// sublanguage identifier http://msdn.microsoft.com/en-us/library/aa913256.aspx
// language ID http://msdn.microsoft.com/en-us/library/ee797784%28v=cs.20%29.aspx
{
	{ LANG_CHINESE,    "zh" },
	{ LANG_DUTCH,      "nl" },
	{ LANG_ENGLISH,    "en" },
	{ LANG_FINNISH,    "fi" },
	{ LANG_FRENCH,     "fr" },
	{ LANG_GERMAN,     "de" },
	{ LANG_HINDI,      "hi" },
	{ LANG_ITALIAN,    "it" },
	{ LANG_JAPANESE,   "ja" },
	{ LANG_KOREAN,     "ko" },
	{ LANG_POLISH,     "pl" },
	{ LANG_PORTUGUESE, "pt" },
	{ LANG_RUSSIAN,    "ru" },
	{ LANG_SPANISH,    "es" },
	{ LANG_SWEDISH,    "sv" },
	{ LANG_UKRAINIAN,  "uk" },
};

void ASLocalizer::setLanguageFromLCID(size_t lcid)
// Windows get the language to use from the user locale.
// NOTE: GetUserDefaultLocaleName() gets nearly the same name as Linux.
//       But it needs Windows Vista or higher.
//       Same with LCIDToLocaleName().
{
	m_lcid = lcid;
	m_langID == "en";	// default to english

	size_t lang = PRIMARYLANGID(LANGIDFROMLCID(m_lcid));
	size_t sublang = SUBLANGID(LANGIDFROMLCID(m_lcid));
	// find language in the wlc table
	size_t count = sizeof(wlc)/sizeof(wlc[0]);
	for (size_t i = 0; i < count; i++ )
	{
		if (wlc[i].winLang == lang)
		{
			m_langID = wlc[i].canonicalLang;
			break;
		}
	}
	if (m_langID == "zh")
	{
		if (sublang == SUBLANG_CHINESE_SIMPLIFIED || sublang == SUBLANG_CHINESE_SINGAPORE)
			m_subLangID = "CHS";
		else
			m_subLangID = "CHT";	// default
	}
	setTranslationClass();
}

#endif	// _win32

string ASLocalizer::getLanguageID() const
// Returns the language ID in m_langID.
{
	return m_langID;
}

const Translation* ASLocalizer::getTranslationClass() const
// Returns the name of the translation class in m_translation.
{
	assert(m_translation);
	return m_translation;
}

void ASLocalizer::setLanguageFromName(const char* langID)
// Linux set the language to use from the langID.
//
// the language string has the following form
//
//      lang[_LANG][.encoding][@modifier]
//
// (see environ(5) in the Open Unix specification)
//
// where lang is the primary language, LANG is a sublang/territory,
// encoding is the charset to use and modifier "allows the user to select
// a specific instance of localization data within a single category"
//
// for example, the following strings are valid:
//      fr
//      fr_FR
//      de_DE.iso88591
//      de_DE@euro
//      de_DE.iso88591@euro
{
	// the constants describing the format of lang_LANG locale string
	static const size_t LEN_LANG = 2;
	static const size_t LEN_SUBLANG = 2;

	m_lcid = 0;
	string langStr = langID;
	m_langID = langStr.substr(0, LEN_LANG);

	// need the sublang for chinese
	if (m_langID == "zh" && langStr[LEN_LANG] == '_')
	{
		string subLang = langStr.substr(LEN_LANG + 1, LEN_SUBLANG);
		if (subLang == "CN" || subLang == "SG")
			m_subLangID = "CHS";
		else
			m_subLangID = "CHT";	// default
	}
	setTranslationClass();
}

const char* ASLocalizer::settext(const char* textIn) const
// Call the settext class and return the value.
{
	assert(m_translation);
	const string stringIn = textIn;
	return m_translation->translate(stringIn).c_str();
}

void ASLocalizer::setTranslationClass()
// Return the required translation class.
// Sets the class variable m_translation from the value of m_langID.
// Get the language ID at http://msdn.microsoft.com/en-us/library/ee797784%28v=cs.20%29.aspx
{
	assert(m_langID.length());
	if (m_langID == "zh" && m_subLangID == "CHS")
		m_translation = new ChineseSimplified;
	else if (m_langID == "zh" && m_subLangID == "CHT")
		m_translation = new ChineseTraditional;
	else if (m_langID == "en")
		m_translation =  new English;
	else if (m_langID == "es")
		m_translation =  new Spanish;
	else if (m_langID == "fr")
		m_translation = new French;
	else if (m_langID == "de")
		m_translation = new German;
	else if (m_langID == "hi")
		m_translation = new Hindi;
	else	// default
		m_translation = new English;
}

//----------------------------------------------------------------------------
// Translation base class methods.
//----------------------------------------------------------------------------

void Translation::addPair(const string& english, const wstring& translated)
// Add a string pair to the translation vector.
{
	pair<string, wstring> entry (english, translated);
	m_translation.push_back(entry);
}

string Translation::convertToMultiByte(const wstring& wideStr) const
// Convert wchar_t to a multibyte string using the currently assigned locale.
// Return an empty string if an error occurs.
{
	static bool msgDisplayed = false;
	// get length of the output excluding the NULL and validate the parameters
	size_t mbLen = wcstombs(NULL, wideStr.c_str(), 0);
	if (mbLen == string::npos)
	{
		if (!msgDisplayed)
		{
			fprintf(stderr, "\n%s\n\n", "Cannot convert to multi-byte string, reverting to English");
			msgDisplayed = true;
		}
		return "";
	}
	// convert the characters
	char* mbStr = new(nothrow) char[mbLen+1];
	if (mbStr == NULL)
	{
		if (!msgDisplayed)
		{
			fprintf(stderr, "\n%s\n\n", "Bad memory alloc for multi-byte string, reverting to English");
			msgDisplayed = true;
		}
		return "";
	}
	wcstombs(mbStr, wideStr.c_str(), mbLen+1);
	// return the string
	string mbTranslation = mbStr;
	delete [] mbStr;
	return mbTranslation;
}

size_t Translation::getTranslationVectorSize() const
// Return the translation vector size.  Used for testing.
{
	return m_translation.size();
}

bool Translation::getWideTranslation(const string& stringIn, wstring& wideOut) const
// Get the wide translation string. Used for testing.
{
	size_t i;
	for (i = 0; i < m_translation.size(); i++)
	{
		if (m_translation[i].first == stringIn)
		{
			wideOut = m_translation[i].second;
			return true;
		}
	}
	// not found
	wideOut = L"";
	return false;
}

string& Translation::translate(const string& stringIn) const
// Translate a string.
// Return a static string instead of a member variable so the method can have a "const" designation.
// This allows "settext" to be called from a "const" method.
{
	static string mbTranslation;
	mbTranslation.clear();
	for (size_t i = 0; i < m_translation.size(); i++)
	{
		if (m_translation[i].first == stringIn)
		{
			mbTranslation = convertToMultiByte(m_translation[i].second);
			break;
		}
	}
	// not found, return english
	if (mbTranslation.empty())
		mbTranslation = stringIn;
	return mbTranslation;
}

//----------------------------------------------------------------------------
// Translation class methods.
// These classes have only a constructor which builds the language vector.
//----------------------------------------------------------------------------

ChineseSimplified::ChineseSimplified()	// 中文（简体）
{
	addPair("Formatted  %s\n", L"格式化  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"不变    %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"目录  %s\n");
	addPair("Exclude  %s\n", L"排除  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"排除（无与伦比） %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s 格式化   %s 不变   ");
	addPair(" seconds   ", L" 秒   ");
	addPair("%d min %d sec   ", L"%d 分钟 %d 秒   ");
	addPair("%s lines\n", L"%s 线\n");
	addPair("Using default options file %s\n", L"使用默认选项文件%s\n");
	addPair("Invalid option file options:", L"无效的选项文件选项：");
	addPair("Invalid command line options:", L"无效的命令行选项：");
	addPair("For help on options type 'astyle -h'", L"有关期权类型'astyle -h'的帮助");
	addPair("Cannot open options file", L"无法打开选项文件");
	addPair("Cannot open directory", L"无法打开目录");
	addPair("Cannot process the input stream", L"无法处理的输入流");
	addPair("Missing filename in %s\n", L"在%s名失踪\n");
	addPair("Recursive option with no wildcard", L"递归选项没有通配符");
	addPair("Did you intend quote the filename", L"你打算引用文件");
	addPair("No file to process %s\n", L"没有文件处理%s\n");
	addPair("Did you intend to use --recursive", L"你打算使用 --recursive");
	addPair("Cannot process UTF-32 encoding", L"不能处理UTF-32编码");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style 已经终止");
}

ChineseTraditional::ChineseTraditional()	// 中文（繁體）
{
	addPair("Formatted  %s\n", L"格式化  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"不變    %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"目錄  %s\n");
	addPair("Exclude  %s\n", L"排除  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"排除（無與倫比） %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s 格式化   %s 不變   ");
	addPair(" seconds   ", L" 秒   ");
	addPair("%d min %d sec   ", L"%d 分鐘 %d 秒   ");
	addPair("%s lines\n", L"%s 線\n");
	addPair("Using default options file %s\n", L"使用默認選項文件%s\n");
	addPair("Invalid option file options:", L"無效的選項文件選項：");
	addPair("Invalid command line options:", L"無效的命令行選項：");
	addPair("For help on options type 'astyle -h'", L"幫助信息選項類型'astyle -h的'");
	addPair("Cannot open options file", L"無法打開選項文件");
	addPair("Cannot open directory", L"無法打開目錄");
	addPair("Cannot process the input stream", L"無法處理的輸入流");
	addPair("Missing filename in %s\n", L"在%s名失踪\n");
	addPair("Recursive option with no wildcard", L"遞歸選項沒有通配符");
	addPair("Did you intend quote the filename", L"你打算引用文件");
	addPair("No file to process %s\n", L"沒有文件處理%s\n");
	addPair("Did you intend to use --recursive", L"你打算使用 --recursive");
	addPair("Cannot process UTF-32 encoding", L"不能處理 UTF-32編碼");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style 已經終止");
}

English::English()
// this class is NOT translated
{}

French::French()	// Française
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formaté    %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Inchangée  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Répertoire  %s\n");
	addPair("Exclude  %s\n", L"Exclure  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Exclure (non appariés)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formaté   %s inchangée   ");
	addPair(" seconds   ", L" seconde   ");
	addPair("%d min %d sec   ", L"%d min %d sec   ");
	addPair("%s lines\n", L"%s lignes\n");
	addPair("Using default options file %s\n", L"Options par défaut utilisation du fichier %s\n");
	addPair("Invalid option file options:", L"Options Blancs option du fichier:");
	addPair("Invalid command line options:", L"Blancs options ligne de commande:");
	addPair("For help on options type 'astyle -h'", L"Pour de l'aide sur les options tapez 'astyle -h'");
	addPair("Cannot open options file", L"Impossible d'ouvrir le fichier d'options");
	addPair("Cannot open directory", L"Impossible d'ouvrir le répertoire");
	addPair("Cannot process the input stream", L"Impossible de traiter le flux d'entrée");
	addPair("Missing filename in %s\n", L"Nom de fichier manquant dans %s\n");
	addPair("Recursive option with no wildcard", L"Option récursive sans joker");
	addPair("Did you intend quote the filename", L"Avez-vous l'intention de citer le nom de fichier");
	addPair("No file to process %s\n", L"Aucun fichier à traiter %s\n");
	addPair("Did you intend to use --recursive", L"Avez-vous l'intention d'utiliser --recursive");
	addPair("Cannot process UTF-32 encoding", L"Impossible de traiter codage UTF-32");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style a mis fin");
}

German::German()	// Deutsch
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formatiert   %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Unverändert  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Verzeichnis  %s\n");
	addPair("Exclude  %s\n", L"Ausschließen  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Ausschließen (unerreichte)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formatiert   %s unverändert   ");
	addPair(" seconds   ", L" sekunden   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s linien\n");
	addPair("Using default options file %s\n", L"Mit Standard-Optionen Dat %s\n");
	addPair("Invalid option file options:", L"Ungültige Option Datei-Optionen:");
	addPair("Invalid command line options:", L"Ungültige Kommandozeilen-Optionen:");
	addPair("For help on options type 'astyle -h'", L"Für Hilfe zu den Optionen geben Sie 'astyle -h'");
	addPair("Cannot open options file", L"Kann nicht geöffnet werden Optionsdatei");
	addPair("Cannot open directory", L"Kann nicht geöffnet werden Verzeichnis");
	addPair("Cannot process the input stream", L"Kann nicht verarbeiten Input-Stream");
	addPair("Missing filename in %s\n", L"Missing in %s Dateiname\n");
	addPair("Recursive option with no wildcard", L"Rekursive Option ohne Wildcard");
	addPair("Did you intend quote the filename", L"Haben Sie die Absicht Inhalte der Dateiname");
	addPair("No file to process %s\n", L"Keine Datei zu verarbeiten %s\n");
	addPair("Did you intend to use --recursive", L"Haben Sie verwenden möchten --recursive");
	addPair("Cannot process UTF-32 encoding", L"Nicht verarbeiten kann UTF-32-Codierung");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style ist beendet");
}

Hindi::Hindi()	// हिन्दी
// build the translation vector in the Translation base class
{
	// NOTE: Scintilla based editors (CodeBlocks) cannot always edit Hindi.
	//       Use Visual Studio instead.
	addPair("Formatted  %s\n", L"स्वरूपित किया  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"अपरिवर्तित     %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"निर्देशिका  %s\n");
	addPair("Exclude  %s\n", L"निकालना  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"अपवर्जित (बेजोड़)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s स्वरूपित किया   %s अपरिवर्तित   ");
	addPair(" seconds   ", L" सेकंड   ");
	addPair("%d min %d sec   ", L"%d मिनट %d सेकंड   ");
	addPair("%s lines\n", L"%s लाइनों\n");
	addPair("Using default options file %s\n", L"डिफ़ॉल्ट विकल्प का उपयोग कर फ़ाइल %s\n");
	addPair("Invalid option file options:", L"अवैध विकल्प फ़ाइल विकल्प हैं:");
	addPair("Invalid command line options:", L"कमांड लाइन विकल्प अवैध:");
	addPair("For help on options type 'astyle -h'", L"विकल्पों पर मदद के लिए प्रकार 'astyle -h'");
	addPair("Cannot open options file", L"विकल्प फ़ाइल नहीं खोल सकता है");
	addPair("Cannot open directory", L"निर्देशिका नहीं खोल सकता");
	addPair("Cannot process the input stream", L"इनपुट स्ट्रीम प्रक्रिया नहीं कर सकते");
	addPair("Missing filename in %s\n", L"लापता में फ़ाइलनाम %s\n");
	addPair("Recursive option with no wildcard", L"कोई वाइल्डकार्ड साथ पुनरावर्ती विकल्प");
	addPair("Did you intend quote the filename", L"क्या आप बोली फ़ाइलनाम का इरादा");
	addPair("No file to process %s\n", L"कोई फ़ाइल %s प्रक्रिया के लिए\n");
	addPair("Did you intend to use --recursive", L"क्या आप उपयोग करना चाहते हैं --recursive");
	addPair("Cannot process UTF-32 encoding", L"UTF-32 कूटबन्धन प्रक्रिया नहीं कर सकते");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style समाप्त किया है");
}

Spanish::Spanish()	// Español
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formato     %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Inalterado  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Directorio  %s\n");
	addPair("Exclude  %s\n", L"Excluir  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Excluir (incomparable)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formato   %s inalterado   ");
	addPair(" seconds   ", L" segundo   ");
	addPair("%d min %d sec   ", L"%d min %d seg   ");
	addPair("%s lines\n", L"%s líneas\n");
	addPair("Using default options file %s\n", L"Uso de las opciones por defecto del archivo %s\n");
	addPair("Invalid option file options:", L"Opción no válida opciones de archivo:");
	addPair("Invalid command line options:", L"No válido opciones de línea de comando:");
	addPair("For help on options type 'astyle -h'", L"Para obtener ayuda sobre las opciones tipo 'astyle -h'");
	addPair("Cannot open options file", L"No se puede abrir el archivo de opciones");
	addPair("Cannot open directory", L"No se puede abrir el directorio");
	addPair("Cannot process the input stream", L"No se puede procesar el flujo de entrada");
	addPair("Missing filename in %s\n", L"Falta nombre del archivo en %s\n");
	addPair("Recursive option with no wildcard", L"Recursiva opción sin comodín");
	addPair("Did you intend quote the filename", L"Se tiene la intención de citar el nombre de archivo");
	addPair("No file to process %s\n", L"No existe el fichero a procesar %s\n");
	addPair("Did you intend to use --recursive", L"Se va a utilizar --recursive");
	addPair("Cannot process UTF-32 encoding", L"No se puede procesar la codificación UTF-32");
	addPair("\nArtistic Style has terminated", L"\nArtistic Style ha terminado");
}


#endif	// ASTYLE_LIB

}   // end of namespace astyle

