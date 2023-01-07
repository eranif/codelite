#include "wxc_bitmap_code_generator.h"
#include "DirectoryChanger.h"
#include "top_level_win_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include "wxrc.h"
#include <environmentconfig.h>
#include <globals.h>
#include <processreaderthread.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/xml/xml.h>

const wxEventType wxEVT_BITMAP_CODE_GENERATION_DONE = ::wxNewEventType();

wxcCodeGeneratorHelper::wxcCodeGeneratorHelper()
{
    // m_bmpGenThread.Start();
}

wxcCodeGeneratorHelper::~wxcCodeGeneratorHelper() {}

wxcCodeGeneratorHelper& wxcCodeGeneratorHelper::Get()
{
    static wxcCodeGeneratorHelper mgr;
    return mgr;
}

void wxcCodeGeneratorHelper::Clear()
{
    m_bitmapMap.clear();
    m_wxrcOutput.Clear();
    m_winIds.clear();
    m_icons.Clear();
}

bool wxcCodeGeneratorHelper::CreateXRC()
{
    wxLogNull noLog;

    wxString text;
    text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
    text << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\">");
    if(wxcProjectMetadata::Get().IsAddHandlers()) {
        text << "<!-- Handler Generation is ON -->\n";
    } else {
        text << "<!-- Handler Generation is OFF -->\n";
    }
    MapString_t::const_iterator iter = m_bitmapMap.begin();

    // Supported hi-res images extension
    wxArrayString exts;
    exts.Add("@2x");
    exts.Add("@1.25x");
    exts.Add("@1.5x");

    for(; iter != m_bitmapMap.end(); ++iter) {
        wxFileName fn(iter->second);
        text << wxT("<object class=\"wxBitmap\" name=\"") << iter->first << wxT("\">") << fn.GetFullPath()
             << wxT("</object>\n");
        // Support for hi-res images
        // The logic:
        // If we find files with the following perfixes: @2x, @1.5x, @1.25x
        // add them to the resource files as well
        for(size_t i = 0; i < exts.size(); ++i) {
            wxFileName hiResImage = fn;
            hiResImage.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
            hiResImage.SetName(hiResImage.GetName() + exts.Item(i));
            if(hiResImage.FileExists()) {
                fn.SetName(hiResImage.GetName());
                text << "<object class=\"wxBitmap\" name=\"" << iter->first << exts.Item(i) << "\">" << fn.GetFullPath()
                     << "</object>\n";
            }
        }
    }
    text << wxT("</resource>");

    wxStringInputStream str(text);
    wxXmlDocument doc(str);

    wxFileName projectFileName(wxcProjectMetadata::Get().GetProjectFile());
    m_cppFile = wxFileName(wxcProjectMetadata::Get().GetProjectPath(), wxcProjectMetadata::Get().GetOutputFileName());
    m_cppFile.SetFullName(wxcProjectMetadata::Get().GetBitmapsFile());
    m_cppFile.SetExt("cpp");

    m_xrcFile = m_cppFile;
    m_xrcFile.SetExt(wxT("xrc"));

    wxString outputString;
    wxStringOutputStream outStream(&outputString);

    if(!doc.Save(outStream)) { return false; }

    // Check to see if we already got
    m_destCPP =
        wxFileName(wxcProjectMetadata::Get().GetGeneratedFilesDir(), wxcProjectMetadata::Get().GetBitmapsFile());
    m_destCPP.SetExt("cpp");
    wxCrafter::MakeAbsToProject(m_destCPP);

    bool isBitmapModifiedOutside = IsGenerateNeeded();

    if(isBitmapModifiedOutside) {
        // Request for update
        wxCommandEvent evtUpdateDesigner(wxEVT_REFRESH_DESIGNER);
        EventNotifier::Get()->AddPendingEvent(evtUpdateDesigner);
    }

#if !defined(__WXMAC__)
    if(wxCrafter::IsTheSame(outputString, m_xrcFile) && m_destCPP.FileExists() && !isBitmapModifiedOutside) {
        // the XRC used to generate the file is the same as the new one
        // and we got a CPP file - skip the code generation
        wxCommandEvent eventEnd(wxEVT_BITMAP_CODE_GENERATION_DONE);
        eventEnd.SetString(m_destCPP.GetFullPath());
        EventNotifier::Get()->AddPendingEvent(eventEnd);
        return true;
    }
#endif
    if(!doc.Save(m_xrcFile.GetFullPath())) { return false; }

    {
        wxFrame* topFrame = EventNotifier::Get()->TopFrame();

        topFrame->SetStatusText("Generating bitmap code...");
        wxBusyCursor bc;
        wxString cppFile = m_destCPP.GetFullPath();
        wxLogNull nl;
        wxcXmlResourceCmp cmp;
        cmp.Run(m_xrcFile.GetFullPath(),                        // Input XRC file
                cppFile,                                        // Output file (our CPP file)
                wxcProjectMetadata::Get().GetBitmapFunction()); // The function name to generate

        wxCrafter::NotifyFileSaved(cppFile);
        wxCommandEvent eventEnd(wxEVT_BITMAP_CODE_GENERATION_DONE);
        eventEnd.SetString(cppFile);
        EventNotifier::Get()->AddPendingEvent(eventEnd);
        topFrame->SetStatusText("Ready");
    }

    // m_bmpGenThread.AddMessage( req );
    return true;
}

wxString wxcCodeGeneratorHelper::GenerateInitCode(TopLevelWinWrapper* tw) const
{
    wxString code;
    code << "    if ( !bBitmapLoaded ) {\n"
         << "        // We need to initialise the default bitmap handler\n";
    if(wxcProjectMetadata::Get().IsAddHandlers()) {
        code << "        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);\n";
    }
    code << "        " << wxcProjectMetadata::Get().GetBitmapFunction() << "();\n"
         << "        bBitmapLoaded = true;\n"
         << "    }\n";

    if(tw->HasIcon()) {
        wxString appIconCode = GenerateTopLevelWindowIconCode();
        if(!appIconCode.IsEmpty()) {
            // add the icon code here
            code << appIconCode << "\n";
        }
    }
    return code;
}

wxString wxcCodeGeneratorHelper::GenerateExternCode() const
{
    wxString code;
    code << wxT("extern void ") << wxcProjectMetadata::Get().GetBitmapFunction() << wxT("();\n");
    return code;
}

wxString wxcCodeGeneratorHelper::BitmapCode(const wxString& bmp, const wxString& bmpname) const
{
    wxString tmpbmp = bmp;
    tmpbmp.Trim().Trim(false);

    if(tmpbmp.IsEmpty()) { return wxT("wxNullBitmap"); }

    wxString artId, clientId, sizeHint;
    wxString code;
    if(wxCrafter::IsArtProviderBitmap(bmp, artId, clientId, sizeHint)) {
        code << "wxArtProvider::GetBitmap(" << artId << ", " << clientId << ", " << wxCrafter::MakeWxSizeStr(sizeHint)
             << ")";

    } else {
        wxFileName fn(tmpbmp);

        wxString name;
        bmpname.IsEmpty() ? name = fn.GetName() : name = bmpname;
        code << wxT("wxXmlResource::Get()->LoadBitmap(") << wxCrafter::WXT(name) << wxT(")");
    }
    return code;
}

wxString wxcCodeGeneratorHelper::AddBitmap(const wxString& bitmapFile, const wxString& name)
{
    wxString bmppath = bitmapFile;
    bmppath.Trim().Trim(false);
    if(bmppath.IsEmpty()) return "";

    wxString artId, clientId, sizeHint;
    if(wxCrafter::IsArtProviderBitmap(bmppath, artId, clientId, sizeHint)) { return ""; }

    wxFileName fn(bmppath);
    wxString bmpname;

    // set the name + remove old entry
    name.IsEmpty() ? bmpname = fn.GetName() : bmpname = name;
    if(m_bitmapMap.count(bmpname)) { m_bitmapMap.erase(bmpname); }

    m_bitmapMap.insert(std::make_pair(bmpname, bmppath));
    return bmpname;
}

bool wxcCodeGeneratorHelper::IsGenerateNeeded() const
{
    if(!m_xrcFile.FileExists()) { return true; }

    wxString basepath = wxcProjectMetadata::Get().GetProjectPath();
    time_t xrcModTime = m_xrcFile.GetModificationTime().GetTicks();

    MapString_t::const_iterator iter = m_bitmapMap.begin();
    for(; iter != m_bitmapMap.end(); ++iter) {
        wxFileName bmpFile(iter->second);
        if(bmpFile.MakeAbsolute(basepath)) {
            if(bmpFile.FileExists()) {
                time_t bmpMod = bmpFile.GetModificationTime().GetTicks();
                if(bmpMod > xrcModTime) {
                    // the bmp file is newer than the xrc file - regenerate
                    return true;
                }
            }
        }
    }
    return false;
}

void wxcCodeGeneratorHelper::AddWindowId(const wxString& winid) { m_winIds.insert(winid); }

void wxcCodeGeneratorHelper::ClearWindowIds() { m_winIds.clear(); }

wxString wxcCodeGeneratorHelper::GenerateWinIdEnum() const
{
    if(m_winIds.empty() || !wxcProjectMetadata::Get().IsUseEnum()) { return ""; }

    int firstId = wxcProjectMetadata::Get().GetFirstWindowId();
    wxString enumCode;
    enumCode << "public:\n"
             << "    enum {\n";
    wxStringSet_t::const_iterator iter = m_winIds.begin();
    for(; iter != m_winIds.end(); ++iter) {
        enumCode << "        " << *iter << " = " << ++firstId << ",\n";
    }
    enumCode << "    };\n";
    return enumCode;
}

void wxcCodeGeneratorHelper::AddIcon(const wxString& bitmapFile)
{
    if(bitmapFile.IsEmpty()) { return; }
    wxString bmpadded = AddBitmap(bitmapFile);
    if(bmpadded.IsEmpty()) return;
    m_icons.Add(bmpadded);
}

wxString wxcCodeGeneratorHelper::GenerateTopLevelWindowIconCode() const
{
    wxString code;

    if(!m_icons.IsEmpty()) {
        code << "    // Set icon(s) to the application/dialog\n";
        code << "    wxIconBundle app_icons;\n";
        for(size_t i = 0; i < m_icons.GetCount(); ++i) {
            code << "    {\n"
                 << "        wxBitmap iconBmp = " << BitmapCode(m_icons.Item(i)) << ";\n"
                 << "        wxIcon icn;\n"
                 << "        icn.CopyFromBitmap(iconBmp);\n"
                 << "        app_icons.AddIcon( icn );\n"
                 << "    }\n";
        }
        code << "    SetIcons( app_icons );\n";
    }
    return code;
}

void wxcCodeGeneratorHelper::ClearIcons()
{
    // for(size_t i=0; i<m_icons.GetCount(); ++i) {
    //    if ( m_bitmapMap.count(m_icons.Item(i)) ) {
    //        m_bitmapMap.erase(m_icons.Item(i));
    //    }
    //}
    m_icons.Clear();
}

void wxcCodeGeneratorHelper::UnInitialize()
{
    // m_bmpGenThread.Stop();
}
