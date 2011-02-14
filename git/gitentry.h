//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitentry__
#define __gitentry__

#include "serialized_object.h"

#include <wx/colour.h>

class GitEntry : public SerializedObject
{
    wxColour m_colourTrackedFile;
    wxColour m_colourDiffFile;
    wxString m_pathGIT;
    wxString m_pathGITK;
    std::map<wxString, wxString> m_entries;

  public:
    GitEntry();
    virtual ~GitEntry();
//
  public:
    virtual void DeSerialize(Archive &arch);
    virtual void Serialize(Archive &arch);

    void SetEntry(const wxString& workspace, const wxString& repo)
    {
      this->m_entries[workspace] = repo;
    }
    void SetTrackedFileColour(const wxColour& colour)
    {
      this->m_colourTrackedFile = colour;
    }
    void SetDiffFileColour(const wxColour& colour)
    {
      this->m_colourDiffFile = colour;
    }
    void SetGITExecutablePath(const wxString& exe)
    {
      this->m_pathGIT = exe;
    }
    void SetGITKExecutablePath(const wxString& exe)
    {
      this->m_pathGITK = exe;
    }
    const wxString& GetPath(const wxString& workspace)
    {
      return m_entries[workspace];
    }
    const wxColour& GetTrackedFileColour()
    {
      return this->m_colourTrackedFile;
    }
    const wxColour& GetDiffFileColour()
    {
      return this->m_colourDiffFile;
    }
    const wxString& GetGITExecutablePath()
    {
      return this->m_pathGIT;
    }
    const wxString& GetGITKExecutablePath()
    {
      return this->m_pathGITK;
    }

};
#endif // __abbreviationentry__
