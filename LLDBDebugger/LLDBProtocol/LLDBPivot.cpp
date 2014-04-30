#include "LLDBPivot.h"
#include <wx/filename.h>

LLDBPivot::LLDBPivot()
{
}

LLDBPivot::~LLDBPivot()
{
}

wxString LLDBPivot::ToLocal(const wxString& remotePath) const
{
    wxFileName fn(remotePath);
    wxString pathInNativeSeparators = fn.GetFullPath();
    if ( pathInNativeSeparators.StartsWith(m_remoteFolder) ) {
        pathInNativeSeparators.Replace(m_remoteFolder, m_localFolder, false);
    }
    fn = wxFileName(pathInNativeSeparators);
    return fn.GetFullPath();
}

wxString LLDBPivot::ToRemote(const wxString& localPath) const
{
    wxFileName fn(localPath);
    wxString pathInNativeSeparators = fn.GetFullPath();
    if ( pathInNativeSeparators.StartsWith(m_localFolder) ) {
        pathInNativeSeparators.Replace(m_localFolder, m_remoteFolder, false);
    }
    pathInNativeSeparators.Replace("\\", "/");
    return pathInNativeSeparators;
}

bool LLDBPivot::IsValid() const
{
    return !m_localFolder.IsEmpty() && !m_remoteFolder.IsEmpty();
}
