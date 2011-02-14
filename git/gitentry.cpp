
//////////////////////////////////////////////////////////////////////////////

#include "gitentry.h"
GitEntry::GitEntry()
{
}

GitEntry::~GitEntry()
{
}

void GitEntry::DeSerialize(Archive& arch)
{
  arch.Read(wxT("m_gitrepopath"), m_entries);
  arch.Read(wxT("m_gitColourTrackedFile"), m_colourTrackedFile);
  arch.Read(wxT("m_gitColourDiffFile"), m_colourDiffFile);
  arch.Read(wxT("m_gitExecutable"), m_pathGIT);
  arch.Read(wxT("m_gitkExecutable"), m_pathGITK);
}

void GitEntry::Serialize(Archive& arch)
{
  arch.Write(wxT("m_gitrepopath"), m_entries);
  arch.Write(wxT("m_gitColourTrackedFile"), m_colourTrackedFile);
  arch.Write(wxT("m_gitColourDiffFile"), m_colourDiffFile);
  arch.Write(wxT("m_gitExecutable"), m_pathGIT);
  arch.Write(wxT("m_gitkExecutable"), m_pathGITK);
}
