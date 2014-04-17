#include "LLDBThread.h"
#include "LLDBEnums.h"

LLDBThread::LLDBThread()
    : m_id(wxNOT_FOUND)
    , m_line(wxNOT_FOUND)
    , m_active(false)
    , m_stopReason( kStopReasonInvalid )
{
}

LLDBThread::~LLDBThread()
{
}

void LLDBThread::FromJSON(const JSONElement& json)
{
    m_id   = json.namedObject("m_id").toInt();
    m_func = json.namedObject("m_func").toString();
    m_file = json.namedObject("m_file").toString();
    m_line = json.namedObject("m_line").toInt();
    m_active = json.namedObject("m_active").toBool();
    m_stopReason = json.namedObject("m_stopReason").toInt();
    m_stopReasonString = json.namedObject("m_stopReasonString").toString();
}

JSONElement LLDBThread::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_id", m_id);
    json.addProperty("m_func", m_func);
    json.addProperty("m_file", m_file);
    json.addProperty("m_line", m_line);
    json.addProperty("m_active", m_active);
    json.addProperty("m_stopReason", m_stopReason);
    json.addProperty("m_stopReasonString", m_stopReasonString);
    return json;
}

JSONElement LLDBThread::ToJSON(const LLDBThread::Vect_t& threads, const wxString &name)
{
    JSONElement arr = JSONElement::createArray(name);
    for(size_t i=0; i<threads.size(); ++i) {
        arr.arrayAppend( threads.at(i).ToJSON() );
    }
    return arr;
}

LLDBThread::Vect_t LLDBThread::FromJSON(const JSONElement& json, const wxString& name)
{
    LLDBThread::Vect_t v;
    JSONElement arr = json.namedObject(name);
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBThread thr;
        thr.FromJSON( arr.arrayItem(i) );
        v.push_back( thr );
    }
    return v;
}
