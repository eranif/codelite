#include "wxc_bitmap_code_generator.h"

#include "event_notifier.h"
#include "top_level_win_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxc_settings.h"
#include "wxgui_helpers.h"
#include "wxrc.h"

#include <wx/log.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

const wxEventType wxEVT_BITMAP_CODE_GENERATION_DONE = ::wxNewEventType();

wxcCodeGeneratorHelper::wxcCodeGeneratorHelper()
{
    // m_bmpGenThread.Start();
}

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

clStatus wxcCodeGeneratorHelper::PrepareBitmapsXrc(std::function<void()> requestDesignerRefresh,
                                                   wxXmlDocument& doc,
                                                   wxString& outputString,
                                                   bool& isBitmapModifiedOutside)
{
    wxString text;
    text << wxT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
    text << wxT("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\">");
    if (wxcProjectMetadata::Get().IsAddHandlers()) {
        text << "<!-- Handler Generation is ON -->\n";
    } else {
        text << "<!-- Handler Generation is OFF -->\n";
    }

    // Supported hi-res images extension
    const wxArrayString exts = {"@2x", "@1.25x", "@1.5x"};

    for (const auto& p : m_bitmapMap) {
        wxFileName fn(p.second);
        text << wxT("<object class=\"wxBitmap\" name=\"") << p.first << wxT("\">") << fn.GetFullPath()
             << wxT("</object>\n");
        // Support for hi-res images
        // The logic:
        // If we find files with the following prefixes: @2x, @1.5x, @1.25x
        // add them to the resource files as well
        for (const auto& ext : exts) {
            wxFileName hiResImage = fn;
            hiResImage.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
            hiResImage.SetName(hiResImage.GetName() + ext);
            if (hiResImage.FileExists()) {
                fn.SetName(hiResImage.GetName());
                text << "<object class=\"wxBitmap\" name=\"" << p.first << ext << "\">" << fn.GetFullPath()
                     << "</object>\n";
            }
        }
    }
    text << wxT("</resource>");

    wxStringInputStream str(text);
    doc.Load(str);

    wxFileName projectFileName(wxcProjectMetadata::Get().GetProjectFile());
    m_cppFile = wxFileName(wxcProjectMetadata::Get().GetProjectPath(), wxcProjectMetadata::Get().GetOutputFileName());
    m_cppFile.SetFullName(wxcProjectMetadata::Get().GetBitmapsFile());
    m_cppFile.SetExt("cpp");

    m_xrcFile = m_cppFile;
    m_xrcFile.SetExt(wxT("xrc"));

    outputString.Clear();
    wxStringOutputStream outStream(&outputString);
    if (!doc.Save(outStream)) {
        return StatusIOError();
    }

    // The bitmaps file (used when the code is not embedded in the base class file)
    m_destCPP =
        wxFileName(wxcProjectMetadata::Get().GetGeneratedFilesDir(), wxcProjectMetadata::Get().GetBitmapsFile());
    m_destCPP.SetExt("cpp");
    wxCrafter::MakeAbsToProject(m_destCPP);

    isBitmapModifiedOutside = IsGenerateNeeded();
    if (isBitmapModifiedOutside && requestDesignerRefresh) {
        requestDesignerRefresh();
    }
    return {};
}

clStatus wxcCodeGeneratorHelper::CreateXRC(std::function<void()> requestDesignerRefresh,
                                           std::function<void()> bitmapGenerationStart,
                                           std::function<void()> bitmapGenerationEnd,
                                           std::function<void(const wxFileName&)> onFileSaved)
{
    wxLogNull noLog;

    wxXmlDocument doc;
    wxString outputString;
    bool isBitmapModifiedOutside{false};
    const auto prepare_status = PrepareBitmapsXrc(requestDesignerRefresh, doc, outputString, isBitmapModifiedOutside);
    if (!prepare_status.ok()) {
        return prepare_status;
    }

    if (wxCrafter::IsTheSame(outputString, m_xrcFile) && m_destCPP.FileExists() && !isBitmapModifiedOutside) {
        // the XRC used to generate the file is the same as the new one
        // and we got a CPP file - skip the code generation
        wxCommandEvent eventEnd(wxEVT_BITMAP_CODE_GENERATION_DONE);
        eventEnd.SetString(m_destCPP.GetFullPath());
        EventNotifier::Get()->AddPendingEvent(eventEnd);
        return {};
    }

    if (!doc.Save(m_xrcFile.GetFullPath())) {
        return StatusIOError(m_xrcFile.GetFullPath());
    }

    {
        wxString cppFile = m_destCPP.GetFullPath();
        wxLogNull nl;
        if (bitmapGenerationStart) {
            bitmapGenerationStart();
        }

        wxcXmlResourceCmp cmp;
        const auto retCode = cmp.Run(m_xrcFile.GetFullPath(),                        // Input XRC file
                                     cppFile,                                        // Output file (our CPP file)
                                     wxcProjectMetadata::Get().GetBitmapFunction()); // The function name to generate

        if (onFileSaved) {
            onFileSaved(cppFile);
        }
        wxCommandEvent eventEnd(wxEVT_BITMAP_CODE_GENERATION_DONE);
        eventEnd.SetString(cppFile);
        EventNotifier::Get()->AddPendingEvent(eventEnd);
        if (bitmapGenerationEnd) {
            bitmapGenerationEnd();
        }
        if (!retCode.ok()) {
            return retCode;
        }
    }

    return {};
}

clStatusOr<wxString> wxcCodeGeneratorHelper::GenerateBitmapsCode(std::function<void()> requestDesignerRefresh,
                                                                 std::function<void()> bitmapGenerationStart,
                                                                 std::function<void()> bitmapGenerationEnd)
{
    wxLogNull noLog;

    wxXmlDocument doc;
    wxString outputString;
    bool isBitmapModifiedOutside{false};
    const auto prepare_status = PrepareBitmapsXrc(requestDesignerRefresh, doc, outputString, isBitmapModifiedOutside);
    if (!prepare_status.ok()) {
        return prepare_status;
    }

    // The code is going to be placed in the base class source file, it is the "host" of the code.
    // The host file is never written here, but the resource compiler creates temporary files next to it.
    wxFileName hostCppFile = wxcProjectMetadata::Get().BaseCppFile();
    wxCrafter::MakeAbsToProject(hostCppFile);
    hostCppFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // The XRC file is the input of the resource compiler. Update it only when needed, so its timestamp
    // can be used to find bitmap files that were modified outside wxCrafter
    if (isBitmapModifiedOutside || !wxCrafter::IsTheSame(outputString, m_xrcFile)) {
        if (!doc.Save(m_xrcFile.GetFullPath())) {
            return StatusIOError(m_xrcFile.GetFullPath());
        }
    }

    if (bitmapGenerationStart) {
        bitmapGenerationStart();
    }

    wxcXmlResourceCmp cmp;
    auto code = cmp.RunEmbedded(
        m_xrcFile.GetFullPath(), hostCppFile.GetFullPath(), wxcProjectMetadata::Get().GetBitmapFunction());

    if (bitmapGenerationEnd) {
        bitmapGenerationEnd();
    }
    return code;
}

bool wxcCodeGeneratorHelper::EmbedBitmapsInBaseClassFile() const
{
    // Without a base class source file, there is no place for the code
    return wxcProjectMetadata::Get().GetGenerateCPPCode() &&
           wxcSettings::Get().HasFlag(wxcSettings::BITMAPS_IN_BASE_CLASS_FILE);
}

wxString wxcCodeGeneratorHelper::GenerateInitCode(TopLevelWinWrapper* tw) const
{
    wxString code;
    code << "    if ( !bBitmapLoaded ) {\n"
         << "        // We need to initialise the default bitmap handler\n";
    if (wxcProjectMetadata::Get().IsAddHandlers()) {
        code << "        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);\n";
    }
    code << "        " << wxcProjectMetadata::Get().GetBitmapFunction() << "();\n"
         << "        bBitmapLoaded = true;\n"
         << "    }\n";

    if (tw->HasIcon()) {
        wxString appIconCode = GenerateTopLevelWindowIconCode();
        if (!appIconCode.IsEmpty()) {
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

    if (tmpbmp.IsEmpty()) {
        return wxT("wxNullBitmap");
    }

    wxString artId, clientId, sizeHint;
    wxString code;
    if (wxCrafter::IsArtProviderBitmap(bmp, artId, clientId, sizeHint)) {
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
    if (bmppath.IsEmpty())
        return "";

    wxString artId, clientId, sizeHint;
    if (wxCrafter::IsArtProviderBitmap(bmppath, artId, clientId, sizeHint)) {
        return "";
    }

    wxFileName fn(bmppath);
    wxString bmpname;

    // set the name + remove old entry
    name.IsEmpty() ? bmpname = fn.GetName() : bmpname = name;
    if (m_bitmapMap.count(bmpname)) {
        m_bitmapMap.erase(bmpname);
    }

    m_bitmapMap.insert(std::make_pair(bmpname, bmppath));
    return bmpname;
}

bool wxcCodeGeneratorHelper::IsGenerateNeeded() const
{
    if (!m_xrcFile.FileExists()) {
        return true;
    }

    wxString basepath = wxcProjectMetadata::Get().GetProjectPath();
    time_t xrcModTime = m_xrcFile.GetModificationTime().GetTicks();

    for (const auto& p : m_bitmapMap) {
        wxFileName bmpFile(p.second);
        if (bmpFile.MakeAbsolute(basepath)) {
            if (bmpFile.FileExists()) {
                time_t bmpMod = bmpFile.GetModificationTime().GetTicks();
                if (bmpMod > xrcModTime) {
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
    if (m_winIds.empty() || !wxcProjectMetadata::Get().IsUseEnum()) {
        return "";
    }

    int firstId = wxcProjectMetadata::Get().GetFirstWindowId();
    wxString enumCode;
    enumCode << "public:\n"
             << "    enum {\n";
    for (const auto& winId : m_winIds) {
        enumCode << "        " << winId << " = " << ++firstId << ",\n";
    }
    enumCode << "    };\n";
    return enumCode;
}

void wxcCodeGeneratorHelper::AddIcon(const wxString& bitmapFile)
{
    if (bitmapFile.IsEmpty()) {
        return;
    }
    wxString bmpadded = AddBitmap(bitmapFile);
    if (bmpadded.IsEmpty())
        return;
    m_icons.Add(bmpadded);
}

wxString wxcCodeGeneratorHelper::GenerateTopLevelWindowIconCode() const
{
    wxString code;

    if (!m_icons.IsEmpty()) {
        code << "    // Set icon(s) to the application/dialog\n";
        code << "    wxIconBundle app_icons;\n";
        for (size_t i = 0; i < m_icons.GetCount(); ++i) {
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
