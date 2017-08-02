#ifndef SVNSHOWFILECHANGESHANDLER_H
#define SVNSHOWFILECHANGESHANDLER_H

#include "svncommandhandler.h" // Base class: SvnCommandHandler
#include <algorithm>
#include <list>

class Subversion2;

struct SvnShowDiffChunk {
    wxString revision;
    wxString description;
    wxString comment;
    wxArrayString commentArr;
    wxString diff;

    void Finalize()
    {
        // Build the comment from the array of strings
        std::for_each(commentArr.begin(), commentArr.end(), [&](wxString& line) { line.Trim(); });
        comment = ::wxJoin(commentArr, '\n');
        commentArr.Clear();
        // Extract the revision number
        revision = description.BeforeFirst('|').Trim().Trim(false);
        
        description.Trim().Trim(false);
    }

    typedef std::list<SvnShowDiffChunk> List_t;
};

class SvnShowFileChangesHandler : public SvnCommandHandler
{
    // Parsing state
    enum eState { kWaitingSeparator, kWaitingHeader, kWaitingDiffBlock };

public:
    SvnShowFileChangesHandler(Subversion2* plugin, int cmdid, wxEvtHandler* owner);
    virtual ~SvnShowFileChangesHandler();

public:
    virtual void Process(const wxString& output);
};

#endif // SVNSHOWFILECHANGESHANDLER_H
