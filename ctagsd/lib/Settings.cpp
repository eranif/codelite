#include "Settings.hpp"

#include "CompileCommandsJSON.h"
#include "CompileFlagsTxt.h"
#include "GCCMetadata.hpp"
#include "JSON.h"
#include "clTempFile.hpp"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"
#include "tags_options_data.h"

#include <set>
#include <wx/string.h>
#include <wx/tokenzr.h>

namespace
{
FileLogger& operator<<(FileLogger& logger, const std::vector<std::pair<wxString, wxString>>& vec)
{
    wxString buffer;
    buffer << "[";
    for(const auto& d : vec) {
        buffer << "{" << d.first << ", " << d.second << "}, ";
    }
    buffer << "]";
    logger << buffer;
    return logger;
}

std::vector<wxString> DEFAULT_TYPES = {
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
};

std::vector<wxString> DEFAULT_TOKENS = {
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
    "_LIBCPP_TYPE_VIS_ONLY",
    "_LIBCPP_UNUSED_VAR(%0)=%0",
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

std::vector<std::pair<wxString, wxString>> to_vector_of_pairs(const std::vector<wxString>& arr)
{
    std::vector<std::pair<wxString, wxString>> result;
    result.reserve(arr.size());
    for(const wxString& line : arr) {
        wxString k = line.BeforeFirst('=');
        wxString v = line.AfterFirst('=');
        result.emplace_back(std::make_pair(k, v));
    }
    return result;
}

void write_to_json(JSONItem& json_arr, const std::vector<std::pair<wxString, wxString>>& arr)
{
    for(const auto& entry : arr) {
        auto type = json_arr.AddObject(wxEmptyString);
        type.addProperty("key", entry.first);
        type.addProperty("value", entry.second);
    }
}
} // namespace

CTagsdSettings::CTagsdSettings()
{
    // set some defaults
    m_tokens = to_vector_of_pairs(DEFAULT_TOKENS);
    m_types = to_vector_of_pairs(DEFAULT_TYPES);
}

CTagsdSettings::~CTagsdSettings() {}

void CTagsdSettings::Load(const wxFileName& filepath)
{
    JSON config_file(filepath);
    m_settings_dir = filepath.GetPath();

    if(!config_file.isOk()) {
        clWARNING() << "Could not locate configuration file:" << filepath << endl;
        CreateDefault(filepath);

    } else {
        auto config = config_file.toElement();
        m_search_path = config["search_path"].toArrayString();
        m_file_mask = config["file_mask"].toString(m_file_mask);
        m_ignore_spec = config["ignore_spec"].toString(m_ignore_spec);
        m_codelite_indexer = config["codelite_indexer"].toString();
        m_limit_results = config["limit_results"].toSize_t(m_limit_results);
        CreateDefault(filepath); // generate the default tokens and types
    }

    build_search_path(filepath);
    if(m_types.empty() || m_tokens.empty() || m_search_path.empty()) {
        CreateDefault(filepath);
    }

    LOG_IF_TRACE { clDEBUG1() << "search path...........:" << m_search_path << endl; }
    LOG_IF_TRACE { clDEBUG1() << "tokens................:" << m_tokens << endl; }
    LOG_IF_TRACE { clDEBUG1() << "types.................:" << m_types << endl; }
    LOG_IF_TRACE { clDEBUG1() << "file_mask.............:" << m_file_mask << endl; }
    LOG_IF_TRACE { clDEBUG1() << "codelite_indexer......:" << m_codelite_indexer << endl; }
    LOG_IF_TRACE { clDEBUG1() << "ignore_spec...........:" << m_ignore_spec << endl; }
    LOG_IF_TRACE { clDEBUG1() << "limit_results.........:" << m_limit_results << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Settings dir is set to:" << m_settings_dir << endl; }

    // conver the tokens to wxArrayString
    wxArrayString wxarr;
    wxarr.reserve(m_tokens.size());
    for(const auto& p : m_tokens) {
        wxarr.Add(p.first + "=" + p.second);
    }

    // update ctags manager
    TagsOptionsData tod;
    tod.SetCcNumberOfDisplayItems(m_limit_results);
    tod.SetTokens(wxJoin(wxarr, '\n'));
    TagsManagerST::Get()->SetCtagsOptions(tod);
}

void CTagsdSettings::Save(const wxFileName& filepath)
{
    JSON config_file(cJSON_Object);
    auto config = config_file.toElement();
    config.addProperty("file_mask", m_file_mask);
    config.addProperty("ignore_spec", m_ignore_spec);
    config.addProperty("codelite_indexer", m_codelite_indexer);
    config.addProperty("limit_results", m_limit_results);
    config.addProperty("search_path", m_search_path);

    auto types = config.AddArray("types");
    write_to_json(types, m_types);

    auto tokens = config.AddArray("tokens");
    write_to_json(tokens, m_tokens);
    config_file.save(filepath);
}

void CTagsdSettings::build_search_path(const wxFileName& filepath)
{
    // check the root folder for compile_flags.txt file or compile_commands.json
    wxFileName fn(filepath);
    fn.RemoveLastDir();

    wxString path = fn.GetPath();

    wxFileName compile_flags_txt(path, "compile_flags.txt");
    wxFileName compile_commands_json(path, "compile_commands.json");

    std::set<wxString> S{ m_search_path.begin(), m_search_path.end() };
    if(compile_flags_txt.FileExists()) {
        // we are using the compile_flags.txt file method
        CompileFlagsTxt cft(compile_flags_txt);
        S.insert(cft.GetIncludes().begin(), cft.GetIncludes().end());
    } else if(compile_commands_json.FileExists()) {
        CompileCommandsJSON ccj(compile_commands_json.GetFullPath());
        S.insert(ccj.GetIncludes().begin(), ccj.GetIncludes().end());
    }

#if defined(__WXGTK__) || defined(__WXOSX__)
    wxString cxx = "/usr/bin/g++";

#ifdef __WXOSX__
    cxx = "/usr/bin/clang++";
#endif

    // Common compiler paths - should be placed at top of the include path!
    wxString command;

    // GCC prints parts of its output to stdout and some to stderr
    // redirect all output to stdout
    wxString working_directory;
    clTempFile tmpfile;
    tmpfile.Write(wxEmptyString);
    command << "/bin/bash -c '" << cxx << " -v -x c++ /dev/null -fsyntax-only > " << tmpfile.GetFullPath() << " 2>&1'";

    ProcUtils::SafeExecuteCommand(command);

    wxString content;
    FileUtils::ReadFileContent(tmpfile.GetFullPath(), content);
    wxArrayString outputArr = wxStringTokenize(content, "\n\r", wxTOKEN_STRTOK);

    // Analyze the output
    bool collect(false);
    wxArrayString search_paths;
    for(wxString& line : outputArr) {
        line.Trim().Trim(false);

        // search the scan starting point
        if(line.Contains("#include <...> search starts here:")) {
            collect = true;
            continue;
        }

        if(line.Contains("End of search list.")) {
            break;
        }

        if(collect) {
            line.Replace("(framework directory)", wxEmptyString);
            // on Mac, (framework directory) appears also,
            // but it is harmless to use it under all OSs
            wxFileName includePath(line, wxEmptyString);
            includePath.Normalize();
            search_paths.Add(includePath.GetPath());
        }
    }

    S.insert(search_paths.begin(), search_paths.end());
#endif

    m_search_path.clear();
    m_search_path.reserve(S.size());
    for(const auto& path : S) {
        m_search_path.Add(path);
    }
}

void CTagsdSettings::CreateDefault(const wxFileName& filepath)
{
    m_tokens = to_vector_of_pairs(DEFAULT_TOKENS);
    m_types = to_vector_of_pairs(DEFAULT_TYPES);
    Save(filepath);
}

wxStringMap_t CTagsdSettings::GetMacroTable() const
{
    wxStringMap_t table;
    table.reserve(m_tokens.size());
    for(const auto& p : m_tokens) {
        table.insert(p);
    }
    return table;
}
