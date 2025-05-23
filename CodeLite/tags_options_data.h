//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_data.h
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
#ifndef TAGS_OPTIONS_DATA_H
#define TAGS_OPTIONS_DATA_H

#include "cl_config.h"
#include "codelite_exports.h"
#include "macros.h"
#include "wxStringHash.h"

#include <map>
#include <unordered_map>
#include <wx/filename.h>
#include <wx/tokenzr.h>

enum CodeCompletionOpts {
    CC_PARSE_COMMENTS = 0x00000001,
    CC_DISP_COMMENTS = 0x00000002,
    CC_DISP_TYPE_INFO = 0x00000004,
    CC_DISP_FUNC_CALLTIP = 0x00000008,
    CC_LOAD_EXT_DB = 0x00000010,
    CC_AUTO_INSERT_SINGLE_CHOICE = 0x00000020,
    CC_COLOUR_VARS = 0x00000080,
    CC_DISABLE_AUTO_PARSING = 0x00000800,
    CC_MARK_TAGS_FILES_IN_BOLD = 0x00001000,
    CC_RETAG_WORKSPACE_ON_STARTUP = 0x00004000,
    CC_ACCURATE_SCOPE_RESOLVING = 0x00008000,
    CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING = 0x00010000,
    CC_IS_CASE_SENSITIVE = 0x00020000,
    CC_BACKSPACE_TRIGGER = 0x00040000,
};

enum CodeCompletionColourOpts {
    CC_COLOUR_CLASS = 0x00000001,
    CC_COLOUR_STRUCT = 0x00000002,
    CC_COLOUR_FUNCTION = 0x00000004,
    CC_COLOUR_ENUM = 0x00000008,
    CC_COLOUR_UNION = 0x00000010,
    CC_COLOUR_PROTOTYPE = 0x00000020,
    CC_COLOUR_TYPEDEF = 0x00000040,
    CC_COLOUR_MACRO = 0x00000080,
    CC_COLOUR_NAMESPACE = 0x00000100,
    CC_COLOUR_ENUMERATOR = 0x00000200,
    CC_COLOUR_VARIABLE = 0x00000400,
    CC_COLOUR_MEMBER = 0x00000800,
    CC_COLOUR_MACRO_BLOCKS = 0x00001000,
    CC_COLOUR_ALL = CC_COLOUR_CLASS | CC_COLOUR_STRUCT | CC_COLOUR_FUNCTION | CC_COLOUR_ENUM | CC_COLOUR_UNION |
                    CC_COLOUR_PROTOTYPE | CC_COLOUR_TYPEDEF | CC_COLOUR_MACRO | CC_COLOUR_NAMESPACE |
                    CC_COLOUR_ENUMERATOR | CC_COLOUR_VARIABLE | CC_COLOUR_MEMBER,
    CC_COLOUR_DEFAULT = CC_COLOUR_CLASS | CC_COLOUR_STRUCT | CC_COLOUR_NAMESPACE | CC_COLOUR_ENUM | CC_COLOUR_TYPEDEF
};

enum CodeCompletionClangOptions {
    CC_CLANG_ENABLED = (1 << 0),
    CC_CLANG_FIRST = (1 << 1),
    CC_CTAGS_ENABLED = (1 << 2),
    CC_CLANG_INLINE_ERRORS = (1 << 3),
};

class WXDLLIMPEXP_CL TagsOptionsData : public clConfigItem
{
protected:
    size_t m_ccFlags;
    size_t m_ccColourFlags;
    wxArrayString m_tokens;
    wxArrayString m_types;
    wxString m_fileSpec;
    wxArrayString m_languages;
    int m_minWordLen;
    wxArrayString m_parserSearchPaths;
    wxArrayString m_parserExcludePaths;
    bool m_parserEnabled;
    int m_maxItemToColour;
    wxStringTable_t m_tokensWxMap;
    wxStringTable_t m_tokensWxMapReversed;
    wxString m_macrosFiles;
    size_t m_clangOptions;
    wxString m_clangBinary;
    wxString m_clangCmpOptions;
    wxArrayString m_clangSearchPaths;
    wxString m_clangMacros;
    wxString m_clangCachePolicy;
    size_t m_ccNumberOfDisplayItems;
    size_t m_version;

public:
    static size_t CURRENT_VERSION;

protected:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void DoUpdateTokensWxMap();
    void DoUpdateTokensWxMapReversed();
    wxString DoJoinArray(const wxArrayString& arr) const;

public:
    static wxString CLANG_CACHE_ON_FILE_LOAD;
    static const std::vector<wxString>& GetDefaultTokens();
    static const std::vector<wxString>& GetDefaultTypes();

public:
    TagsOptionsData();
    virtual ~TagsOptionsData();

    void AddDefaultTokens();

    void Merge(const TagsOptionsData& tod);
    void SetVersion(size_t version) { this->m_version = version; }

    // setters/getters
    void SetCcNumberOfDisplayItems(size_t ccNumberOfDisplayItems)
    {
        this->m_ccNumberOfDisplayItems = ccNumberOfDisplayItems;
    }
    size_t GetCcNumberOfDisplayItems() const { return m_ccNumberOfDisplayItems; }

    const wxArrayString& GetClangSearchPathsArray() const { return m_clangSearchPaths; }

    void SetClangSearchPathsArray(const wxArrayString& paths) { m_clangSearchPaths = paths; }

    void SetClangOptions(size_t clangOptions) { this->m_clangOptions = clangOptions; }
    size_t GetClangOptions() const { return m_clangOptions; }

    void SetFlags(size_t flags) { m_ccFlags = flags; }

    void SetFileSpec(const wxString& filespec) { m_fileSpec = filespec; }

    void SetLanguages(const wxArrayString& langs) { m_languages = langs; }
    const wxArrayString& GetLanguages() const { return m_languages; }

    void SetTokens(const wxString& tokens);
    void SetTypes(const wxString& types) { this->m_types = ::wxStringTokenize(types, "\r\n", wxTOKEN_STRTOK); }
    wxString GetTokens() const { return DoJoinArray(m_tokens); }

    std::map<std::string, std::string> GetTokensMap() const;

    const wxStringTable_t& GetTokensWxMap() const;

    size_t GetFlags() const { return m_ccFlags; }
    const wxString& GetFileSpec() const { return m_fileSpec; }

    // Setters
    void SetMinWordLen(int minWordLen) { this->m_minWordLen = minWordLen; }
    // Getters
    int GetMinWordLen() const { return m_minWordLen; }

    // Serialization API

    wxString ToString() const;

    /**
     * @brief Sync the data stored in this object with the file system
     */
    void SyncData();

    void SetCcColourFlags(size_t ccColourFlags) { this->m_ccColourFlags = ccColourFlags; }

    size_t GetCcColourFlags() const { return m_ccColourFlags; }
    void SetParserSearchPaths(const wxArrayString& parserSearchPaths) { this->m_parserSearchPaths = parserSearchPaths; }
    const wxArrayString& GetParserSearchPaths() const { return m_parserSearchPaths; }
    void SetParserExcludePaths(const wxArrayString& parserExcludePaths)
    {
        this->m_parserExcludePaths = parserExcludePaths;
    }
    const wxArrayString& GetParserExcludePaths() const { return m_parserExcludePaths; }
};

#endif // TAGS_OPTIONS_DATA_H
