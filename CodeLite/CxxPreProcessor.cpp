#include "CxxPreProcessor.h"
#include <wx/regex.h>
#include "file_logger.h"

CxxPreProcessor::CxxPreProcessor()
    : m_maxDepth(-1)
    , m_currentDepth(0)
{
}

CxxPreProcessor::~CxxPreProcessor() {}

void CxxPreProcessor::Parse(const wxFileName& filename, size_t options)
{
    CxxPreProcessorScanner* scanner = NULL;
    try {
        //CL_DEBUG("Calling CxxPreProcessor::Parse for file '%s'\n", filename.GetFullPath());
        m_options = options;
        scanner = new CxxPreProcessorScanner(filename, m_options);
        // Remove the option so recursive scanner won't get it
        m_options &= ~kLexerOpt_DontCollectMacrosDefinedInThisFile;
        if(scanner && !scanner->IsNull()) {
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

bool
CxxPreProcessor::ExpandInclude(const wxFileName& currentFile, const wxString& includeStatement, wxFileName& outFile)
{
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

    std::map<wxString, wxString>::iterator iter = m_fileMapping.find(includeStatement);
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
        tmpfile = fn.GetFullPath();
        // CL_DEBUG(" ... Checking include file: %s\n", fn.GetFullPath());
        struct stat buff;
        if((stat(tmpfile.mb_str(wxConvUTF8).data(), &buff) == 0)) {
            //CL_DEBUG1(" ==> Creating scanner for file: %s\n", tmpfile);
            wxFileName fixedFileName(tmpfile);
            if(fixedFileName.FileExists()) {
                fixedFileName.Normalize(wxPATH_NORM_DOTS);
                tmpfile = fixedFileName.GetFullPath();
                m_fileMapping.insert(std::make_pair(includeStatement, tmpfile));
                outFile = fixedFileName;
                return true;
            } else {
                //CL_DEBUG("Including a folder :/ : %s", fixedFileName.GetFullPath());
            }
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
    for(size_t i = 0; i < includePaths.GetCount(); ++i) {
        wxString path = includePaths.Item(i);
        path.Trim().Trim(false);
        if(path.IsEmpty()) continue;

        if(m_includePaths.Index(path) == wxNOT_FOUND) {
            m_includePaths.Add(path);
        }
    }
}

bool CxxPreProcessor::CanGoDeeper() const
{
    if(m_maxDepth == -1) return true;
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
