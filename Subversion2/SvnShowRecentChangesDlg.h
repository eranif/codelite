#ifndef SVNSHOWRECENTCHANGESDLG_H
#define SVNSHOWRECENTCHANGESDLG_H
#include "subversion2_ui.h"
#include "SvnShowFileChangesHandler.h"

class SvnShowRecentChangesDlg : public SvnShowRecentChangesBaseDlg
{
    SvnShowDiffChunk::List_t m_changes;

protected:
    void DoSelectRevision(int index);

public:
    SvnShowRecentChangesDlg(wxWindow* parent, const SvnShowDiffChunk::List_t& changes);
    virtual ~SvnShowRecentChangesDlg();

protected:
    virtual void OnRevisionSelected(wxCommandEvent& event);
};
#endif // SVNSHOWRECENTCHANGESDLG_H
