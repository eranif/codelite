#include "wxcNetworkReply.h"

wxcNetworkReply::wxcNetworkReply() 
    : m_replyType(wxCrafter::kReplyTypeInvalid)
{
}

wxcNetworkReply::wxcNetworkReply(const wxString& json)
{
    JSONRoot root( json );
    FromJSON( root.toElement() );
}

wxcNetworkReply::~wxcNetworkReply()
{
}

void wxcNetworkReply::FromJSON(const JSONElement& json)
{
    m_replyType = (wxCrafter::eReplyType) json.namedObject(wxT("m_replyType")).toInt(wxCrafter::kCommandTypeInvalid);
    m_wxcpFile = json.namedObject(wxT("m_wxcpFile")).toString();
    
    m_files.clear();
    JSONElement arr = json.namedObject(wxT("m_files"));
    for(int i=0; i<arr.arraySize(); ++i) {
        m_files.push_back( arr.arrayItem(i).toString() );
    }
}

JSONElement wxcNetworkReply::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("m_replyType"), (int)m_replyType);
    json.addProperty(wxT("m_wxcpFile"), m_wxcpFile);

    JSONElement arr = JSONElement::createArray(wxT("m_files"));
    json.append( arr );
    
    for(size_t i=0; i<m_files.size(); ++i) {
        arr.arrayAppend( m_files.at(i).GetFullPath() );
    }
    return json;
}

