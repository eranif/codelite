#include "LLDBBacktrace.h"
#include <wx/filename.h>

LLDBBacktrace::LLDBBacktrace(lldb::SBThread &thread)
{
    m_callstack.clear();
    if ( thread.IsValid() ) {
        m_threadId = thread.GetIndexID();
        int nFrames = thread.GetNumFrames();
        for(int j=0; j<nFrames; ++j) {
            
            lldb::SBFrame frame = thread.GetFrameAtIndex(j);
            LLDBBacktrace::Entry entry;
            
            if ( frame.IsValid() ) {
                // do we have a file:line?
                if ( frame.GetLineEntry().IsValid() ) {

                    lldb::SBFileSpec fileSepc = frame.GetLineEntry().GetFileSpec();
                    entry.filename      = wxFileName(fileSepc.GetDirectory(), fileSepc.GetFilename()).GetFullPath();
                    entry.functionName  = frame.GetFunctionName();
                    entry.line          = frame.GetLineEntry().GetLine()-1;
                    entry.id            = frame.GetFrameID();
                    entry.address << wxString::Format("%p", (void*)frame.GetFP());
                    m_callstack.push_back( entry );

                } else {
                    // FIXME: if we dont have a debug symbol, we should learn how to construct a proper entry
                    // for now, we add an empty entry
                    m_callstack.push_back( entry );
                }

            }
        }
    }
}

LLDBBacktrace::~LLDBBacktrace()
{
}

wxString LLDBBacktrace::ToString() const
{
    wxString str;
    str << "Thread ID: " << m_threadId << "\n";
    for(size_t i=0; i<m_callstack.size(); ++i) {
        const LLDBBacktrace::Entry& entry = m_callstack.at(i);
        str << entry.address << ", " 
            << entry.functionName << ", " 
            << entry.filename << ", " 
            << entry.line << "\n";
    }
    return str;
}
