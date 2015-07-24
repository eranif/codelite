//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_output_parser_api.h
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

#ifndef CLANG_OUTPUT_PARSER_API_H
#define CLANG_OUTPUT_PARSER_API_H

#include <string>
#include <vector>

struct ClangEntry {
    enum {
        TypeUnknown = -1,
        TypeEnum,     // 0
        TypeMethod,   // 1
        TypeCtor,     // 2
        TypeDtor,     // 3
        TypeVariable, // 4
        TypeClass
    };

    std::string name;
    std::string return_value;
    std::string signature; // function's signature
    std::string parent;
    std::string type_name; // variable type
    int type;
    std::string func_suffix;
    std::string tmp;

    void reset()
    {
        name.clear();
        return_value.clear();
        signature.clear();
        parent.clear();
        type_name.clear();
        tmp.clear();
        func_suffix.clear();
        type = TypeUnknown;
    }

    std::string pattern() const
    {
        std::string p;
        p += "/^ ";
        if(return_value.empty() == false) {
            p += return_value;
            p += " ";
        }

        if(type_name.empty() == false) {
            p += type_name;
            p += " ";
        }

        if(parent.empty() == false) {
            p += parent;
            p += "::";
        }

        if(name.empty() == false) {
            p += name;
        }

        if(signature.empty() == false) {
            p += signature;
        }

        if(func_suffix.empty() == false) {
            p += " ";
            p += func_suffix;
        }
        p += " $/";
        return p;
    }

    void print() const
    {
        printf("----\n");
        printf("name        : %s\n", name.c_str());
        printf("return value: %s\n", return_value.c_str());
        printf("signature   : %s\n", signature.c_str());
        printf("parent      : %s\n", parent.c_str());
        printf("type        : %d\n", type);
        printf("type_name   : %s\n", type_name.c_str());
        printf("func_suffix : %s\n", func_suffix.c_str());
        printf("pattern     : %s\n", pattern().c_str());
    }
};
typedef std::vector<ClangEntry> ClangEntryVector;

extern void clang_parse_string(const std::string& str);
extern const ClangEntryVector& clang_results();

#endif // CLANG_OUTPUT_PARSER_API_H
