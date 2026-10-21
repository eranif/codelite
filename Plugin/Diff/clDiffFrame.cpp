#include "clDiffFrame.h"

#include "DiffSideBySidePanel.h"
#include "UnicodeSymbols.hpp"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

namespace
{
/// The file that identifies a diff: prefer the right side (e.g. the working copy)
wxFileName GetDiffFile(const wxFileName& left, const wxFileName& right) { return right.IsOk() ? right : left; }

/// The tab title: "Diff View <symbol> - <file name>"
wxString GetTabTitle(const wxFileName& file)
{
    wxString title = _("Diff View");
    if (file.IsOk()) {
        title << " " << GetUnicodeSymbol(IconType::kCircle) << " " << file.GetFullName();
    }
    return title;
}

/// If a diff tab with this title is open, select it and return it
clDiffFrame* SelectDiffTab(const wxString& title)
{
    auto diff = dynamic_cast<clDiffFrame*>(clGetManager()->FindPage(title));
    if (diff != nullptr) {
        clGetManager()->SelectPage(diff);
    }
    return diff;
}
} // namespace

clDiffFrame::clDiffFrame(wxWindow* parent,
                         const DiffSideBySidePanel::FileInfo& left,
                         const DiffSideBySidePanel::FileInfo& right,
                         bool originSourceControl)
    : wxPanel(parent)
{
    Hide();
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    m_diffView->SetFilesDetails(left, right);
    m_diffView->Diff();
    if (originSourceControl) {
        m_diffView->SetOriginSourceControl();
    }
    sz->Add(m_diffView, 1, wxEXPAND, 0);
}

clDiffFrame::clDiffFrame(wxWindow* parent)
    : wxPanel(parent)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    sz->Add(m_diffView, 1, wxEXPAND, 0);
    m_diffView->DiffNew();
}

clDiffFrame::clDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile)
    : wxPanel(parent)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    m_diffView = new DiffSideBySidePanel(this);
    sz->Add(m_diffView, 1, wxEXPAND, 0);
    if (isTempFile) {
        m_diffView->SetSaveFilepaths(false);
    }

    m_diffView->DiffNew(left, right);
}

void clDiffFrame::ShowInTab(const wxString& title)
{
    clGetManager()->AddPage(this, title, wxEmptyString, wxEmptyString, true);

    // IManager::AddPage() does not set a bitmap
    auto book = clGetManager()->GetMainNotebook();
    int index = book->GetPageIndex(this);
    if (index != wxNOT_FOUND) {
        book->SetPageBitmap(index, clGetManager()->GetStdIcons()->LoadBitmap("diff"));
    }

    // The diff can be requested from another top level window (e.g. the folders diff)
    EventNotifier::Get()->TopFrame()->Raise();
}

clDiffFrame* clDiffFrame::Open(const DiffSideBySidePanel::FileInfo& left,
                               const DiffSideBySidePanel::FileInfo& right,
                               bool originSourceControl)
{
    const wxString title = GetTabTitle(GetDiffFile(left.filename, right.filename));
    if (auto existing = SelectDiffTab(title)) {
        return existing;
    }
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook(), left, right, originSourceControl);
    panel->ShowInTab(title);
    return panel;
}

clDiffFrame* clDiffFrame::OpenNew()
{
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook());
    panel->ShowInTab(GetTabTitle(wxFileName()));
    return panel;
}

clDiffFrame* clDiffFrame::Open(const wxFileName& left, const wxFileName& right, bool isTempFile)
{
    const wxString title = GetTabTitle(GetDiffFile(left, right));
    if (auto existing = SelectDiffTab(title)) {
        return existing;
    }
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook(), left, right, isTempFile);
    panel->ShowInTab(title);
    return panel;
}
