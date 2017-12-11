#include "DiffSideBySidePanel.h"
#include "clDiffFrame.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include <wx/icon.h>

clDiffFrame::clDiffFrame(wxWindow* parent, const DiffSideBySidePanel::FileInfo& left,
                         const DiffSideBySidePanel::FileInfo& right, bool originSourceControl)
    : wxFrame(parent, wxID_ANY, _("Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    DiffSideBySidePanel* p = new DiffSideBySidePanel(this);
    p->SetFilesDetails(left, right);
    p->Diff();
    if(originSourceControl) {
        p->SetOriginSourceControl();
    }
    sz->Add(p, 1, wxEXPAND, 0);
    WindowAttrManager::Load(this);

    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    SetIcons(b);
}

clDiffFrame::clDiffFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, _("Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    DiffSideBySidePanel* p = new DiffSideBySidePanel(this);
    sz->Add(p, 1, wxEXPAND, 0);
    p->DiffNew();
    WindowAttrManager::Load(this);
    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    SetIcons(b);
}

clDiffFrame::clDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile)
    : wxFrame(parent, wxID_ANY, _("Diff View"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    DiffSideBySidePanel* p = new DiffSideBySidePanel(this);
    sz->Add(p, 1, wxEXPAND, 0);
    if(isTempFile) {
        p->SetSaveFilepaths(false);
    }
    
    p->DiffNew(left, right);
    WindowAttrManager::Load(this);
    wxIconBundle b;
    {
        wxIcon icn;
        icn.CopyFromBitmap(clGetManager()->GetStdIcons()->LoadBitmap("diff"));
        b.AddIcon(icn);
    }
    SetIcons(b);
}

clDiffFrame::~clDiffFrame() {}
