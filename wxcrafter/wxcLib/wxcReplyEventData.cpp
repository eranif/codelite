#include "wxcReplyEventData.h"

wxcReplyEventData::wxcReplyEventData()
{
}

wxcReplyEventData::~wxcReplyEventData()
{
}

void wxcReplyEventData::SetFiles(const std::vector<wxFileName>& files)
{
    m_files.clear();
    for(size_t i=0; i<files.size(); ++i) {
        // In wx2.8 wxString was not thread safe, so we must use .c_str() to force a new copy
        m_files.push_back( files.at(i).GetFullPath().c_str() );
    }
}

void wxcReplyEventData::SetFiles(const std::vector<wxString>& files)
{
    m_files.clear();
    for(size_t i=0; i<files.size(); ++i) {
        // In wx2.8 wxString was not thread safe, so we must use .c_str() to force a new copy
        m_files.push_back( files.at(i).c_str() );
    }
}
