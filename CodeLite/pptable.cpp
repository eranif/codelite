//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : pptable.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "pptable.h"

#include "Cxx/CxxLexerAPI.h"
#include "Cxx/CxxScannerTokens.h"

#include <algorithm>
#include <set>
#include <wx/tokenzr.h>

bool IsWordCharA(char c, int strSize)
{
    if(strSize) {
        return ((c >= 97 && c <= 122) || // a-z
                (c >= 65 && c <= 90) ||  // A-Z
                (c >= 48 && c <= 57) ||  // 0-9
                (c == '_'));

    } else {
        return ((c >= 97 && c <= 122) || // a-z
                (c >= 65 && c <= 90) ||  // A-Z
                (c == '_'));
    }
}

std::string ReplaceWordA(const std::string& str, const std::string& word, const std::string& replaceWith)
{
    char currChar;
    char nextChar;
    std::string currentWord;
    std::string output;

    output.reserve(str.length() * 2);

    for(size_t i = 0; i < str.length(); i++) {
        // Look ahead
        if(str.length() > i + 1) {
            nextChar = str[i + 1];
        } else {
            // we are at the end of buffer
            nextChar = '\0';
        }

        currChar = str[i];
        if(!IsWordCharA(currChar, currentWord.length())) {
            output += str[i];
            currentWord.clear();

        } else {

            currentWord += currChar;
            if(IsWordCharA(nextChar, currentWord.length())) {
                // do nothing

            } else if(!IsWordCharA(nextChar, currentWord.length()) && currentWord == word) {
                output += replaceWith;
                currentWord.clear();

            } else {
                output += currentWord;
                currentWord.clear();
            }
        }
    }
    return output;
}

void PPToken::print(wxFFile& fp)
{
    wxString buff;
    buff << name << wxT("(") << (flags & IsFunctionLike) << wxT(")") << wxT("=") << replacement << wxT("\n");
    fp.Write(buff);
}

bool
PPToken::readInitList(const std::string& in, size_t from, std::string& initList, std::vector<std::string>& initListArr)
{
    if(in.length() < from) {
        return false;
    }

    std::string tmpString = in.substr(from);
    size_t start = tmpString.find('(');
    if(start == std::string::npos) {
        return false;
    }

    // skip the open brace
    tmpString = tmpString.substr(start + 1);

    for(size_t i = 0; i < start; i++) {
        initList += " ";
    }

    initList += "(";
    std::string word;
    int depth(1);

    for(size_t i = 0; i < tmpString.length(); i++) {
        char ch = tmpString[i];
        initList += ch;
        switch(ch) {
        case ')':
            depth--;
            if(depth == 0) {
                initListArr.push_back(word);
                return true;
            } else {
                word += ch;
            }
            break;
        case '(':
            depth++;
            word += ch;
            break;
        case ',':
            if(depth == 1) {
                initListArr.push_back(word);
                word.clear();
            } else {
                word += ch;
            }
            break;
        default:
            word += ch;
            break;
        }
    }
    return false;
}

bool PPToken::readInitList(const wxString& in, int from, wxString& initList, wxArrayString& initListArr)
{
    // sanity
    if(in.length() > 100) return false;
    
    if((int)in.Length() < from) {
        return false;
    }

    wxString tmpString = in.Mid(from);
    int start = tmpString.Find(wxT("("));
    if(start == wxNOT_FOUND) {
        return false;
    }
    tmpString = tmpString.Mid(start + 1);

    for(size_t i = 0; i < (size_t)start; i++) {
        initList << wxT(" ");
    }

    initList << wxT("(");
    wxString word;
    int depth(1);

    for(size_t i = 0; i < tmpString.Length(); i++) {
        wxChar ch = tmpString[i];
        initList << ch;
        switch(ch) {
        case wxT(')'):
            depth--;
            if(depth == 0) {
                initListArr.Add(word);
                return true;
            } else {
                word << ch;
            }
            break;
        case wxT('('):
            depth++;
            word << ch;
            break;
        case wxT(','):
            if(depth == 1) {
                initListArr.Add(word);
                word.Clear();
            } else {
                word << ch;
            }
            break;
        default:
            word << ch;
            break;
        }
    }
    return false;
}

void PPToken::expandOnce(const wxArrayString& initList)
{
    if(initList.size() != args.size()) return;

    for(size_t i = 0; i < args.size(); i++) {
        wxString placeHolder;
        placeHolder << wxT("%") << i;

        wxString replaceWith = initList.Item(i);
        replaceWith.Trim().Trim(false);

        if(replaceWith.Contains(placeHolder)) continue;

        replacement.Replace(placeHolder, initList.Item(i));
    }
}

///////////////////////////////////////////////////
std::string replacement;

bool CLReplacePatternA(const std::string& in, const CLReplacement& repl, std::string& outStr)
{
    if(repl.is_compound) {
        size_t where = in.find(repl.searchFor);
        if(where == std::string::npos) return false;

        std::string initList;
        std::vector<std::string> initListArr;
        if(PPToken::readInitList(in, repl.searchFor.length() + where, initList, initListArr) == false) return false;

        // update the 'replacement' with the actual values ( replace %0..%n)
        replacement = repl.replaceWith;
        char placeHolder[4];
        for(size_t i = 0; i < initListArr.size(); i++) {

            memset(placeHolder, 0, sizeof(placeHolder));
            sprintf(placeHolder, "%%%d", (int)i);

            size_t pos = replacement.find(placeHolder);
            const std::string& init = initListArr[i];
            while(pos != std::string::npos) {
                replacement.replace(pos, strlen(placeHolder), init.c_str());

                // search for the next match
                pos = replacement.find(placeHolder, pos + 1);
            }
        }

        outStr = in;
        where = outStr.find(repl.searchFor);
        if(where == std::string::npos) return false;

        outStr.replace(where, repl.searchFor.length() + initList.length(), replacement);
        return true;

    } else {

        size_t where = in.find(repl.searchFor);
        if(where == std::string::npos) {
            return false;
        }

        outStr = ReplaceWordA(in, repl.searchFor, repl.replaceWith);

        //		outStr = in;
        //		outStr.replace(where, repl.searchFor.length(), repl.replaceWith);

        // simple replacement
        return outStr != in;
    }
}

void CLReplacement::construct(const std::string& pattern, const std::string& replacement)
{
    is_ok = true;
    full_pattern = pattern;
    is_compound = full_pattern.find("%0") != std::string::npos;
    if(is_compound) {
        // a patterened expression
        replaceWith = replacement;
        size_t where = pattern.find('(');
        if(where == std::string::npos) {
            is_ok = false;
            return;
        }

        searchFor = pattern.substr(0, where);
        if(searchFor.empty()) {
            is_ok = false;
            return;
        }

    } else {
        // simple Key=Value pair
        replaceWith = replacement;
        searchFor = full_pattern;
    }
}
