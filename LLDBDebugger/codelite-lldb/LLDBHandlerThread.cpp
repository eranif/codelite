#include "LLDBHandlerThread.h"
#include "file_logger.h"
#include "codelite-lldb-app.h"
#include <lldb/API/SBEvent.h>

LLDBHandlerThread::LLDBHandlerThread(CodeLiteLLDBApp* app, lldb::SBListener listener, lldb::SBProcess process)
    : wxThread(wxTHREAD_JOINABLE)
    , m_app(app)
    , m_process(process)
    , m_listener(listener)
{
}

LLDBHandlerThread::~LLDBHandlerThread()
{
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void* LLDBHandlerThread::Entry()
{
    bool first_time_stopped = true;
    CL_DEBUG("LLDB Thread: started");
    while ( !TestDestroy() ) {
        lldb::SBEvent event;
        if ( m_listener.WaitForEvent(1, event) && event.IsValid() ) {
            lldb::StateType state = m_process.GetStateFromEvent( event );
            switch ( state ) {
            case lldb::eStateInvalid:
                CL_DEBUG("LLDBHandlerThread: eStateInvalid");
                break;
            
            case lldb::eStateUnloaded:
                CL_DEBUG("LLDBHandlerThread: eStateUnloaded");
                break;

            case lldb::eStateStopped: 
                if ( first_time_stopped ) {
                    first_time_stopped = false;
                    CL_DEBUG("LLDBHandlerThread: eStateStopped - first time");
                    m_app->CallAfter( &CodeLiteLLDBApp::NotifyStoppedOnFirstEntry );
                    
                } else {
                    CL_DEBUG("LLDBHandlerThread: eStateStopped");
                    m_app->CallAfter( &CodeLiteLLDBApp::NotifyBacktrace );
                    m_app->CallAfter( &CodeLiteLLDBApp::NotifyStopped );
                    
                }
                break;

            case lldb::eStateConnected:
                CL_DEBUG("LLDBHandlerThread: eStateConnected");
                break;

            case lldb::eStateLaunching:
                CL_DEBUG("LLDBHandlerThread: eStateLaunching");
                break;

            case lldb::eStateRunning:
                m_app->CallAfter( &CodeLiteLLDBApp::NotifyRunning );
                CL_DEBUG("LLDBHandlerThread: eStateRunning");
                break;

            case lldb::eStateExited:
                CL_DEBUG("LLDBHandlerThread: eStateExited");
                m_app->CallAfter( &CodeLiteLLDBApp::NotifyExited );
                break;
                
            case lldb::eStateAttaching:
                CL_DEBUG("LLDBHandlerThread: eStateExited");
                break;
                
            case lldb::eStateStepping:
                CL_DEBUG("LLDBHandlerThread: eStateStepping");
                break;
                
            case lldb::eStateCrashed:
                CL_DEBUG("LLDBHandlerThread: eStateCrashed");
                break;
                
            case lldb::eStateDetached:
                CL_DEBUG("LLDBHandlerThread: eStateDetached");
                break;
                
            case lldb::eStateSuspended:
                CL_DEBUG("LLDBHandlerThread: eStateSuspended");
                break;
            }
        }
    }
    return NULL;
}
