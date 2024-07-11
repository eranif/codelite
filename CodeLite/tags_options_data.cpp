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
#include "tags_options_data.h"

#include "cl_config.h"
#include "ctags_manager.h"
#include "database/istorage.h"
#include "precompiled_header.h"

#include <set>
#include <wx/ffile.h>
#include <wx/tokenzr.h>

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
    if(first.find_first_not_of("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != wxString::npos) {
        return false;
    }
    // make sure that rest of the id contains only a-zA-Z0-9_
    if(id.find_first_not_of("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != wxString::npos) {
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

thread_local std::vector<wxString> DEFAULT_TOKENS = {
    "ATTRIBUTE_PRINTF_1",
    "ATTRIBUTE_PRINTF_2",
    "BEGIN_DECLARE_EVENT_TYPES()=enum {",
    "BOOST_FOREACH(%0, %1)=%0;",
    "DECLARE_EVENT_TYPE",
    "DECLARE_EVENT_TYPE(%0,%1)=int %0;",
    "DECLARE_EXPORTED_EVENT_TYPE",
    "DECLARE_INSTANCE_TYPE",
    "DLLIMPORT",
    "END_DECLARE_EVENT_TYPES()=};",
    "EXPORT",
    "LLDB_API",
    "PYTHON_API",
    "QT_BEGIN_HEADER",
    "QT_BEGIN_NAMESPACE",
    "QT_END_HEADER",
    "QT_END_NAMESPACE",
    "Q_GADGET",
    "Q_INLINE_TEMPLATE",
    "Q_OBJECT",
    "Q_OUTOFLINE_TEMPLATE",
    "Q_PACKED",
    "Q_REQUIRED_RESULT",
    "SCI_SCOPE(%0)=%0",
    "UNALIGNED",
    "WINAPI",
    "WINBASEAPI",
    "WXDLLEXPORT",
    "WXDLLIMPEXP_ADV",
    "WXDLLIMPEXP_AUI",
    "WXDLLIMPEXP_BASE",
    "WXDLLIMPEXP_CL",
    "WXDLLIMPEXP_SDK",
    "WXDLLIMPEXP_CORE",
    "WXDLLIMPEXP_FWD_ADV",
    "WXDLLIMPEXP_FWD_AUI",
    "WXDLLIMPEXP_FWD_BASE",
    "WXDLLIMPEXP_FWD_CORE",
    "WXDLLIMPEXP_FWD_PROPGRID",
    "WXDLLIMPEXP_FWD_XML",
    "WXDLLIMPEXP_LE_SDK",
    "WXDLLIMPEXP_SCI",
    "WXDLLIMPEXP_SQLITE3",
    "WXDLLIMPEXP_XML",
    "WXDLLIMPEXP_XRC",
    "WXDLLIMPORT",
    "WXMAKINGDLL",
    "WXUNUSED(%0)=%0",
    "WXUSINGDLL",
    "_ALIGNAS(%0)=alignas(%0)",
    "_ALIGNAS_TYPE(%0)=alignas(%0)",
    "_ANONYMOUS_STRUCT",
    "_ANONYMOUS_UNION",
    "_ATTRIBUTE(%0)",
    "_CRTIMP",
    "_CRTIMP2",
    "_CRTIMP2_PURE",
    "_CRTIMP_ALTERNATIVE",
    "_CRTIMP_NOIA64",
    "_CRTIMP_PURE",
    "_CRT_ALIGN(%0)",
    "_CRT_DEPRECATE_TEXT(%0)",
    "_CRT_INSECURE_DEPRECATE_GLOBALS(%0)",
    "_CRT_INSECURE_DEPRECATE_MEMORY(%0)",
    "_CRT_OBSOLETE(%0)",
    "_CRT_STRINGIZE(%0)=\"%0\"",
    "_CRT_UNUSED(%0)=%0",
    "_CRT_WIDE(%0)=L\"%0\"",
    "_GLIBCXX14_CONSTEXPR",
    "_GLIBCXX17_CONSTEXPR",
    "_GLIBCXX17_DEPRECATED",
    "_GLIBCXX17_INLINE",
    "_GLIBCXX20_CONSTEXPR",
    "_GLIBCXX20_DEPRECATED(%0)",
    "_GLIBCXX_BEGIN_EXTERN_C=extern \"C\" {",
    "_GLIBCXX_BEGIN_NAMESPACE(%0)=namespace %0{",
    "_GLIBCXX_BEGIN_NAMESPACE_ALGO",
    "_GLIBCXX_BEGIN_NAMESPACE_CONTAINER",
    "_GLIBCXX_BEGIN_NAMESPACE_CXX11",
    "_GLIBCXX_BEGIN_NAMESPACE_LDBL",
    "_GLIBCXX_BEGIN_NAMESPACE_LDBL_OR_CXX11",
    "_GLIBCXX_BEGIN_NAMESPACE_TR1=namespace tr1{",
    "_GLIBCXX_BEGIN_NAMESPACE_VERSION",
    "_GLIBCXX_BEGIN_NESTED_NAMESPACE(%0, %1)=namespace %0{",
    "_GLIBCXX_CONST",
    "_GLIBCXX_CONSTEXPR",
    "_GLIBCXX_DEPRECATED",
    "_GLIBCXX_DEPRECATED_SUGGEST(%0)",
    "_GLIBCXX_END_EXTERN_C=}",
    "_GLIBCXX_END_NAMESPACE=}",
    "_GLIBCXX_END_NAMESPACE_ALGO",
    "_GLIBCXX_END_NAMESPACE_CONTAINER",
    "_GLIBCXX_END_NAMESPACE_CXX11",
    "_GLIBCXX_END_NAMESPACE_LDBL",
    "_GLIBCXX_END_NAMESPACE_LDBL_OR_CXX11",
    "_GLIBCXX_END_NAMESPACE_TR1=}",
    "_GLIBCXX_END_NAMESPACE_VERSION",
    "_GLIBCXX_END_NESTED_NAMESPACE=}",
    "_GLIBCXX_NAMESPACE_CXX11",
    "_GLIBCXX_NAMESPACE_LDBL",
    "_GLIBCXX_NAMESPACE_LDBL_OR_CXX11",
    "_GLIBCXX_NODISCARD",
    "_GLIBCXX_NOEXCEPT",
    "_GLIBCXX_NOEXCEPT_IF(%0)",
    "_GLIBCXX_NOEXCEPT_PARM",
    "_GLIBCXX_NOEXCEPT_QUAL",
    "_GLIBCXX_NORETURN",
    "_GLIBCXX_NOTHROW",
    "_GLIBCXX_PSEUDO_VISIBILITY(%0)",
    "_GLIBCXX_PURE",
    "_GLIBCXX_STD=std",
    "_GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(%0)",
    "_GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(%0)",
    "_GLIBCXX_THROW(%0)",
    "_GLIBCXX_THROW_OR_ABORT(%0)",
    "_GLIBCXX_TXN_SAFE",
    "_GLIBCXX_TXN_SAFE_DYN",
    "_GLIBCXX_USE_CONSTEXPR",
    "_GLIBCXX_USE_NOEXCEPT",
    "_GLIBCXX_VISIBILITY(%0)",
    "_LIBCPP_HIDE_FROM_ABI",
    "_LIBCPP_ALWAYS_INLINE",
    "_LIBCPP_BEGIN_NAMESPACE_FILESYSTEM=namespace std { namespace filesystem {",
    "_LIBCPP_BEGIN_NAMESPACE_STD=namespace std{",
    "_LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS",
    "_LIBCPP_CONCAT(%0,%1)=%0%1",
    "_LIBCPP_CONCAT1(%0,%1)=%0%1",
    "_LIBCPP_CONSTEVAL",
    "_LIBCPP_CONSTEXPR",
    "_LIBCPP_CONSTEXPR_AFTER_CXX11",
    "_LIBCPP_CONSTEXPR_AFTER_CXX14",
    "_LIBCPP_CONSTEXPR_AFTER_CXX17",
    "_LIBCPP_CONSTEXPR_IF_NODEBUG",
    "_LIBCPP_CRT_FUNC",
    "_LIBCPP_PUSH_MACROS",
    "_LIBCPP_DECLARE_STRONG_ENUM(%0)=enum class %0",
    "_LIBCPP_DECLARE_STRONG_ENUM_EPILOG(%0)",
    "_LIBCPP_DECLSPEC_EMPTY_BASES",
    "_LIBCPP_DEFAULT",
    "_LIBCPP_DEPRECATED",
    "_LIBCPP_DEPRECATED_IN_CXX11",
    "_LIBCPP_DEPRECATED_IN_CXX14",
    "_LIBCPP_DEPRECATED_IN_CXX17",
    "_LIBCPP_DEPRECATED_IN_CXX20",
    "_LIBCPP_DEPRECATED_WITH_CHAR8_T",
    "_LIBCPP_DIAGNOSE_ERROR(%0)",
    "_LIBCPP_DIAGNOSE_WARNING(%0)",
    "_LIBCPP_DISABLE_EXTENSION_WARNING",
    "_LIBCPP_DLL_VIS",
    "_LIBCPP_END_NAMESPACE_FILESYSTEM=} }",
    "_LIBCPP_END_NAMESPACE_STD=}",
    "_LIBCPP_EQUAL_DELETE",
    "_LIBCPP_EXCEPTION_ABI",
    "_LIBCPP_EXCLUDE_FROM_EXPLICIT_INSTANTIATION",
    "_LIBCPP_EXPLICIT",
    "_LIBCPP_EXPLICIT_AFTER_CXX11",
    "_LIBCPP_EXPORTED_FROM_ABI",
    "_LIBCPP_EXTERN_TEMPLATE(%0)",
    "_LIBCPP_EXTERN_TEMPLATE_DEFINE(%0)",
    "_LIBCPP_EXTERN_TEMPLATE_EVEN_IN_DEBUG_MODE(%0)",
    "_LIBCPP_EXTERN_TEMPLATE_TYPE_VIS",
    "_LIBCPP_FALLTHROUGH(%0)",
    "_LIBCPP_FORMAT_PRINTF(%0,%1)",
    "_LIBCPP_FUNC_VIS",
    "_LIBCPP_INIT_PRIORITY_MAX",
    "_LIBCPP_INLINE_VAR",
    "_LIBCPP_INLINE_VISIBILITY",
    "_LIBCPP_INTERNAL_LINKAGE",
    "_LIBCPP_NOALIAS",
    "_LIBCPP_NODEBUG",
    "_LIBCPP_NODEBUG_TYPE",
    "_LIBCPP_NODISCARD_AFTER_CXX17",
    "_LIBCPP_NODISCARD_ATTRIBUTE",
    "_LIBCPP_NODISCARD_EXT",
    "_LIBCPP_NORETURN",
    "_LIBCPP_NO_DESTROY",
    "_LIBCPP_OVERRIDABLE_FUNC_VIS",
    "_LIBCPP_PREFERRED_NAME(%0)",
    "_LIBCPP_SAFE_STATIC",
    "_LIBCPP_THREAD_SAFETY_ANNOTATION(%0)",
    "_LIBCPP_TOSTRING(%0)=\"%0\"",
    "_LIBCPP_TOSTRING2(%0)=\"%0\"",
    "_LIBCPP_TYPE_VIS",
    "_LIBCPP_TEMPLATE_VIS",
    "_LIBCPP_TYPE_VIS_ONLY",
    "_LIBCPP_UNUSED_VAR(%0)=%0",
    "_LIBCPP_SHARED_PTR_TRIVIAL_ABI",
    "_LIBCPP_WEAK",
    "_MCRTIMP",
    "_MRTIMP2",
    "_NOEXCEPT",
    "noexcept",
    "_NOEXCEPT_(%0)",
    "_Noreturn",
    "_PSTL_ASSERT(%0)",
    "_PSTL_ASSERT_MSG(%0,%1)",
    "_STD_BEGIN=namespace std{",
    "_STD_END=}",
    "_STRUCT_NAME(%0)",
    "_Static_assert(%0,%1)",
    "_T",
    "_UNION_NAME(%0)",
    "_VSTD=std",
    "_VSTD_FS=std::filesystem",
    "__BEGIN_DECLS=extern \"C\" {",
    "__CLRCALL_OR_CDECL",
    "__CONCAT(%0,%1)=%0%1",
    "__CRTDECL",
    "__CRT_INLINE",
    "__CRT_STRINGIZE(%0)=\"%0\"",
    "__CRT_UUID_DECL(%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11)",
    "__CRT_WIDE(%0)=L\"%0\"",
    "__END_DECLS=}",
    "__GOMP_NOTHROW",
    "__LEAF",
    "__LEAF_ATTR",
    "__MINGW_ATTRIB_CONST",
    "__MINGW_ATTRIB_DEPRECATED",
    "__MINGW_ATTRIB_DEPRECATED_MSG(%0)",
    "__MINGW_ATTRIB_MALLOC",
    "__MINGW_ATTRIB_NONNULL(%0)",
    "__MINGW_ATTRIB_NORETURN",
    "__MINGW_ATTRIB_NO_OPTIMIZE",
    "__MINGW_ATTRIB_PURE",
    "__MINGW_ATTRIB_UNUSED",
    "__MINGW_ATTRIB_USED",
    "__MINGW_BROKEN_INTERFACE(%0)",
    "__MINGW_IMPORT",
    "__MINGW_INTRIN_INLINE=extern",
    "__MINGW_NOTHROW",
    "__MINGW_PRAGMA_PARAM(%0)",
    "__N(%0)=%0",
    "__NTH(%0)=%0",
    "__NTHNL(%0)=%0",
    "__P(%0)=%0",
    "__PMT(%0)=%0",
    "__PSTL_ASSERT(%0)",
    "__PSTL_ASSERT_MSG(%0,%1)",
    "__STRING(%0)=\"%0\"",
    "__THROW",
    "__THROWNL",
    "__UNUSED_PARAM(%0)=%0",
    "__always_inline",
    "__attribute__(%0)",
    "__attribute_alloc_size__(%0)",
    "__attribute_artificial__",
    "__attribute_const__",
    "__attribute_copy__(%0)",
    "__attribute_deprecated__",
    "__attribute_deprecated_msg__(%0)",
    "__attribute_format_arg__(%0)",
    "__attribute_format_strfmon__(%0,%1)",
    "__attribute_malloc__",
    "__attribute_noinline__",
    "__attribute_nonstring__",
    "__attribute_pure__",
    "__attribute_used__",
    "__attribute_warn_unused_result__",
    "__cdecl",
    "__const=const",
    "__cpp_deduction_guides=0",
    "__errordecl(%0,%1)=extern void %0 (void)",
    "__extension__",
    "__extern_always_inline=extern",
    "__extern_inline=extern",
    "__flexarr=[]",
    "__forceinline",
    "__fortify_function=extern",
    "__glibc_likely(%0)=(%0)",
    "__glibc_macro_warning(%0)",
    "__glibc_macro_warning1(%0)",
    "__glibc_unlikely(%0)=(%0)",
    "__glibcxx_assert(%0)",
    "__glibcxx_assert_impl(%0)",
    "__inline",
    "__nonnull(%0)",
    "__nothrow",
    "__restrict",
    "__restrict__",
    "__restrict_arr",
    "__stdcall",
    "__warnattr(%0)",
    "__warndecl(%0,%1)=extern void %0 (void)",
    "__wur",
    "_inline",
    "emit",
    "static_assert(%0)",
    "wxDECLARE_EVENT(%0,%1)=int %0;",
    "wxDECLARE_EXPORTED_EVENT(%0,%1,%2)=int %1;",
    "wxDEPRECATED(%0)=%0",
    "wxMSVC_FWD_MULTIPLE_BASES",
    "wxOVERRIDE",
    "wxStatusBar=wxStatusBarBase",
    "wxT",
    "WXDLLIMPEXP_DATA_BASE(%0)=%0",
    "WXDLLIMPEXP_DATA_CORE(%0)=%0",
#if defined(__WXGTK__)
    "__WXGTK__=1",
    "__linux__=1",
    "linux=1",
    "wxTopLevelWindowNative=wxTopLevelWindowGTK",
    "wxWindowNative=wxWindowGTK",
    "wxWindow=wxWindowGTK",
    "BUTTON_BASE=wxButton",
#elif defined(__WXMSW__)
    "__WXMSW__=1",
    "_WIN32=1",
    "_WIN64=1",
    "wxTopLevelWindowNative=wxTopLevelWindowMSW",
    "wxWindow=wxWindowMSW",
    "wxWindowNative=wxWindowMSW",
    "BUTTON_BASE=wxControl",
#else
    "__APPLE__=1",
    "wxTopLevelWindowNative=wxTopLevelWindowMac",
    "wxWindowNative=wxWindowMac",
    "BUTTON_BASE=wxControl",
    "wxWindow=wxWindowMac",
#endif
};

thread_local std::vector<wxString> DEFAULT_TYPES = {
    "std::unique_ptr::pointer=_Tp", // needed for unique_ptr
                                    // {unordered}_map / map / {unordered}_multimap
    "std::*map::*iterator=std::pair<_Key, _Tp>",
    "std::*map::value_type=std::pair<_Key, _Tp>",
    "std::*map::key_type=_Key",
    "std::*map::mapped_type=_Tp",
    // unordered_set / unordered_multiset
    "std::unordered_*set::*iterator=_Value",
    "std::unordered_*set::value_type=_Value",
    // set / multiset
    "std::set::*iterator=_Key",
    "std::multiset::*iterator=_Key",
    "std::set::value_type=_Key",
    // vector
    "std::vector::*reference=_Tp",
    "std::vector::*iterator=_Tp",
    // queue / priority_queue
    "std::*que*::*reference=_Tp",
    "std::*que*::*iterator=_Tp",
    // stack
    "std::stack::*reference=_Tp",
    // list
    "std::list::*reference=_Tp",
    // shared_ptr
    "std::shared_ptr::element_type=_Tp",
};

const std::vector<wxString>& TagsOptionsData::GetDefaultTokens() { return DEFAULT_TOKENS; }
const std::vector<wxString>& TagsOptionsData::GetDefaultTypes() { return DEFAULT_TYPES; }

TagsOptionsData::TagsOptionsData()
    : clConfigItem("code-completion")
    , m_ccFlags(CC_DISP_TYPE_INFO | CC_DISP_FUNC_CALLTIP)
    , m_ccColourFlags(CC_COLOUR_DEFAULT)
    , m_fileSpec("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++")
    , m_minWordLen(3)
    , m_parserEnabled(true)
    , m_maxItemToColour(1000)
#ifdef __WXMSW__
    , m_macrosFiles("_mingw.h bits/c++config.h")
#elif defined(__WXMAC__)
    , m_macrosFiles("sys/cdefs.h bits/c++config.h AvailabilityMacros.h")
#else
    , m_macrosFiles("sys/cdefs.h bits/c++config.h")
#endif
    , m_clangOptions(0)
    , m_clangBinary("")
    , m_clangCachePolicy(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD)
    , m_ccNumberOfDisplayItems(150)
    , m_version(0)
{
    // Initialize defaults
    m_languages.Add("C++");
    AddDefaultTokens();
    AddDefaultTypes();

    SyncData();
}

TagsOptionsData::~TagsOptionsData() {}

void TagsOptionsData::AddDefaultTokens()
{
    m_tokens.reserve(m_tokens.size() + DEFAULT_TOKENS.size());
    for(const auto& token : DEFAULT_TOKENS) {
        m_tokens.Add(token);
    }
}

void TagsOptionsData::AddDefaultTypes() {}

wxString TagsOptionsData::ToString() const
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

    const wxStringTable_t& tokensMap = GetTokensWxMap();
    wxStringTable_t::const_iterator iter = tokensMap.begin();

    if(tokensMap.empty() == false) {
        for(; iter != tokensMap.end(); ++iter) {
            if(!iter->second.IsEmpty() || (iter->second.IsEmpty() && iter->first.Find("%0") != wxNOT_FOUND)) {
                // Key = Value pair. Place this one in the output file
                file_content << iter->first << "=" << iter->second << "\n";
            } else {

                if(options.IsEmpty()) {
                    options = " -I";
                }

                options << iter->first;
                options << ",";
            }
        }

        if(options.IsEmpty() == false) {
            options.RemoveLast();
        }

        options += " ";
    }

    // write the file content
    if(file_name.IsEmpty() == false) {
        wxFFile fp(file_name, "w+b");
        if(fp.IsOpened()) {
            fp.Write(file_content);
            fp.Close();
        }
    }

    //    if(GetLanguages().IsEmpty() == false) {
    //        options += " --language-force=";
    //        options += GetLanguages().Item(0);
    //        options += " ";
    //    }
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
        wxString k = item.BeforeFirst('=');
        wxString v = item.AfterFirst('=');

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
        wxString k = item.BeforeFirst('=');
        wxString v = item.AfterFirst('=');
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
        wxString k = item.BeforeFirst('=');
        wxString v = item.AfterFirst('=');
        m_tokensWxMap[k] = v;
    }
}

void TagsOptionsData::DoUpdateTokensWxMapReversed()
{
    m_tokensWxMapReversed.clear();
    for(size_t i = 0; i < m_tokens.GetCount(); i++) {
        wxString item = m_tokens.Item(i).Trim().Trim(false);
        wxString k = item.AfterFirst('=');
        wxString v = item.BeforeFirst('=');
        if(_IsValidCppIndetifier(k) && !_IsCppKeyword(k)) {
            m_tokensWxMapReversed[k] = v;
        }
    }
}

const wxStringTable_t& TagsOptionsData::GetTokensReversedWxMap() const { return m_tokensWxMapReversed; }

void TagsOptionsData::FromJSON(const JSONItem& json)
{
    m_version = json.namedObject("version").toSize_t();
    m_ccFlags = json.namedObject("m_ccFlags").toSize_t(m_ccFlags);
    m_ccColourFlags = json.namedObject("m_ccColourFlags").toSize_t(m_ccColourFlags);
    m_tokens = json.namedObject("m_tokens").toArrayString();
    m_types = json.namedObject("m_types").toArrayString();
    m_fileSpec = json.namedObject("m_fileSpec").toString(m_fileSpec);
    m_languages = json.namedObject("m_languages").toArrayString();
    m_minWordLen = json.namedObject("m_minWordLen").toInt(m_minWordLen);
    m_parserSearchPaths = json.namedObject("m_parserSearchPaths").toArrayString();
    m_parserEnabled = json.namedObject("m_parserEnabled").toBool();
    m_parserExcludePaths = json.namedObject("m_parserExcludePaths").toArrayString();
    m_maxItemToColour = json.namedObject("m_maxItemToColour").toInt();
    m_macrosFiles = json.namedObject("m_macrosFiles").toString();
    m_clangOptions = json.namedObject("m_clangOptions").toSize_t();
    m_clangBinary = json.namedObject("m_clangBinary").toString();
    m_clangCmpOptions = json.namedObject("m_clangCmpOptions").toString();
    m_clangSearchPaths = json.namedObject("m_clangSearchPaths").toArrayString();
    m_clangMacros = json.namedObject("m_clangMacros").toString();
    m_clangCachePolicy = json.namedObject("m_clangCachePolicy").toString();
    m_ccNumberOfDisplayItems = json.namedObject("m_ccNumberOfDisplayItems").toSize_t(m_ccNumberOfDisplayItems);

    if(!m_fileSpec.Contains("*.hxx")) {
        m_fileSpec = "*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++";
    }

    DoUpdateTokensWxMapReversed();
    DoUpdateTokensWxMap();
    m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;
}

JSONItem TagsOptionsData::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
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
    for(size_t i = 0; i < arr.GetCount(); ++i) {
        s << arr.Item(i) << "\n";
    }

    if(s.IsEmpty() == false) {
        s.RemoveLast();
    }

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
        m_ccNumberOfDisplayItems = tod.m_ccNumberOfDisplayItems;
    }
    m_version = TagsOptionsData::CURRENT_VERSION;
}

void TagsOptionsData::SyncData()
{
    DoUpdateTokensWxMap();
    DoUpdateTokensWxMapReversed();
}

std::vector<std::pair<wxString, wxString>> TagsOptionsData::GetTypes() const
{
    std::vector<std::pair<wxString, wxString>> arr;
    arr.reserve(m_types.size());

    for(const auto& line : m_types) {
        std::pair<wxString, wxString> p;
        p.first = line.BeforeFirst('=');
        p.second = line.AfterFirst('=');
        arr.emplace_back(p);
    }
    return arr;
}
