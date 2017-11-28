//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_data.cpp
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
#include "precompiled_header.h"
#include "istorage.h"
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include "tags_options_data.h"
#include <set>
#include "cl_config.h"
#include "ctags_manager.h"

wxString TagsOptionsData::CLANG_CACHE_LAZY = "Lazy";
wxString TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD = "On File Load";

size_t TagsOptionsData::CURRENT_VERSION = 7100;

static bool _IsValidCppIndetifier(const wxString& id)
{
    if(id.IsEmpty()) {
        return false;
    }
    // first char can be only _A-Za-z
    wxString first(id.Mid(0, 1));
    if(first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos) {
        return false;
    }
    // make sure that rest of the id contains only a-zA-Z0-9_
    if(id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
        return false;
    }
    return true;
}

static bool _IsCppKeyword(const wxString& word)
{
    static wxStringSet_t words;
    if(words.empty()) {
        TagsManager::GetCXXKeywords(words);
    }
    return words.count(word);
}

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData()
    : clConfigItem("code-completion")
    , m_ccFlags(CC_DISP_FUNC_CALLTIP | CC_CPP_KEYWORD_ASISST | CC_COLOUR_VARS | CC_ACCURATE_SCOPE_RESOLVING |
                CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING | CC_WORD_ASSIST)
    , m_ccColourFlags(CC_COLOUR_DEFAULT)
    , m_fileSpec(wxT("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++"))
    , m_minWordLen(3)
    , m_parserEnabled(true)
    , m_maxItemToColour(1000)
#ifdef __WXMSW__
    , m_macrosFiles(wxT("_mingw.h bits/c++config.h"))
#elif defined(__WXMAC__)
    , m_macrosFiles(wxT("sys/cdefs.h bits/c++config.h AvailabilityMacros.h"))
#else
    , m_macrosFiles(wxT("sys/cdefs.h bits/c++config.h"))
#endif
    , m_clangOptions(0)
    , m_clangBinary(wxT(""))
    , m_clangCachePolicy(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD)
    , m_ccNumberOfDisplayItems(500)
    , m_version(0)
{
    // Initialize defaults
    m_languages.Add(wxT("C++"));
    m_tokens.Add(wxT("EXPORT"));
    m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_VERSION"));
    m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE_VERSION"));
    m_tokens.Add(wxT("_GLIBCXX_NOEXCEPT"));
    m_tokens.Add(wxT("_NOEXCEPT"));
    m_tokens.Add(wxT("WXDLLIMPEXP_CORE"));
    m_tokens.Add(wxT("WXDLLIMPEXP_BASE"));
    m_tokens.Add(wxT("WXDLLIMPEXP_XML"));
    m_tokens.Add(wxT("WXDLLIMPEXP_XRC"));
    m_tokens.Add(wxT("WXDLLIMPEXP_ADV"));
    m_tokens.Add(wxT("WXDLLIMPEXP_AUI"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_ADV"));
    m_tokens.Add(wxT("WXDLLIMPEXP_CL"));
    m_tokens.Add(wxT("WXDLLIMPEXP_LE_SDK"));
    m_tokens.Add(wxT("WXDLLIMPEXP_SQLITE3"));
    m_tokens.Add(wxT("WXDLLIMPEXP_SCI"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_AUI"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_PROPGRID"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_XML"));
    m_tokens.Add(wxT("WXMAKINGDLL"));
    m_tokens.Add(wxT("WXUSINGDLL"));
    m_tokens.Add(wxT("_CRTIMP"));
    m_tokens.Add(wxT("__CRT_INLINE"));
    m_tokens.Add(wxT("__cdecl"));
    m_tokens.Add(wxT("__stdcall"));
    m_tokens.Add(wxT("WXDLLEXPORT"));
    m_tokens.Add(wxT("WXDLLIMPORT"));
    m_tokens.Add(wxT("__MINGW_ATTRIB_PURE"));
    m_tokens.Add(wxT("__MINGW_ATTRIB_MALLOC"));
    m_tokens.Add(wxT("__GOMP_NOTHROW"));
    m_tokens.Add(wxT("wxT"));
    m_tokens.Add(wxT("SCI_SCOPE(%0)=%0"));
    m_tokens.Add(wxT("WINBASEAPI"));
    m_tokens.Add(wxT("WINAPI"));
    m_tokens.Add(wxT("__nonnull"));
    m_tokens.Add(wxT("noexcept"));
    m_tokens.Add(wxT("wxOVERRIDE"));
    m_tokens.Add(wxT("override"));
    m_tokens.Add("final"); // ignore "final" keyword
#if defined(__WXGTK__)
    m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowGTK"));
    m_tokens.Add(wxT("wxWindow=wxWindowGTK"));

#elif defined(__WXMSW__)
    m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowMSW"));
    m_tokens.Add(wxT("wxWindow=wxWindowMSW"));
#else
    m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowMac"));
    m_tokens.Add(wxT("wxWindow=wxWindowMac"));
#endif
    m_tokens.Add(wxT("wxWindowNative=wxWindowBase"));
    m_tokens.Add(wxT("wxStatusBar=wxStatusBarBase"));
    m_tokens.Add(wxT("BEGIN_DECLARE_EVENT_TYPES()=enum {"));
    m_tokens.Add(wxT("END_DECLARE_EVENT_TYPES()=};"));
    m_tokens.Add(wxT("DECLARE_EVENT_TYPE"));
    m_tokens.Add(wxT("DECLARE_EXPORTED_EVENT_TYPE"));
    m_tokens.Add(wxT("WXUNUSED(%0)=%0"));
    m_tokens.Add(wxT("wxDEPRECATED(%0)=%0"));
    m_tokens.Add(wxT("_T"));
    m_tokens.Add(wxT("ATTRIBUTE_PRINTF_1"));
    m_tokens.Add(wxT("ATTRIBUTE_PRINTF_2"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_BASE"));
    m_tokens.Add(wxT("WXDLLIMPEXP_FWD_CORE"));
    m_tokens.Add(wxT("DLLIMPORT"));
    m_tokens.Add(wxT("DECLARE_INSTANCE_TYPE"));
    m_tokens.Add(wxT("emit"));
    m_tokens.Add(wxT("Q_OBJECT"));
    m_tokens.Add(wxT("Q_PACKED"));
    m_tokens.Add(wxT("Q_GADGET"));
    m_tokens.Add(wxT("QT_BEGIN_NAMESPACE"));
    m_tokens.Add(wxT("QT_END_NAMESPACE"));
    m_tokens.Add(wxT("Q_GADGET"));
    m_tokens.Add(wxT("QT_BEGIN_HEADER"));
    m_tokens.Add(wxT("QT_END_HEADER"));
    m_tokens.Add(wxT("Q_REQUIRED_RESULT"));
    m_tokens.Add(wxT("Q_INLINE_TEMPLATE"));
    m_tokens.Add(wxT("Q_OUTOFLINE_TEMPLATE"));
    m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE(%0)=namespace %0{"));
    m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE=}"));
    m_tokens.Add(wxT("_GLIBCXX_BEGIN_NESTED_NAMESPACE(%0, %1)=namespace %0{"));
    m_tokens.Add(wxT("wxDECLARE_EXPORTED_EVENT(%0,%1,%2)=int %1;"));
    m_tokens.Add(wxT("wxDECLARE_EVENT(%0,%1)=int %0;"));
    m_tokens.Add(wxT("BOOST_FOREACH(%0, %1)=%0;"));
    m_tokens.Add(wxT("DECLARE_EVENT_TYPE(%0,%1)=int %0;"));
    m_tokens.Add(wxT("_GLIBCXX_END_NESTED_NAMESPACE=}"));
    m_tokens.Add(wxT("_GLIBCXX_VISIBILITY(%0)"));
    m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_TR1=namespace tr1{"));
    m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE_TR1=}"));
    m_tokens.Add(wxT("_GLIBCXX_STD=std"));
    m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_CONTAINER"));
    m_tokens.Add(wxT("__const=const"));
    m_tokens.Add(wxT("__restrict"));
    m_tokens.Add(wxT("__THROW"));
    m_tokens.Add(wxT("__wur"));
    m_tokens.Add(wxT("_STD_BEGIN=namespace std{"));
    m_tokens.Add(wxT("_STD_END=}"));
    m_tokens.Add(wxT("__CLRCALL_OR_CDECL"));
    m_tokens.Add(wxT("_CRTIMP2_PURE"));
    m_tokens.Add(wxT("_GLIBCXX_CONST"));
    m_tokens.Add(wxT("_GLIBCXX_CONSTEXPR"));
    m_tokens.Add(wxT("_GLIBCXX_NORETURN"));
    m_tokens.Add(wxT("_GLIBCXX_NOTHROW"));
    m_tokens.Add(wxT("_GLIBCXX_PURE"));
    m_tokens.Add(wxT("_GLIBCXX_THROW(%0)"));
    m_tokens.Add(wxT("_GLIBCXX_DEPRECATED"));
    m_tokens.Add("LLDB_API");
    m_tokens.Add("PYTHON_API");
    m_tokens.Add("__cpp_deduction_guides=0");
    
    // libcpp macros
    m_tokens.Add("_LIBCPP_TYPE_VIS_ONLY");
    m_tokens.Add("_LIBCPP_CONSTEXPR");
    m_tokens.Add("_LIBCPP_CONSTEXPR_AFTER_CXX11");
    m_tokens.Add("_LIBCPP_INLINE_VISIBILITY");
    m_tokens.Add("_LIBCPP_BEGIN_NAMESPACE_STD=namespace std{");
    m_tokens.Add("_LIBCPP_END_NAMESPACE_STD=}");
    m_types.Add(wxT("std::vector::reference=_Tp"));
    m_types.Add(wxT("std::vector::const_reference=_Tp"));
    m_types.Add(wxT("std::vector::iterator=_Tp"));
    m_types.Add(wxT("std::vector::const_iterator=_Tp"));
    m_types.Add(wxT("std::queue::reference=_Tp"));
    m_types.Add(wxT("std::queue::const_reference=_Tp"));
    m_types.Add(wxT("std::priority_queue::reference=_Tp"));
    m_types.Add(wxT("std::priority_queue::const_reference=_Tp"));
    m_types.Add(wxT("std::set::const_iterator=_Key"));
    m_types.Add(wxT("std::set::iterator=_Key"));
    m_types.Add(wxT("std::unordered_set::const_iterator=_Key"));
    m_types.Add(wxT("std::unordered_set::iterator=_Key"));
    m_types.Add(wxT("std::deque::reference=_Tp"));
    m_types.Add(wxT("std::deque::const_reference=_Tp"));
    m_types.Add(wxT("std::map::iterator=std::pair<_Key, _Tp>"));
    m_types.Add(wxT("std::map::const_iterator=std::pair<_Key,_Tp>"));
    m_types.Add(wxT("std::unordered_map::iterator=std::pair<_Key, _Tp>"));
    m_types.Add(wxT("std::unordered_map::mapped_type=_Tp"));
    m_types.Add(wxT("std::unordered_map::const_iterator=std::pair<_Key,_Tp>"));
    m_types.Add(wxT("std::unordered_map::value_type=std::pair<_Key,_Tp>"));
    m_types.Add(wxT("std::multimap::iterator=std::pair<_Key,_Tp>"));
    m_types.Add(wxT("std::multimap::const_iterator=std::pair<_Key,_Tp>"));
    m_types.Add(wxT("wxOrderedMap::iterator=std::pair<Key,Value>"));
    m_types.Add(wxT("wxOrderedMap::const_iterator=std::pair<Key,Value>"));
    m_types.Add(wxT("boost::shared_ptr::type=T"));
    m_types.Add(wxT("std::unique_ptr::pointer=_Tp"));

    DoUpdateTokensWxMap();
    DoUpdateTokensWxMapReversed();
}

TagsOptionsData::~TagsOptionsData() {}

wxString TagsOptionsData::ToString()
{
    wxString options(wxEmptyString);

    static wxString file_name;
    wxString file_content;

    if(file_name.IsEmpty()) {
        char* ctagsReplacement = getenv("CTAGS_REPLACEMENTS");
        if(ctagsReplacement) {
            file_name = wxString(ctagsReplacement, wxConvUTF8).c_str();
        }
    }

    DoUpdateTokensWxMap();
    const wxStringTable_t& tokensMap = GetTokensWxMap();
    wxStringTable_t::const_iterator iter = tokensMap.begin();

    if(tokensMap.empty() == false) {
        for(; iter != tokensMap.end(); ++iter) {
            if(!iter->second.IsEmpty() || (iter->second.IsEmpty() && iter->first.Find(wxT("%0")) != wxNOT_FOUND)) {
                // Key = Value pair. Place this one in the output file
                file_content << iter->first << wxT("=") << iter->second << wxT("\n");
            } else {

                if(options.IsEmpty()) options = wxT(" -I");

                options << iter->first;
                options << wxT(",");
            }
        }

        if(options.IsEmpty() == false) options.RemoveLast();

        options += wxT(" ");
    }

    // write the file content
    if(file_name.IsEmpty() == false) {
        wxFFile fp(file_name, wxT("w+b"));
        if(fp.IsOpened()) {
            fp.Write(file_content);
            fp.Close();
        }
    }

    if(GetLanguages().IsEmpty() == false) {
        options += wxT(" --language-force=");
        options += GetLanguages().Item(0);
        options += wxT(" ");
    }
    return options;
}

void TagsOptionsData::SetLanguageSelection(const wxString& lang)
{
    int where = m_languages.Index(lang);
    if(where != wxNOT_FOUND) {
        m_languages.RemoveAt(where);
    }
    m_languages.Insert(lang, 0);
}

std::map<std::string, std::string> TagsOptionsData::GetTokensMap() const
{
    std::map<std::string, std::string> tokens;
    for(size_t i = 0; i < m_tokens.GetCount(); i++) {
        // const wxCharBuffer bufKey = _C(
        wxString item = m_tokens.Item(i);
        item.Trim().Trim(false);
        wxString k = item.BeforeFirst(wxT('='));
        wxString v = item.AfterFirst(wxT('='));

        const wxCharBuffer bufKey = _C(k);
        std::string key = bufKey.data();
        std::string value;
        if(!v.empty()) {
            const wxCharBuffer bufValue = _C(v);
            value = bufValue.data();
        }
        tokens[key] = value;
    }
    return tokens;
}

const wxStringTable_t& TagsOptionsData::GetTokensWxMap() const { return m_tokensWxMap; }

wxStringTable_t TagsOptionsData::GetTypesMap() const
{
    wxStringTable_t tokens;
    for(size_t i = 0; i < m_types.GetCount(); i++) {
        wxString item = m_types.Item(i);
        item.Trim().Trim(false);
        wxString k = item.BeforeFirst(wxT('='));
        wxString v = item.AfterFirst(wxT('='));
        tokens[k] = v;
    }
    return tokens;
}

void TagsOptionsData::SetTokens(const wxString& tokens)
{
    this->m_tokens = ::wxStringTokenize(tokens, "\r\n", wxTOKEN_STRTOK);
    DoUpdateTokensWxMapReversed();
    DoUpdateTokensWxMap();
}

void TagsOptionsData::DoUpdateTokensWxMap()
{
    m_tokensWxMap.clear();
    for(size_t i = 0; i < m_tokens.GetCount(); i++) {
        wxString item = m_tokens.Item(i).Trim().Trim(false);
        wxString k = item.BeforeFirst(wxT('='));
        wxString v = item.AfterFirst(wxT('='));
        m_tokensWxMap[k] = v;
    }
}

void TagsOptionsData::DoUpdateTokensWxMapReversed()
{
    m_tokensWxMapReversed.clear();
    for(size_t i = 0; i < m_tokens.GetCount(); i++) {
        wxString item = m_tokens.Item(i).Trim().Trim(false);
        wxString k = item.AfterFirst(wxT('='));
        wxString v = item.BeforeFirst(wxT('='));
        if(_IsValidCppIndetifier(k) && !_IsCppKeyword(k)) {
            m_tokensWxMapReversed[k] = v;
        }
    }
}

const wxStringTable_t& TagsOptionsData::GetTokensReversedWxMap() const { return m_tokensWxMapReversed; }

void TagsOptionsData::FromJSON(const JSONElement& json)
{
    m_version = json.namedObject("version").toSize_t();
    m_ccFlags = json.namedObject(wxT("m_ccFlags")).toSize_t(m_ccFlags);
    m_ccColourFlags = json.namedObject(wxT("m_ccColourFlags")).toSize_t(m_ccColourFlags);
    m_tokens = json.namedObject(wxT("m_tokens")).toArrayString();
    m_types = json.namedObject(wxT("m_types")).toArrayString();
    m_fileSpec = json.namedObject(wxT("m_fileSpec")).toString(m_fileSpec);
    m_languages = json.namedObject(wxT("m_languages")).toArrayString();
    m_minWordLen = json.namedObject(wxT("m_minWordLen")).toInt(m_minWordLen);
    m_parserSearchPaths = json.namedObject(wxT("m_parserSearchPaths")).toArrayString();
    m_parserEnabled = json.namedObject(wxT("m_parserEnabled")).toBool();
    m_parserExcludePaths = json.namedObject(wxT("m_parserExcludePaths")).toArrayString();
    m_maxItemToColour = json.namedObject(wxT("m_maxItemToColour")).toInt();
    m_macrosFiles = json.namedObject(wxT("m_macrosFiles")).toString();
    m_clangOptions = json.namedObject(wxT("m_clangOptions")).toSize_t();
    m_clangBinary = json.namedObject(wxT("m_clangBinary")).toString();
    m_clangCmpOptions = json.namedObject(wxT("m_clangCmpOptions")).toString();
    m_clangSearchPaths = json.namedObject(wxT("m_clangSearchPaths")).toArrayString();
    m_clangMacros = json.namedObject(wxT("m_clangMacros")).toString();
    m_clangCachePolicy = json.namedObject(wxT("m_clangCachePolicy")).toString();
    m_ccNumberOfDisplayItems = json.namedObject(wxT("m_ccNumberOfDisplayItems")).toSize_t(m_ccNumberOfDisplayItems);

    if(!m_fileSpec.Contains("*.hxx")) {
        m_fileSpec = "*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++";
    }

    DoUpdateTokensWxMapReversed();
    DoUpdateTokensWxMap();
    m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;
}

JSONElement TagsOptionsData::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("version", m_version);
    json.addProperty("m_ccFlags", m_ccFlags);
    json.addProperty("m_ccColourFlags", m_ccColourFlags);
    json.addProperty("m_tokens", m_tokens);
    json.addProperty("m_types", m_types);
    json.addProperty("m_fileSpec", m_fileSpec);
    json.addProperty("m_languages", m_languages);
    json.addProperty("m_minWordLen", m_minWordLen);
    json.addProperty("m_parserSearchPaths", m_parserSearchPaths);
    json.addProperty("m_parserEnabled", m_parserEnabled);
    json.addProperty("m_parserExcludePaths", m_parserExcludePaths);
    json.addProperty("m_maxItemToColour", m_maxItemToColour);
    json.addProperty("m_macrosFiles", m_macrosFiles);
    json.addProperty("m_clangOptions", m_clangOptions);
    json.addProperty("m_clangBinary", m_clangBinary);
    json.addProperty("m_clangCmpOptions", m_clangCmpOptions);
    json.addProperty("m_clangSearchPaths", m_clangSearchPaths);
    json.addProperty("m_clangMacros", m_clangMacros);
    json.addProperty("m_clangCachePolicy", m_clangCachePolicy);
    json.addProperty("m_ccNumberOfDisplayItems", m_ccNumberOfDisplayItems);
    return json;
}

wxString TagsOptionsData::DoJoinArray(const wxArrayString& arr) const
{
    wxString s;
    for(size_t i = 0; i < arr.GetCount(); ++i)
        s << arr.Item(i) << "\n";

    if(s.IsEmpty() == false) s.RemoveLast();

    return s;
}

void TagsOptionsData::Merge(const TagsOptionsData& tod)
{
    clConfig conf;
    m_tokens = conf.MergeArrays(m_tokens, tod.m_tokens);
    m_types = conf.MergeArrays(m_types, tod.m_types);
    DoUpdateTokensWxMapReversed();
    DoUpdateTokensWxMap();
    if(m_version != TagsOptionsData::CURRENT_VERSION) {
        m_ccFlags |= CC_WORD_ASSIST;
        m_ccNumberOfDisplayItems = tod.m_ccNumberOfDisplayItems;
    }
    m_version = TagsOptionsData::CURRENT_VERSION;
}
