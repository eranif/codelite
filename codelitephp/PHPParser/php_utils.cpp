#include "php_utils.h"

#include "JSON.h"
#include "PHPSourceFile.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "php_project.h"
#include "php_workspace.h"

#include <editor_config.h>
#include <lexer_configuration.h>
#include <map>
#include <wx/base64.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>
#include <wx/treectrl.h>
#include <wx/uri.h>

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
    if(!editor) {
        return false;
    }
    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
    return ::IsPHPFileByExt(editor->GetFileName().GetFullPath()) && PHPSourceFile::IsInPHPSection(buffer);
}

bool IsPHPFileByExt(const wxString& filename)
{
    return (FileExtManager::GetType(filename) == FileExtManager::TypePhp);
    // wxFileName fileName = filename;
    // LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer(wxT("php"));
    // wxString fileSpec;

    // if(!lexer) {
    //    // Incase somehow we failed in retrieving the lexer (corrupted XML file)
    //    // use some hardcoded file spec
    //    fileSpec = wxT("*.php;*.inc;*.phtml");

    //} else {
    //    fileSpec = lexer->GetFileSpec();
    //}

    // wxStringTokenizer tkz(fileSpec, wxT(";"));
    // while(tkz.HasMoreTokens()) {
    //    wxString fileExt = tkz.NextToken();
    //    wxString fullname = fileName.GetFullName();

    //    fileExt.MakeLower();
    //    fullname.MakeLower();
    //    if(wxMatchWild(fileExt, fullname)) {
    //        return true;
    //    }
    //}
    // return false;
}

wxString GetResourceDirectory()
{
    wxFileName fn;
#ifdef __WXGTK__
    fn = wxFileName(PLUGINS_DIR, "");
    fn.AppendDir("resources");
#else
#ifdef USE_POSIX_LAYOUT
    fn = wxFileName(clStandardPaths::Get().GetPluginsDirectory());
#else
    fn = wxFileName(clStandardPaths::Get().GetExecutablePath());
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

static wxString URIEncode(const wxString& inputStr) { return FileUtils::EncodeURI(inputStr); }

wxString FileNameToURI(const wxString& filename)
{
    wxString sourceFullPath = wxFileName(filename).GetFullPath();
    if(!sourceFullPath.StartsWith(FILE_SCHEME)) {
        sourceFullPath.Prepend(FILE_SCHEME);
    }
    sourceFullPath.Replace("\\", "/");
    while(sourceFullPath.Replace("//", "/")) {}
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

static void DecodeFileName(wxString& filename) { filename = FileUtils::DecodeURI(filename); }

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

    // Remove URI encoding ("%20"=>" " etc)
    DecodeFileName(filename);

    // First check if the remote file exists locally
    if(wxFileName(filename).Exists()) {
        return wxFileName(filename).GetFullPath();
    }

    // Use the active project file mapping
    const PHPProjectSettingsData& settings = pProject->GetSettings();
    const wxStringMap_t& mapping = settings.GetFileMapping();
    wxStringMap_t::const_iterator iter = mapping.begin();
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
