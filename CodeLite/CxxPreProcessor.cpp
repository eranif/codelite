#include "CxxPreProcessor.h"
#include "file_logger.h"
#include <wx/regex.h>

CxxPreProcessor::CxxPreProcessor()
    : m_options(0)
    , m_maxDepth(-1)
    , m_currentDepth(0)
{
}

CxxPreProcessor::~CxxPreProcessor() {}

void CxxPreProcessor::Parse(const wxFileName& filename, size_t options)
{
    try {
        m_options = options;
        std::unordered_set<wxString> V;
        CxxPreProcessorScanner scanner(filename, m_options, V);

        // Remove the option so recursive scanner won't get it
        m_options &= ~kLexerOpt_DontCollectMacrosDefinedInThisFile;
        if(!scanner.IsNull()) {
            scanner.Parse(this);
        }
    } catch(CxxLexerException& e) {
        wxUnusedVar(e);
    }

    // Delete all the 'deleteOnExit' tokens
    CxxPreProcessorToken::Map_t filteredMap;
    filteredMap.reserve(m_tokens.size());

    for(const auto& p : m_tokens) {
        if(!p.second.deleteOnExit) {
            filteredMap.insert(std::make_pair(p.first, p.second));
        }
    }
    m_tokens.swap(filteredMap);
}

bool CxxPreProcessor::ExpandInclude(const wxFileName& currentFile, const wxString& includeStatement,
                                    wxFileName& outFile)
{
    // skip STL debug folders
    if(includeStatement.StartsWith("<debug/")) {
        return false;
    }

    wxString includeName = includeStatement;
    includeName.Replace("\"", "");
    includeName.Replace("<", "");
    includeName.Replace(">", "");

    // Try the current file's directory first
    wxArrayString paths = m_includePaths;
    paths.Insert(currentFile.GetPath(), 0);

    if(m_noSuchFiles.count(includeStatement)) {
        // wxPrintf("No such file hit\n");
        return false;
    }

    auto iter = m_fileMapping.find(includeStatement);
    if(iter != m_fileMapping.end()) {
        // if this file has a mapped file, it means that we either
        // already scanned it or could not find a match for it
        // wxPrintf("File already been scanned\n");
        return false;
    }

    for(size_t i = 0; i < paths.GetCount(); ++i) {
        wxString tmpfile;
        tmpfile << paths.Item(i) << "/" << includeName;
        wxFileName fn(tmpfile);
        if(fn.FileExists()) {
            fn.MakeAbsolute();
            m_fileMapping.insert({ includeStatement, fn.GetFullPath() });
            outFile = fn;
            return true;
        } else {
            // CL_DEBUG("Including a folder :/ : %s", fixedFileName.GetFullPath());
        }
    }

    // remember that we could not locate this include statement
    m_noSuchFiles.insert(includeStatement);
    m_fileMapping.insert(std::make_pair(includeStatement, wxString()));
    return false;
}

void CxxPreProcessor::AddIncludePath(const wxString& path) { m_includePaths.Add(path); }

void CxxPreProcessor::AddDefinition(const wxString& def)
{
    wxString macroName = def.BeforeFirst('=');
    wxString macroValue = def.AfterFirst('=');

    CxxPreProcessorToken token;
    token.name = macroName;
    token.value = macroValue;
    m_tokens.insert(std::make_pair(macroName, token));
}

wxArrayString CxxPreProcessor::GetDefinitions() const
{
    wxArrayString defs;
    CxxPreProcessorToken::Map_t::const_iterator iter = m_tokens.begin();
    for(; iter != m_tokens.end(); ++iter) {
        wxString macro;
        macro = iter->second.name;
        if(!iter->second.value.IsEmpty()) {
            macro << "=" << iter->second.value;
        }
        defs.Add(macro);
    }
    return defs;
}

wxString CxxPreProcessor::GetGxxCommand(const wxString& gxx, const wxString& filename) const
{
    wxString command;
    command << gxx << " -dM -E -D__WXMSW__ -D__cplusplus -fsyntax-only ";
    for(size_t i = 0; i < m_includePaths.GetCount(); ++i) {
        command << "-I" << m_includePaths.Item(i) << " ";
    }
    command << " - < " << filename;
    return command;
}

void CxxPreProcessor::SetIncludePaths(const wxArrayString& includePaths)
{
    m_includePaths.Clear();
    m_includePaths.reserve(includePaths.size());

    std::unordered_set<wxString> S;
    S.reserve(includePaths.size());

    for(size_t i = 0; i < includePaths.GetCount(); ++i) {
        wxString path = includePaths.Item(i);
        path.Trim().Trim(false);
        if(path.IsEmpty()) {
            continue;
        }

        if(S.count(path) == 0) {
            m_includePaths.Add(path);
            S.insert(path);
        }
    }
}

bool CxxPreProcessor::CanGoDeeper() const
{
    if(m_maxDepth == -1)
        return true;
    return m_currentDepth < m_maxDepth;
}

void CxxPreProcessor::DecDepth()
{
    m_currentDepth--;
    if(m_currentDepth < 0) {
        m_currentDepth = 0;
    }
}

void CxxPreProcessor::IncDepth() { m_currentDepth++; }
