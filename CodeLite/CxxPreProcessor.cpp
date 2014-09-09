#include "CxxPreProcessor.h"
#include <wx/regex.h>
#include "file_logger.h"

CxxPreProcessor::CxxPreProcessor()
{
}

CxxPreProcessor::~CxxPreProcessor()
{
}

void CxxPreProcessor::Parse(const wxFileName& filename, size_t options)
{
    CxxPreProcessorScanner* scanner = NULL;
    try {
        CL_DEBUG("Calling CxxPreProcessor::Parse for file '%s'\n", filename.GetFullPath());
        m_options = options;
        scanner = new CxxPreProcessorScanner(filename, m_options);
        // Remove the option so recursive scanner won't get it
        m_options &= ~kLexerOpt_DontCollectMacrosDefinedInThisFile;
        if(scanner) {
            scanner->Parse(this);
        }
    } catch(CxxLexerException& e) {
        wxUnusedVar(e);
    }

    // Delete all the 'deleteOnExit' tokens
    CxxPreProcessorToken::Map_t filteredMap;
    CxxPreProcessorToken::Map_t::iterator iter = m_tokens.begin();
    for(; iter != m_tokens.end(); ++iter) {
        if(!iter->second.deleteOnExit) {
            filteredMap.insert(std::make_pair(iter->first, iter->second));
        }
    }
    m_tokens.swap(filteredMap);
    
    // Make sure that the scanner is deleted
    wxDELETE(scanner);
}

CxxPreProcessorScanner* CxxPreProcessor::CreateScanner(const wxFileName& currentFile, const wxString& includeStatement)
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
        tmpfile = fn.GetFullPath();
        // CL_DEBUG(" ... Checking include file: %s\n", fn.GetFullPath());
        struct stat buff;
        if(stat(tmpfile.mb_str(wxConvUTF8).data(), &buff) == 0) {
            // match
            if(m_filesVisited.count(tmpfile)) {
                // we already scanned this file
                CL_DEBUG1(" ==> File '%s' was already scanned\n", tmpfile);
                return NULL;
            }
            CL_DEBUG1(" ==> Creating scanner for file: %s\n", tmpfile);
            m_filesVisited.insert(tmpfile);
            return new CxxPreProcessorScanner(fn, m_options);
        }
    }
    return NULL;
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
