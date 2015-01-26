#include <wx/stc/stc.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <lexer_configuration.h>
#include <editor_config.h>
#include "php_utils.h"
#include <wx/uri.h>
#include <wx/base64.h>
#include <map>
#include "php_project.h"
#include "php_workspace.h"
#include "json_node.h"

bool IsPHPCommentOrString(int styleAtPos)
{
    if((styleAtPos == wxSTC_HPHP_HSTRING) || (styleAtPos == wxSTC_HPHP_SIMPLESTRING) ||
       (styleAtPos == wxSTC_HPHP_COMMENT) || (styleAtPos == wxSTC_HPHP_COMMENTLINE))
        return true;
    return false;
}

bool IsPHPSection(int styleAtPos)
{
    if((styleAtPos == wxSTC_HPHP_DEFAULT) || (styleAtPos == wxSTC_HPHP_HSTRING) ||
       (styleAtPos == wxSTC_HPHP_SIMPLESTRING) || (styleAtPos == wxSTC_HPHP_WORD) ||
       (styleAtPos == wxSTC_HPHP_NUMBER) || (styleAtPos == wxSTC_HPHP_VARIABLE) || (styleAtPos == wxSTC_HPHP_COMMENT) ||
       (styleAtPos == wxSTC_HPHP_COMMENTLINE) || (styleAtPos == wxSTC_HPHP_HSTRING_VARIABLE) ||
       (styleAtPos == wxSTC_HPHP_OPERATOR))
        return true;
    return false;
}

bool IsPHPFile(IEditor* editor)
{
    if(!editor)
        return false;
    return ::IsPHPFile(editor->GetFileName().GetFullPath());
}

bool IsPHPFile(const wxString& filename)
{
    wxFileName fileName = filename;
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer(wxT("php"));
    wxString fileSpec;

    if(!lexer) {
        // Incase somehow we failed in retrieving the lexer (corrupted XML file)
        // use some hardcoded file spec
        fileSpec = wxT("*.php;*.inc;*.phtml");

    } else {
        fileSpec = lexer->GetFileSpec();
    }

    wxStringTokenizer tkz(fileSpec, wxT(";"));
    while(tkz.HasMoreTokens()) {
        wxString fileExt = tkz.NextToken();
        wxString fullname = fileName.GetFullName();

        fileExt.MakeLower();
        fullname.MakeLower();
        if(wxMatchWild(fileExt, fullname)) {
            return true;
        }
    }
    return false;
}

wxMemoryBuffer ReadFileContent(const wxString& filename)
{
    FILE* fp;
    long len;
    wxMemoryBuffer buffer;

    fp = fopen(filename.mb_str(wxConvUTF8).data(), "rb");
    if(!fp) {
        return buffer;
    }

    // read the whole file
    fseek(fp, 0, SEEK_END);                                   // go to end
    len = ftell(fp);                                          // get position at end (length)
    fseek(fp, 0, SEEK_SET);                                   // go to begining
    char* pbuf = static_cast<char*>(buffer.GetWriteBuf(len)); // make sure the buffer is large enough

    size_t bytes = fread(pbuf, sizeof(char), len, fp);
    buffer.SetDataLen(bytes);
    return buffer;
}

wxString GetResourceDirectory()
{
    wxFileName fn;
#ifdef __WXGTK__
    fn = wxFileName(PLUGINS_DIR, "");
    fn.AppendDir("resources");
#else
#ifdef USE_POSIX_LAYOUT
    fn = wxFileName(wxStandardPaths::Get().GetDataDir() + wxT(PLUGINS_DIR));
#else
    fn = wxFileName(wxStandardPaths::Get().GetExecutablePath());
    fn.AppendDir("plugins");
#endif
    fn.AppendDir("resources");
#endif
    fn.AppendDir("php");
    return fn.GetPath();
}

wxString URIToFileName(const wxString& uriFileName)
{
    wxString filename = wxURI::Unescape(uriFileName);
    filename.StartsWith(FILE_SCHEME, &filename);

#ifdef __WXMSW__
    if(filename.StartsWith("/")) {
        filename.Remove(0, 1);
    }
#endif
    return wxFileName(filename).GetFullPath();
}

static std::map<int, wxString> sEncodeMap;

static wxString URIEncode(const wxString& inputStr)
{
    if(sEncodeMap.empty()) {
        sEncodeMap['!'] = "%21";
        sEncodeMap['#'] = "%23";
        sEncodeMap['$'] = "%24";
        sEncodeMap['&'] = "%26";
        sEncodeMap['\''] = "%27";
        sEncodeMap['('] = "%28";
        sEncodeMap[')'] = "%29";
        sEncodeMap['*'] = "%2A";
        sEncodeMap['+'] = "%2B";
        sEncodeMap[','] = "%2C";
        sEncodeMap[';'] = "%3B";
        sEncodeMap['='] = "%3D";
        sEncodeMap['?'] = "%3F";
        sEncodeMap['@'] = "%40";
        sEncodeMap['['] = "%5B";
        sEncodeMap[']'] = "%5D";
        sEncodeMap[' '] = "%20";
        // sEncodeMap['/'] = "%2F";
        // sEncodeMap[':'] = "%3A";
    }

    wxString encoded;
    for(size_t i = 0; i < inputStr.length(); ++i) {
        std::map<int, wxString>::iterator iter = sEncodeMap.find(inputStr.at(i));
        if(iter != sEncodeMap.end()) {
            encoded << iter->second;
        } else {
            encoded << inputStr.at(i);
        }
    }
    return encoded;
}

wxString FileNameToURI(const wxString& filename)
{
    wxString sourceFullPath = wxFileName(filename).GetFullPath();
    if(!sourceFullPath.StartsWith(FILE_SCHEME)) {
        sourceFullPath.Prepend(FILE_SCHEME);
    }
    sourceFullPath.Replace("\\", "/");
    while(sourceFullPath.Replace("//", "/")) {
    }
    //    wxURI uri(sourceFullPath);
    sourceFullPath = URIEncode(sourceFullPath);
    sourceFullPath.Replace("file:", FILE_SCHEME);
    return sourceFullPath;
}

wxString Base64Encode(const wxString& str)
{
    wxString encodedString = ::wxBase64Encode(str.mb_str(wxConvUTF8).data(), str.length());
    return encodedString;
}

wxString MapRemoteFileToLocalFile(const wxString& remoteFile)
{
    // Check that a workspace is opened
    if(!PHPWorkspace::Get()->IsOpen())
        return remoteFile;

    // Sanity
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetActiveProject();
    if(!pProject)
        return remoteFile;

    // Map filename file attribute returned by xdebug to local filename
    wxString filename = remoteFile;
    
    // Remote the "file://" from the file path
    filename.StartsWith(FILE_SCHEME, &filename);
    // On Windows, the file is returned like (after removing the file://)
    // /C:/Http/htdocs/file.php - remote the leading "/"
    wxRegEx reMSWPrefix("/[a-zA-Z]{1}:/");
    if(reMSWPrefix.IsValid() && reMSWPrefix.Matches(filename)) {
        // Windows file
        filename.Remove(0, 1);
    }
    
    // First check if the remote file exists locally
    if(wxFileName(filename).Exists()) {
        return wxFileName(filename).GetFullPath();
    }
    
    // Use the active project file mapping
    const PHPProjectSettingsData& settings = pProject->GetSettings();
    const JSONElement::wxStringMap_t& mapping = settings.GetFileMapping();
    JSONElement::wxStringMap_t::const_iterator iter = mapping.begin();
    for(; iter != mapping.end(); ++iter) {
        const wxString& localFolder = iter->first;
        const wxString& remoteFolder = iter->second;
        if(filename.StartsWith(remoteFolder)) {
            filename.Replace(remoteFolder, localFolder);
            return wxFileName(filename).GetFullPath();
        }
    }
    
    // No matching was found
    return remoteFile;
}
