#ifndef GITBLAMEPAGE_H
#define GITBLAMEPAGE_H

#include "clThemedSTC.hpp"
#include "cl_command_event.h"
#include "git.h"

#include <wx/stc/stc.h>

namespace git::blame
{
struct LineInfo {
    long line_number = 0;
    wxString author;           // commit author
    wxString author_email;     // email
    wxString author_time;      // commit time
    wxString content;          // the line content
    wxString summary;          // first line of the commit
    wxString commit_hash;      // commit hash
    wxString prev_commit_hash; // previous commit hash
    wxString display_line;
    typedef std::vector<LineInfo> vec_t;

    /// https://git-scm.com/docs/git-blame#_the_porcelain_format
    /// Parse and consume a complete porcelain format record
    bool FromPorcelainFormat(wxArrayString& lines);
};

} // namespace git::blame

class GitBlamePage : public clThemedSTC
{
    GitPlugin* m_plugin = nullptr;
    std::vector<git::blame::LineInfo::vec_t> m_stack;
    wxString m_filename;
    int m_first_visible_line = wxNOT_FOUND;

protected:
    const git::blame::LineInfo::vec_t& current_info() const;
    void InitialiseView();
    void OnMarginRightClick(wxStyledTextEvent& event);
    void OnIdle(wxIdleEvent& event);

public:
    GitBlamePage(wxWindow* parent, GitPlugin* plugin, const wxString& fullpath);
    virtual ~GitBlamePage();

    void ParseBlameOutput(const wxString& blame);
};
#endif // GITBLAMEPAGE_H
