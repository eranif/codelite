#include "LLDBReply.h"
#include "LLDBEnums.h"

LLDBReply::~LLDBReply()
{
}

LLDBReply::LLDBReply(const wxString& str)
{
    JSONRoot root(str);
    FromJSON( root.toElement() );
}

void LLDBReply::FromJSON(const JSONElement& json)
{
    m_replyType         = json.namedObject("m_replyType").toInt(kReplyTypeInvalid);
    m_interruptResaon   = json.namedObject("m_stopResaon").toInt(kInterruptReasonNone);
    m_line              = json.namedObject("m_line").toInt(wxNOT_FOUND);
    m_filename          = json.namedObject("m_filename").toString();
    m_lldbId            = json.namedObject("m_lldbId").toInt();
    m_expression        = json.namedObject("m_expression").toString();
    m_debugSessionType  = json.namedObject("m_debugSessionType").toInt(kDebugSessionTypeNormal);

    m_breakpoints.clear();
    JSONElement arr = json.namedObject("m_breakpoints");
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint() );
        bp->FromJSON( arr.arrayItem(i) );
        m_breakpoints.push_back( bp );
    }
    
    m_variables.clear();
    JSONElement localsArr = json.namedObject("m_locals");
    m_variables.reserve( localsArr.arraySize() );
    for(int i=0; i<localsArr.arraySize(); ++i) {
        LLDBVariable::Ptr_t variable(new LLDBVariable() );
        variable->FromJSON( localsArr.arrayItem(i) );
        m_variables.push_back( variable );
    }
    
    m_backtrace.Clear();
    JSONElement backtrace = json.namedObject("m_backtrace");
    m_backtrace.FromJSON( backtrace );
    
    m_threads = LLDBThread::FromJSON( json, "m_threads" );
}

JSONElement LLDBReply::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_replyType",         m_replyType);
    json.addProperty("m_stopResaon",        m_interruptResaon);
    json.addProperty("m_line",              m_line);
    json.addProperty("m_filename",          m_filename);
    json.addProperty("m_lldbId",            m_lldbId);
    json.addProperty("m_expression",        m_expression);
    json.addProperty("m_debugSessionType",  m_debugSessionType);
    
    JSONElement bparr = JSONElement::createArray("m_breakpoints");
    json.append( bparr );
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        bparr.arrayAppend( m_breakpoints.at(i)->ToJSON() );
    }

    JSONElement localsArr = JSONElement::createArray("m_locals");
    json.append( localsArr );
    for(size_t i=0; i<m_variables.size(); ++i) {
        localsArr.arrayAppend( m_variables.at(i)->ToJSON() );
    }

    json.addProperty("m_backtrace", m_backtrace.ToJSON());
    json.append( LLDBThread::ToJSON(m_threads, "m_threads") );
    return json;
}

void LLDBReply::UpdatePaths(const LLDBPivot& pivot)
{
    if ( pivot.IsValid() ) {
        
        // Update the file name
        m_filename = pivot.ToLocal( m_filename );
        
        // Update the breakpoint list
        for(size_t i=0; i<m_breakpoints.size(); ++i) {
            m_breakpoints.at(i)->SetFilename( pivot.ToLocal( m_breakpoints.at(i)->GetFilename() ) );
        }
        
        // Update the call stack entries
        LLDBBacktrace::EntryVec_t callstack = m_backtrace.GetCallstack();
        for(size_t i=0; i<callstack.size(); ++i) {
            callstack.at(i).filename = pivot.ToLocal( callstack.at(i).filename );
        }
        m_backtrace.SetCallstack( callstack );
        
        // Update the thread stack
        for(size_t i=0; i<m_threads.size(); ++i) {
            m_threads.at(i).SetFile( pivot.ToLocal(m_threads.at(i).GetFile()) );
        }
    }
}

