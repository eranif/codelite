#include <wx/stc/stc.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <lexer_configuration.h>
#include <editor_config.h>
#include "php_utils.h"
#include <wx/uri.h>
#include <wx/base64.h>

bool IsPHPCommentOrString(int styleAtPos)
{
    if( (styleAtPos == wxSTC_HPHP_HSTRING)      ||
        (styleAtPos == wxSTC_HPHP_SIMPLESTRING) ||
        (styleAtPos == wxSTC_HPHP_COMMENT)      ||
        (styleAtPos == wxSTC_HPHP_COMMENTLINE)
      )
        return true;
    return false;
}

bool IsPHPSection(int styleAtPos )
{
    if( (styleAtPos == wxSTC_HPHP_DEFAULT)          ||
        (styleAtPos == wxSTC_HPHP_HSTRING)          ||
        (styleAtPos == wxSTC_HPHP_SIMPLESTRING)     ||
        (styleAtPos == wxSTC_HPHP_WORD)             ||
        (styleAtPos == wxSTC_HPHP_NUMBER)           ||
        (styleAtPos == wxSTC_HPHP_VARIABLE)         ||
        (styleAtPos == wxSTC_HPHP_COMMENT)          ||
        (styleAtPos == wxSTC_HPHP_COMMENTLINE)      ||
        (styleAtPos == wxSTC_HPHP_HSTRING_VARIABLE) ||
        (styleAtPos == wxSTC_HPHP_OPERATOR))
        return true;
    return false;
}

bool IsPHPFile(IEditor* editor)
{
    if(!editor)
        return false;

    wxFileName fileName = editor->GetFileName();
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer(wxT("Html"));
    wxString fileSpec;

    if(!lexer) {
        // Incase somehow we failed in retrieving the lexer (corrupted XML file)
        // use some hardcoded file spec
        fileSpec = wxT("*.php;*.inc;*.phtml");

    } else {
        fileSpec = lexer->GetFileSpec();

    }

    wxStringTokenizer tkz ( fileSpec, wxT ( ";" ) );
    while ( tkz.HasMoreTokens() ) {
        wxString fileExt  = tkz.NextToken();
        wxString fullname = fileName.GetFullName();

        fileExt.MakeLower();
        fullname.MakeLower();
        if ( wxMatchWild ( fileExt, fullname ) ) {
            return true;
        }
    }
    return false;

}

wxMemoryBuffer ReadFileContent(const wxString& filename)
{
    FILE *fp;
    long len;
    wxMemoryBuffer buffer;

    fp = fopen(filename.mb_str(wxConvUTF8).data(), "rb");
    if (!fp) {
        return buffer;
    }

    //read the whole file
    fseek(fp, 0, SEEK_END);  //go to end
    len = ftell(fp);         //get position at end (length)
    fseek(fp, 0, SEEK_SET);  //go to begining
    char *pbuf = static_cast<char*>(buffer.GetWriteBuf(len)); // make sure the buffer is large enough

    size_t bytes = fread(pbuf, sizeof(char), len, fp);
    buffer.SetDataLen(bytes);
    return buffer;
}

wxString GetResourceDirectory()
{
    wxFileName fn;
#ifdef __WXGTK__
    fn = wxFileName( PLUGINS_DIR, "");
    fn.AppendDir( "resources" );
#else
#  ifdef USE_POSIX_LAYOUT
    fn = wxFileName( wxStandardPaths::Get().GetDataDir() + wxT(PLUGINS_DIR) );
#  else
    fn = wxFileName( wxStandardPaths::Get().GetExecutablePath() );
    fn.AppendDir( "plugins" );
#  endif
    fn.AppendDir( "resources" );
#endif
    fn.AppendDir( "php" );
    return fn.GetPath();
}

wxString GetCCResourceDirectory()
{
    wxFileName fn( GetResourceDirectory(), "" );
    fn.AppendDir( "cc" );
    return fn.GetPath();
}

wxString URIToFileName(const wxString& uriFileName)
{
    wxString filename = uriFileName;
    filename.StartsWith(FILE_SCHEME, &filename);
#ifdef __WXMSW__
    if ( filename.StartsWith("/") ) {
        filename.Remove(0, 1);
    }
#endif
    return wxFileName(filename).GetFullPath();
}

wxString FileNameToURI(const wxString& filename)
{
    wxString sourceFullPath = wxFileName(filename).GetFullPath();
    if ( !sourceFullPath.StartsWith(FILE_SCHEME) ) {
        sourceFullPath.Prepend(FILE_SCHEME);
    }
    sourceFullPath.Replace("\\", "/");
    while (sourceFullPath.Replace("//", "/") ) {}
    wxURI uri(sourceFullPath);
    sourceFullPath = uri.BuildURI();
    sourceFullPath.Replace("file:", FILE_SCHEME);
    return sourceFullPath;
}

wxString Base64Encode(const wxString& str)
{
    wxString encodedString = ::wxBase64Encode(str.mb_str(wxConvUTF8).data(), str.length());
    return encodedString;
}
