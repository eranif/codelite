#include "CxxPreProcessor.h"

#include "CxxPreProcessorScanner.h"

CxxPreProcessor::CxxPreProcessor()
    : m_options(0)
{
}

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
    } catch (const CxxLexerException& e) {
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
        }
    }

    // remember that we could not locate this include statement
    m_noSuchFiles.insert(includeStatement);
    m_fileMapping.insert(std::make_pair(includeStatement, wxString()));
    return false;
}

wxArrayString CxxPreProcessor::GetDefinitions() const
{
    wxArrayString defs;

    for (const auto& p : m_tokens) {
        wxString macro;
        macro = p.second.name;
        if (!p.second.value.IsEmpty()) {
            macro << "=" << p.second.value;
        }
        defs.Add(macro);
    }
    return defs;
}
