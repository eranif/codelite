#include "clDiffFrame.h"

#include "DiffSideBySidePanel.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"

#include <wx/icon.h>
#include <wx/xrc/xmlres.h>

clDiffFrame::clDiffFrame(wxWindow* parent, const DiffSideBySidePanel::FileInfo& left,
                         const DiffSideBySidePanel::FileInfo& right, bool originSourceControl)
    : wxFrame(parent, wxID_ANY, _("CodeLite - Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    m_diffView->SetFilesDetails(left, right);
    m_diffView->Diff();
    if(originSourceControl) {
        m_diffView->SetOriginSourceControl();
    }
    sz->Add(m_diffView, 1, wxEXPAND, 0);
    WindowAttrManager::Load(this);

    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    CreateMenuBar();
    SetIcons(b);
    ::clSetTLWindowBestSizeAndPosition(this);
}

clDiffFrame::clDiffFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, _("CodeLite - Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    sz->Add(m_diffView, 1, wxEXPAND, 0);
    m_diffView->DiffNew();
    WindowAttrManager::Load(this);
    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    CreateMenuBar();
    SetIcons(b);
    CallAfter(&clDiffFrame::Maximize, true);
}

clDiffFrame::clDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile)
    : wxFrame(parent, wxID_ANY, _("CodeLite - Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    sz->Add(m_diffView, 1, wxEXPAND, 0);
    if(isTempFile) {
        m_diffView->SetSaveFilepaths(false);
    }

    m_diffView->DiffNew(left, right);
    WindowAttrManager::Load(this);
    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    CreateMenuBar();
    SetIcons(b);
    CallAfter(&clDiffFrame::Maximize, true);
}

clDiffFrame::~clDiffFrame() {}

void clDiffFrame::CreateMenuBar()
{
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar("diff_menu_bar"));
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnSaveChanges, m_diffView, wxID_SAVE);
    Bind(wxEVT_UPDATE_UI, &DiffSideBySidePanel::OnSaveChangesUI, m_diffView, wxID_SAVE);
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnFind, m_diffView, wxID_FIND);
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnNextDiffSequence, m_diffView, XRCID("ID_DIFF_TOOL_NEXT"));
    Bind(wxEVT_UPDATE_UI, &DiffSideBySidePanel::OnNextDiffUI, m_diffView, XRCID("ID_DIFF_TOOL_NEXT"));
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnPrevDiffSequence, m_diffView, XRCID("ID_DIFF_TOOL_PREV"));
    Bind(wxEVT_UPDATE_UI, &DiffSideBySidePanel::OnPrevDiffUI, m_diffView, XRCID("ID_DIFF_TOOL_PREV"));

    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnCopyLeftToRight, m_diffView, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
    Bind(wxEVT_UPDATE_UI, &DiffSideBySidePanel::OnCopyLeftToRightUI, m_diffView, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnCopyRightToLeft, m_diffView, XRCID("ID_DIFF_TOOL_COPY_LEFT"));
    Bind(wxEVT_UPDATE_UI, &DiffSideBySidePanel::OnCopyRightToLeftUI, m_diffView, XRCID("ID_DIFF_TOOL_COPY_LEFT"));

    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnCopyFileLeftToRight, m_diffView,
         XRCID("ID_DIFF_TOOL_COPY_ALL_LEFT_TO_RIGHT"));
    Bind(wxEVT_MENU, &DiffSideBySidePanel::OnCopyFileFromRight, m_diffView,
         XRCID("ID_DIFF_TOOL_COPY_ALL_RIGHT_TO_LEFT"));

    Bind(wxEVT_MENU, &clDiffFrame::OnClose, this, wxID_CLOSE);

    // Not a menu item, but this is a convenient place to bind ESC to Close()
    Bind(wxEVT_CHAR_HOOK, &clDiffFrame::OnCharHook, this);
}

void clDiffFrame::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void clDiffFrame::OnCharHook(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        Close();
    }
}
