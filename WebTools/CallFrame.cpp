#include "CallFrame.h"
#include "CallFrameScope.h"

CallFrame::CallFrame() {}

CallFrame::~CallFrame() {}

JSONItem CallFrame::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("callFrameId", m_callFrameId);
    json.addProperty("functionName", m_functionName);
    json.append(m_location.ToJSON("location"));
    json.append(m_this.ToJSON("this"));
    JSONItem arr = JSONItem::createArray("scopeChain");
    for(size_t i = 0; i < m_scopeChain.size(); ++i) {
        nSerializableObject::Ptr_t p = m_scopeChain[i];
        arr.arrayAppend(p->To<CallFrameScope>()->ToJSON(""));
    }
    json.append(arr);
    return json;
}

void CallFrame::FromJSON(const JSONItem& json)
{
    m_callFrameId = json.namedObject("callFrameId").toString();
    m_functionName = json.namedObject("functionName").toString();
    m_location.FromJSON(json.namedObject("location"));
    m_this.FromJSON(json.namedObject("this"));
    JSONItem scopChain = json.namedObject("scopeChain");
    int arr_size = scopChain.arraySize();
    for(int i = 0; i < arr_size; ++i) {
        JSONItem scope = scopChain.arrayItem(i);
        CallFrameScope::Ptr_t pScope(new CallFrameScope());
        pScope->FromJSON(scope);
        m_scopeChain.push_back(pScope);
    }
}
