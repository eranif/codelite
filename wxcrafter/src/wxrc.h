#ifndef WXRC_H
#define WXRC_H

#include <wx/cmdline.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/hashset.h>
#include <wx/mimetype.h>
#include <wx/utils.h>
#include <wx/vector.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>

WX_DECLARE_HASH_SET(wxString, ::wxStringHash, ::wxStringEqual, StringSet);

class XRCWidgetData
{
public:
    XRCWidgetData(const wxString& vname, const wxString& vclass)
        : m_class(vclass)
        , m_name(vname)
    {
    }
    const wxString& GetName() const { return m_name; }
    const wxString& GetClass() const { return m_class; }

private:
    wxString m_class;
    wxString m_name;
};
WX_DECLARE_OBJARRAY(XRCWidgetData, ArrayOfXRCWidgetData);

struct ExtractedString {
    ExtractedString()
        : lineNo(-1)
    {
    }
    ExtractedString(const wxString& str_, const wxString& filename_, int lineNo_)
        : str(str_)
        , filename(filename_)
        , lineNo(lineNo_)
    {
    }

    wxString str;

    wxString filename;
    int lineNo;
};
typedef wxVector<ExtractedString> ExtractedStrings;

class XRCWndClassData
{
private:
    wxString m_className;
    wxString m_parentClassName;
    StringSet m_ancestorClassNames;
    ArrayOfXRCWidgetData m_wdata;

    void BrowseXmlNode(wxXmlNode* node)
    {
        wxString classValue;
        wxString nameValue;
        wxXmlNode* children;
        while(node) {
            if(node->GetName() == wxT("object") && node->GetAttribute(wxT("class"), &classValue) &&
               node->GetAttribute(wxT("name"), &nameValue)) {
                m_wdata.Add(XRCWidgetData(nameValue, classValue));
            }
            children = node->GetChildren();
            if(children) BrowseXmlNode(children);
            node = node->GetNext();
        }
    }

public:
    XRCWndClassData(const wxString& className, const wxString& parentClassName, const wxXmlNode* node)
        : m_className(className)
        , m_parentClassName(parentClassName)
    {
        if(className == wxT("wxMenu")) {
            m_ancestorClassNames.insert(wxT("wxMenu"));
            m_ancestorClassNames.insert(wxT("wxMenuBar"));
        } else if(className == wxT("wxMDIChildFrame")) {
            m_ancestorClassNames.insert(wxT("wxMDIParentFrame"));
        } else if(className == wxT("wxMenuBar") || className == wxT("wxStatusBar") || className == wxT("wxToolBar")) {
            m_ancestorClassNames.insert(wxT("wxFrame"));
        } else {
            m_ancestorClassNames.insert(wxT("wxWindow"));
        }

        BrowseXmlNode(node->GetChildren());
    }

    const ArrayOfXRCWidgetData& GetWidgetData() { return m_wdata; }

    bool CanBeUsedWithXRCCTRL(const wxString& name)
    {
        if(name == wxT("tool") || name == wxT("data") || name == wxT("unknown") || name == wxT("notebookpage") ||
           name == wxT("separator") || name == wxT("sizeritem") || name == wxT("wxMenu") || name == wxT("wxMenuBar") ||
           name == wxT("wxMenuItem") || name.EndsWith(wxT("Sizer"))) {
            return false;
        }
        return true;
    }

    void GenerateHeaderCode(wxFFile& file)
    {

        file.Write(wxT("class ") + m_className + wxT(" : public ") + m_parentClassName + wxT(" {\nprotected:\n"));
        size_t i;
        for(i = 0; i < m_wdata.GetCount(); ++i) {
            const XRCWidgetData& w = m_wdata.Item(i);
            if(!CanBeUsedWithXRCCTRL(w.GetClass())) continue;
            if(w.GetName().empty()) continue;
            file.Write(wxT(" ") + w.GetClass() + wxT("* ") + w.GetName() + wxT(";\n"));
        }
        file.Write(wxT("\nprivate:\n void InitWidgetsFromXRC(wxWindow *parent){\n")
                       wxT("  wxXmlResource::Get()->LoadObject(this,parent,wxT(\"") +
                   m_className + wxT("\"), wxT(\"") + m_parentClassName + wxT("\"));\n"));
        for(i = 0; i < m_wdata.GetCount(); ++i) {
            const XRCWidgetData& w = m_wdata.Item(i);
            if(!CanBeUsedWithXRCCTRL(w.GetClass())) continue;
            if(w.GetName().empty()) continue;
            file.Write(wxT("  ") + w.GetName() + wxT(" = XRCCTRL(*this,\"") + w.GetName() + wxT("\",") + w.GetClass() +
                       wxT(");\n"));
        }
        file.Write(wxT(" }\n"));

        file.Write(wxT("public:\n"));

        if(m_ancestorClassNames.size() == 1) {
            file.Write(m_className + wxT("(") + *m_ancestorClassNames.begin() + wxT(" *parent=NULL){\n") +
                       wxT("  InitWidgetsFromXRC((wxWindow *)parent);\n") wxT(" }\n") wxT("};\n"));
        } else {
            file.Write(m_className + wxT("(){\n") + wxT("  InitWidgetsFromXRC(NULL);\n") wxT(" }\n") wxT("};\n"));

            for(StringSet::const_iterator it = m_ancestorClassNames.begin(); it != m_ancestorClassNames.end(); ++it) {
                file.Write(m_className + wxT("(") + *it + wxT(" *parent){\n") +
                           wxT("  InitWidgetsFromXRC((wxWindow *)parent);\n") wxT(" }\n") wxT("};\n"));
            }
        }
    }
};

WX_DECLARE_OBJARRAY(XRCWndClassData, ArrayOfXRCWndClassData);

class wxcXmlResourceCmp
{
    wxString m_outputCppFile;
    wxString m_functionName;
    wxString m_xrcFile;
    int m_retCode;
    wxString m_outputPath;

public:
    // don't use builtin cmd line parsing:
    virtual bool OnInit() { return true; }
    virtual int Run(const wxString& inXrcFile, const wxString& outputCppFile, const wxString& functionName);

private:
    void CompileRes();
    wxArrayString PrepareTempFiles();
    void FindFilesInXML(wxXmlNode* node, wxArrayString& flist, const wxString& inputPath);

    wxString GetInternalFileName(const wxString& name, const wxArrayString& flist);
    void DeleteTempFiles(const wxArrayString& flist);
    void MakePackageCPP(const wxArrayString& flist);
    void MakePackagePython(const wxArrayString& flist);

    void OutputGettext();
    ExtractedStrings FindStrings();
    ExtractedStrings FindStrings(const wxString& filename, wxXmlNode* node);

    ArrayOfXRCWndClassData aXRCWndClassData;
    void GenCPPHeader();
};
#endif
