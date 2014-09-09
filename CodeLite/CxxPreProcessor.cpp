#include "CxxPreProcessor.h"
#include <wx/regex.h>

CxxPreProcessor::CxxPreProcessor()
{
}

CxxPreProcessor::~CxxPreProcessor()
{
}

void CxxPreProcessor::Parse(const wxFileName& filename, size_t options)
{
    try {
        m_options = options;
        CxxPreProcessorScanner::Ptr_t scanner(new CxxPreProcessorScanner(filename, m_options));
        scanner->Parse(this);
    } catch(CxxLexerException& e) {
    }
}

CxxPreProcessorScanner::Ptr_t CxxPreProcessor::CreateScanner(const wxFileName& currentFile,
                                                             const wxString& includeStatement)
{
    wxString includeName = includeStatement;
    includeName.Replace("\"", "");
    includeName.Replace("<", "");
    includeName.Replace(">", "");

    // Try the current file's directory first
    wxArrayString paths = m_includePaths;
    paths.Insert(currentFile.GetPath(), 0);

    for(size_t i = 0; i < paths.GetCount(); ++i) {
        wxString tmpfile;
        tmpfile << paths.Item(i) << "/" << includeName;
        wxFileName fn(tmpfile);
        DEBUGMSG(" ? Checking include file: %s\n", fn.GetFullPath());
        if(fn.Exists() && !m_filesVisited.count(fn.GetFullPath())) {
            DEBUGMSG(" ==> Creating scanner for file: %s\n", fn.GetFullPath());
            m_filesVisited.insert(fn.GetFullPath());
            return CxxPreProcessorScanner::Ptr_t(new CxxPreProcessorScanner(fn, m_options));
        }
    }
    return CxxPreProcessorScanner::Ptr_t(NULL);
}

void CxxPreProcessor::AddIncludePath(const wxString& path)
{
    m_includePaths.Add(path);
}

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
