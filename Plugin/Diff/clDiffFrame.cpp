#include "clDiffFrame.h"

#include "DiffSideBySidePanel.h"
#include "UnicodeSymbols.hpp"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

namespace
{
/// The tab shows the name of the file being compared, prefer the right side (e.g. the working copy)
wxString GetTabTitle(const wxFileName& left, const wxFileName& right)
{
    const wxFileName& fn = right.IsOk() ? right : left;
    wxString title;
    title << GetUnicodeSymbol(IconType::kDiff) << " " << (fn.IsOk() ? fn.GetFullName() : _("Diff"));
    return title;
}
} // namespace

clDiffFrame::clDiffFrame(wxWindow* parent,
                         const DiffSideBySidePanel::FileInfo& left,
                         const DiffSideBySidePanel::FileInfo& right,
                         bool originSourceControl)
    : wxPanel(parent)
{
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
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook(), left, right, originSourceControl);
    panel->ShowInTab(GetTabTitle(left.filename, right.filename));
    return panel;
}

clDiffFrame* clDiffFrame::OpenNew()
{
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook());
    panel->ShowInTab(GetTabTitle(wxFileName(), wxFileName()));
    return panel;
}

clDiffFrame* clDiffFrame::Open(const wxFileName& left, const wxFileName& right, bool isTempFile)
{
    auto panel = new clDiffFrame(clGetManager()->GetMainNotebook(), left, right, isTempFile);
    panel->ShowInTab(GetTabTitle(left, right));
    return panel;
}
