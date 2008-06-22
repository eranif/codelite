#include "detachedpanesinfo.h"
DetachedPanesInfo::DetachedPanesInfo(wxArrayString arr)
: m_panes(arr)
{
}

DetachedPanesInfo::~DetachedPanesInfo()
{
}

void DetachedPanesInfo::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_panes"), m_panes);
}

void DetachedPanesInfo::Serialize(Archive& arch)
{
	arch.Write(wxT("m_panes"), m_panes);
}

