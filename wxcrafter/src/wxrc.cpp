/////////////////////////////////////////////////////////////////////////////
// Name:        wxrc.cpp
// Purpose:     XML resource compiler
// Author:      Vaclav Slavik, Eduardo Marques <edrdo@netcabo.pt>
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wxrc.h"

#include "wxc_project_metadata.h"

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(ArrayOfXRCWidgetData)
WX_DEFINE_OBJARRAY(ArrayOfXRCWndClassData)

int wxcXmlResourceCmp::Run(const wxString& inXrcFile, const wxString& outputCppFile, const wxString& functionName)
{
    m_outputCppFile = outputCppFile;
    m_xrcFile = inXrcFile;
    m_functionName = functionName;
    m_retCode = 0;
    m_outputPath = wxFileName(m_outputCppFile).GetPath();
    CompileRes();
    return m_retCode;
}

void wxcXmlResourceCmp::CompileRes()
{
    wxArrayString files = PrepareTempFiles();

    wxRemoveFile(m_outputCppFile);

    if(!m_retCode) {
        MakePackageCPP(files);
    }
    DeleteTempFiles(files);
}

wxString wxcXmlResourceCmp::GetInternalFileName(const wxString& name, const wxArrayString& flist)
{
    wxString name2 = name;
    name2.Replace(wxT(":"), wxT("_"));
    name2.Replace(wxT("/"), wxT("_"));
    name2.Replace(wxT("\\"), wxT("_"));
    name2.Replace(wxT("*"), wxT("_"));
    name2.Replace(wxT("?"), wxT("_"));

    wxString s = wxFileNameFromPath(m_outputCppFile) + wxT("$") + name2;

    if(wxFileExists(s) && flist.Index(s) == wxNOT_FOUND) {
        for(int i = 0;; i++) {
            s.Printf(wxFileNameFromPath(m_outputCppFile) + wxT("$%03i-") + name2, i);
            if(!wxFileExists(s) || flist.Index(s) != wxNOT_FOUND)
                break;
        }
    }
    return s;
}

wxArrayString wxcXmlResourceCmp::PrepareTempFiles()
{
    wxArrayString flist;
    wxXmlDocument doc;

    if(!doc.Load(m_xrcFile)) {
        m_retCode = 1;
        return wxArrayString();
    }

    wxString name, ext, path;
    wxFileName::SplitPath(m_xrcFile, &path, &name, &ext);
    FindFilesInXML(doc.GetRoot(), flist, path);
    wxString internalName = GetInternalFileName(m_xrcFile, flist);

    doc.Save(m_outputPath + wxFILE_SEP_PATH + internalName);
    flist.Add(internalName);

    return flist;
}

// Does 'node' contain filename information at all?
static bool NodeContainsFilename(wxXmlNode* node)
{
    const wxString name = node->GetName();

    // Any bitmaps (bitmap2 is used for disabled toolbar buttons):
    if(name == wxT("bitmap") || name == wxT("bitmap2"))
        return true;

    if(name == wxT("icon"))
        return true;

    // wxBitmapButton:
    wxXmlNode* parent = node->GetParent();
    if(parent != NULL && parent->GetAttribute(wxT("class"), wxT("")) == wxT("wxBitmapButton") &&
       (name == wxT("focus") || name == wxT("disabled") || name == wxT("hover") || name == wxT("selected")))
        return true;

    // wxBitmap or wxIcon toplevel resources:
    if(name == wxT("object")) {
        wxString klass = node->GetAttribute(wxT("class"), wxEmptyString);
        if(klass == wxT("wxBitmap") || klass == wxT("wxIcon") || klass == wxT("data"))
            return true;
    }

    // URLs in wxHtmlWindow:
    if(name == wxT("url") && parent != NULL && parent->GetAttribute(wxT("class"), wxT("")) == wxT("wxHtmlWindow")) {
        // FIXME: this is wrong for e.g. http:// URLs
        return true;
    }

    return false;
}

// find all files mentioned in structure, e.g. <bitmap>filename</bitmap>
void wxcXmlResourceCmp::FindFilesInXML(wxXmlNode* node, wxArrayString& flist, const wxString& inputPath)
{
    // Is 'node' XML node element?
    if(node == NULL)
        return;
    if(node->GetType() != wxXML_ELEMENT_NODE)
        return;

    bool containsFilename = NodeContainsFilename(node);

    wxXmlNode* n = node->GetChildren();
    while(n) {
        if(containsFilename && (n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE)) {
            wxString fullname;
            if(wxIsAbsolutePath(n->GetContent()) || inputPath.empty())
                fullname = n->GetContent();
            else
                fullname = inputPath + wxFILE_SEP_PATH + n->GetContent();

            wxString filename = GetInternalFileName(n->GetContent(), flist);
            n->SetContent(filename);

            if(flist.Index(filename) == wxNOT_FOUND)
                flist.Add(filename);

            wxFileInputStream sin(fullname);
            wxFileOutputStream sout(m_outputPath + wxFILE_SEP_PATH + filename);
            sin.Read(sout); // copy the stream
        }

        // subnodes:
        if(n->GetType() == wxXML_ELEMENT_NODE)
            FindFilesInXML(n, flist, inputPath);

        n = n->GetNext();
    }
}

void wxcXmlResourceCmp::DeleteTempFiles(const wxArrayString& flist)
{
    for(size_t i = 0; i < flist.GetCount(); i++)
        wxRemoveFile(m_outputPath + wxFILE_SEP_PATH + flist[i]);
}

static wxString FileToCppArray(wxString filename, int num)
{
    wxString output;
    wxString tmp;
    wxString snum;
    wxFFile file(filename, wxT("rb"));
    wxFileOffset offset = file.Length();
    wxASSERT_MSG(offset >= 0, wxT("Invalid file length"));

    const size_t lng = wx_truncate_cast(size_t, offset);
    wxASSERT_MSG(static_cast<wxFileOffset>(lng) == offset, wxT("Huge file not supported"));

    snum.Printf(wxT("%i"), num);
    output.Printf(wxT("static size_t xml_res_size_") + snum + wxT(" = %lu;\n"), static_cast<unsigned long>(lng));
    output += wxT("static unsigned char xml_res_file_") + snum + wxT("[] = {\n");
    // we cannot use string literals because MSVC is dumb wannabe compiler
    // with arbitrary limitation to 2048 strings :(

    unsigned char* buffer = new unsigned char[lng];
    file.Read(buffer, lng);

    for(size_t i = 0, linelng = 0; i < lng; i++) {
        tmp.Printf(wxT("%i"), buffer[i]);
        if(i != 0)
            output << wxT(',');
        if(linelng > 70) {
            linelng = 0;
            output << wxT("\n");
        }
        output << tmp;
        linelng += tmp.Length() + 1;
    }

    delete[] buffer;

    output += wxT("};\n\n");

    return output;
}

void wxcXmlResourceCmp::MakePackageCPP(const wxArrayString& flist)
{
    wxFFile file(m_outputCppFile, wxT("wt"));
    unsigned i;

    file.Write(""
               "//\n"
               "// This file was automatically generated by wxrc, do not edit by hand.\n"
               "//\n\n"
               "#include <wx/wxprec.h>\n"
               "\n"
               "#ifdef __BORLANDC__\n"
               "    #pragma hdrstop\n"
               "#endif\n"
               "\n"
               ""
               "#include <wx/filesys.h>\n"
               "#include <wx/fs_mem.h>\n"
               "#include <wx/xrc/xmlres.h>\n"
               "#include <wx/xrc/xh_all.h>\n"
               "\n"
               "#if wxCHECK_VERSION(2,8,5) && wxABI_VERSION >= 20805\n"
               "    #define XRC_ADD_FILE(name, data, size, mime) \\\n"
               "        wxMemoryFSHandler::AddFileWithMimeType(name, data, size, mime)\n"
               "#else\n"
               "    #define XRC_ADD_FILE(name, data, size, mime) \\\n"
               "        wxMemoryFSHandler::AddFile(name, data, size)\n"
               "#endif\n"
               "\n");

    for(i = 0; i < flist.GetCount(); i++)
        file.Write(FileToCppArray(m_outputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write(""
               "void " +
               m_functionName +
               "()\n"
               "{\n"
               "\n"
               "    // Check for memory FS. If not present, load the handler:\n"
               "    {\n"
               "        wxMemoryFSHandler::AddFile(wxT(\"XRC_resource/dummy_file\"), wxT(\"dummy one\"));\n"
               "        wxFileSystem fsys;\n"
               "        wxFSFile *f = fsys.OpenFile(wxT(\"memory:XRC_resource/dummy_file\"));\n"
               "        wxMemoryFSHandler::RemoveFile(wxT(\"XRC_resource/dummy_file\"));\n"
               "        if (f) delete f;\n");
    if(wxcProjectMetadata::Get().IsAddHandlers()) {
        file.Write("        else wxFileSystem::AddHandler(new wxMemoryFSHandlerBase);\n");
    }
    file.Write("    }\n\n");
    for(i = 0; i < flist.GetCount(); i++) {
        wxString s;

        wxString mime;
        wxString ext = wxFileName(flist[i]).GetExt();
        if(ext.Lower() == wxT("xrc"))
            mime = wxT("text/xml");
#if wxUSE_MIMETYPE
        else {
            wxFileType* ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
            if(ft) {
                ft->GetMimeType(&mime);
                delete ft;
            }
        }
#endif // wxUSE_MIMETYPE

        s.Printf("    XRC_ADD_FILE(wxT(\"XRC_resource/" + flist[i] +
                     "\"), xml_res_file_%u, xml_res_size_%u, wxT(\"%s\"));\n",
                 i, i, mime.c_str());
        file.Write(s);
    }

    file.Write("    wxXmlResource::Get()->Load(wxT(\"memory:XRC_resource/" + GetInternalFileName(m_xrcFile, flist) +
               "\"));\n");
    file.Write("}\n");
}

void wxcXmlResourceCmp::GenCPPHeader()
{
    // Generate the output header in the same directory as the source file.
    wxFileName headerName(m_outputCppFile);
    headerName.SetExt(wxcProjectMetadata::Get().GetHeaderFileExt());

    wxFFile file(headerName.GetFullPath(), wxT("wt"));
    file.Write("//\n"
               "// This file was automatically generated by wxrc, do not edit by hand.\n"
               "//\n\n"
               "#ifndef __" +
               headerName.GetName() + "_" + headerName.GetExt() +
               "__\n"
               "#define __" +
               headerName.GetName() + "_" + headerName.GetExt() + "__\n");
    for(size_t i = 0; i < aXRCWndClassData.GetCount(); ++i) {
        aXRCWndClassData.Item(i).GenerateHeaderCode(file);
    }
    file.Write("\nvoid \n" + m_functionName + "();\n#endif\n");
}

static wxString FileToPythonArray(wxString filename, int num)
{
    wxString output;
    wxString tmp;
    wxString snum;
    wxFFile file(filename, wxT("rb"));
    wxFileOffset offset = file.Length();
    wxASSERT_MSG(offset >= 0, wxT("Invalid file length"));

    const size_t lng = wx_truncate_cast(size_t, offset);
    wxASSERT_MSG(static_cast<wxFileOffset>(lng) == offset, wxT("Huge file not supported"));

    snum.Printf(wxT("%i"), num);
    output = "    xml_res_file_" + snum + " = '''\\\n";

    unsigned char* buffer = new unsigned char[lng];
    file.Read(buffer, lng);

    for(size_t i = 0, linelng = 0; i < lng; i++) {
        unsigned char c = buffer[i];
        if(c == '\n') {
            tmp = (wxChar)c;
            linelng = 0;
        } else if(c < 32 || c > 127 || c == '\'')
            tmp.Printf(wxT("\\x%02x"), c);
        else if(c == '\\')
            tmp = wxT("\\\\");
        else
            tmp = (wxChar)c;
        if(linelng > 70) {
            linelng = 0;
            output << wxT("\\\n");
        }
        output << tmp;
        linelng += tmp.Length();
    }

    delete[] buffer;

    output += wxT("'''\n\n");

    return output;
}

void wxcXmlResourceCmp::MakePackagePython(const wxArrayString& flist)
{
    wxFFile file(m_outputCppFile, wxT("wt"));
    unsigned i;

    file.Write("#\n"
               "# This file was automatically generated by wxrc, do not edit by hand.\n"
               "#\n\n"
               "import wx\n"
               "import wx.xrc\n\n");

    file.Write("def " + m_functionName + "():\n");

    for(i = 0; i < flist.GetCount(); i++)
        file.Write(FileToPythonArray(m_outputPath + wxFILE_SEP_PATH + flist[i], i));

    file.Write("    # check if the memory filesystem handler has been loaded yet, and load it if not\n"
               "    wx.MemoryFSHandler.AddFile('XRC_resource/dummy_file', 'dummy value')\n"
               "    fsys = wx.FileSystem()\n"
               "    f = fsys.OpenFile('memory:XRC_resource/dummy_file')\n"
               "    wx.MemoryFSHandler.RemoveFile('XRC_resource/dummy_file')\n"
               "    if f is not None:\n"
               "        f.Destroy()\n"
               "    else:\n"
               "        wx.FileSystem.AddHandler(wx.MemoryFSHandler())\n"
               "\n"
               "    # load all the strings as memory files and load into XmlRes\n");

    for(i = 0; i < flist.GetCount(); i++) {
        wxString s;
        s.Printf("    wx.MemoryFSHandler.AddFile('XRC_resource/" + flist[i] + "', xml_res_file_%u)\n", i);
        file.Write(s);
    }
    file.Write("    wx.xrc.XmlResource.Get().Load('memory:XRC_resource/" + GetInternalFileName(m_xrcFile, flist) +
               "')\n");
    file.Write("\n");
}

void wxcXmlResourceCmp::OutputGettext()
{
    ExtractedStrings str = FindStrings();

    wxFFile fout;
    if(m_outputCppFile.empty())
        fout.Attach(stdout);
    else
        fout.Open(m_outputCppFile, wxT("wt"));

    for(ExtractedStrings::const_iterator i = str.begin(); i != str.end(); ++i) {
        const wxFileName filename(i->filename);

        wxString s;
        s.Printf("#line %d \"%s\"\n", i->lineNo, filename.GetFullPath(wxPATH_UNIX));

        fout.Write(s);
        fout.Write("_(\"" + i->str + "\");\n");
    }

    if(!m_outputCppFile)
        fout.Detach();
}

ExtractedStrings wxcXmlResourceCmp::FindStrings()
{
    ExtractedStrings arr, a2;

    wxXmlDocument doc;
    if(!doc.Load(m_xrcFile)) {
        m_retCode = 1;
        return arr;
    }
    a2 = FindStrings(m_xrcFile, doc.GetRoot());

    WX_APPEND_ARRAY(arr, a2);
    return arr;
}
#if 0
static wxString ConvertText(const wxString& str)
{
    wxString str2;
    const wxChar *dt;

    for (dt = str.c_str(); *dt; dt++) {
        if (*dt == wxT('_')) {
            if ( *(dt+1) == 0 )
                str2 << wxT('_');
            else if ( *(++dt) == wxT('_') )
                str2 << wxT('_');
            else
                str2 << wxT('&') << *dt;
        } else {
            switch (*dt) {
            case wxT('\n') :
                str2 << wxT("\\n");
                break;
            case wxT('\t') :
                str2 << wxT("\\t");
                break;
            case wxT('\r') :
                str2 << wxT("\\r");
                break;
            case wxT('\\') :
                if ((*(dt+1) != 'n') &&
                    (*(dt+1) != 't') &&
                    (*(dt+1) != 'r'))
                    str2 << wxT("\\\\");
                else
                    str2 << wxT("\\");
                break;
            case wxT('"')  :
                str2 << wxT("\\\"");
                break;
            default        :
                str2 << *dt;
                break;
            }
        }
    }

    return str2;
}
#endif

ExtractedStrings wxcXmlResourceCmp::FindStrings(const wxString& filename, wxXmlNode* node)
{
    ExtractedStrings arr;

    wxXmlNode* n = node;
    if(n == NULL)
        return arr;
    n = n->GetChildren();

    while(n) {
        if((node->GetType() == wxXML_ELEMENT_NODE) &&
           // parent is an element, i.e. has subnodes...
           (n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE) &&
           // ...it is textnode...
           (node /*not n!*/->GetName() == wxT("label") ||
            (node /*not n!*/->GetName() == wxT("value") && !n->GetContent().IsNumber()) ||
            node /*not n!*/->GetName() == wxT("help") || node /*not n!*/->GetName() == wxT("longhelp") ||
            node /*not n!*/->GetName() == wxT("tooltip") || node /*not n!*/->GetName() == wxT("htmlcode") ||
            node /*not n!*/->GetName() == wxT("title") || node /*not n!*/->GetName() == wxT("item")))
            // ...and known to contain translatable string
            // subnodes:
            if(n->GetType() == wxXML_ELEMENT_NODE) {
                ExtractedStrings a2 = FindStrings(filename, n);
                WX_APPEND_ARRAY(arr, a2);
            }

        n = n->GetNext();
    }
    return arr;
}
