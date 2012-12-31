//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitentry__
#define __gitentry__

#include <wx/colour.h>
#include "cl_config.h"

extern const wxEventType wxEVT_GIT_CONFIG_CHANGED;
class GitEntry : public clConfigItem
{
    wxColour                   m_colourTrackedFile;
    wxColour                   m_colourDiffFile;
    wxString                   m_pathGIT;
    wxString                   m_pathGITK;
    JSONElement::wxStringMap_t m_entries;
    size_t                     m_flags;
    int                        m_gitDiffDlgSashPos;

public:
    enum {
        Git_Verbose_Log  = 0x00000001,
        Git_Show_Terminal= 0x00000002
    };

public:
    GitEntry();
    virtual ~GitEntry();
public:

    void EnableFlag( size_t flag, bool b ) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    void SetGitDiffDlgSashPos(int gitDiffDlgSashPos) {
        this->m_gitDiffDlgSashPos = gitDiffDlgSashPos;
    }
    int GetGitDiffDlgSashPos() const {
        return m_gitDiffDlgSashPos;
    }
    void SetEntries(const JSONElement::wxStringMap_t& entries) {
        this->m_entries = entries;
    }
    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    const JSONElement::wxStringMap_t& GetEntries() const {
        return m_entries;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    void SetEntry(const wxString& workspace, const wxString& repo) {
        this->m_entries[workspace] = repo;
    }
    void SetTrackedFileColour(const wxColour& colour) {
        this->m_colourTrackedFile = colour;
    }
    void SetDiffFileColour(const wxColour& colour) {
        this->m_colourDiffFile = colour;
    }
    void SetGITExecutablePath(const wxString& exe) {
        this->m_pathGIT = exe;
    }
    void SetGITKExecutablePath(const wxString& exe) {
        this->m_pathGITK = exe;
    }
    const wxString& GetPath(const wxString& workspace) {
        return m_entries[workspace];
    }
    const wxColour& GetTrackedFileColour() {
        return this->m_colourTrackedFile;
    }
    const wxColour& GetDiffFileColour() {
        return this->m_colourDiffFile;
    }
    const wxString& GetGITExecutablePath() {
        return this->m_pathGIT;
    }
    const wxString& GetGITKExecutablePath() {
        return this->m_pathGITK;
    }
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};
#endif
