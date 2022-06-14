#include "DAPMainView.h"

DAPMainView::DAPMainView(wxWindow* parent)
    : DAPMainViewBase(parent)
{
    m_treeThreads->SetShowHeader(true);
    m_treeThreads->AddHeader(_("ID"));     // The thread/frame ID
    m_treeThreads->AddHeader(_("Name"));   // The thread/frame name
    m_treeThreads->AddHeader(_("Source")); // The frame source (function or file)
    m_treeThreads->AddHeader(_("Line"));   // The frame line
}

DAPMainView::~DAPMainView() {}
