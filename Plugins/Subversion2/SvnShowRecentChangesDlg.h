#ifndef SVNSHOWRECENTCHANGESDLG_H
#define SVNSHOWRECENTCHANGESDLG_H
#include "SvnShowFileChangesHandler.h"
#include "subversion2_ui.hpp"

class SvnShowRecentChangesDlg : public SvnShowRecentChangesBaseDlg
{
    SvnShowDiffChunk::List_t m_changes;

protected:
    void DoSelectRevision(int index);

public:
    SvnShowRecentChangesDlg(wxWindow* parent, const SvnShowDiffChunk::List_t& changes);
    ~SvnShowRecentChangesDlg() override = default;

protected:
    void OnRevisionSelected(wxCommandEvent& event) override;
};
#endif // SVNSHOWRECENTCHANGESDLG_H
