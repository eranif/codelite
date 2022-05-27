#include "OutputDebugStringThread.h"

#include "event_notifier.h"
#include "file_logger.h"

wxDEFINE_EVENT(wxEVT_OUTPUT_DEBUG_STRING, clCommandEvent);

OutputDebugStringThread::OutputDebugStringThread()
    : wxThread(wxTHREAD_JOINABLE)
#ifdef __WXMSW__
    , m_hDBWinMutex(INVALID_HANDLE_VALUE)
    , m_hDBMonBuffer(INVALID_HANDLE_VALUE)
    , m_hEventBufferReady(INVALID_HANDLE_VALUE)
    , m_hEventDataReady(INVALID_HANDLE_VALUE)
    , m_pDBBuffer(NULL)
#endif
    , m_collecting(false)
{
#ifdef __WXMSW__
    SetLastError(0);

    // Mutex: DBWin
    // ---------------------------------------------------------
    m_hDBWinMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"DBWinMutex");

    if(m_hDBWinMutex == NULL) {
        clWARNING() << "Failed to open mutex: 'DBWinMutex'." << GetLastError() << clEndl;
        return;
    }

    m_hEventBufferReady = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"DBWIN_BUFFER_READY");

    if(m_hEventBufferReady == NULL) {
        m_hEventBufferReady = ::CreateEvent(NULL,
                                            FALSE, // auto-reset
                                            TRUE,  // initial state: signaled
                                            L"DBWIN_BUFFER_READY");

        if(m_hEventBufferReady == NULL) {
            clWARNING() << "Failed to create event: 'DBWIN_BUFFER_READY'." << GetLastError() << clEndl;
            return;
        }
    }
    // Event: data ready
    // ---------------------------------------------------------
    m_hEventDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, L"DBWIN_DATA_READY");

    if(m_hEventDataReady == NULL) {
        m_hEventDataReady = ::CreateEvent(NULL,
                                          FALSE, // auto-reset
                                          FALSE, // initial state: nonsignaled
                                          L"DBWIN_DATA_READY");

        if(m_hEventDataReady == NULL) {
            clWARNING() << "Failed to create event: 'DBWIN_DATA_READY'." << GetLastError() << clEndl;
            return;
        }
    }

    // Shared memory
    // ---------------------------------------------------------
    wxString DBWIN_BUFFER = L"DBWIN_BUFFER";
    m_hDBMonBuffer = ::OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER.c_str());

    if(m_hDBMonBuffer == NULL) {
        m_hDBMonBuffer = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct dbwin_buffer),
                                             DBWIN_BUFFER.c_str());

        if(m_hDBMonBuffer == NULL) {
            clWARNING() << "Failed to CreateFileMapping:" << DBWIN_BUFFER << "." << GetLastError() << clEndl;
            return;
        }
    }

    m_pDBBuffer = (struct dbwin_buffer*)::MapViewOfFile(m_hDBMonBuffer, SECTION_MAP_READ, 0, 0, 0);

    if(m_pDBBuffer == NULL) {
        clWARNING() << "Failed to MapViewOfFile:" << DBWIN_BUFFER << "." << GetLastError() << clEndl;
        return;
    }
#endif
}

OutputDebugStringThread::~OutputDebugStringThread()
{
#ifdef __WXMSW__
    if(m_hDBWinMutex != NULL) {
        CloseHandle(m_hDBWinMutex);
        m_hDBWinMutex = NULL;
    }

    if(m_hDBMonBuffer != NULL) {
        ::UnmapViewOfFile(m_pDBBuffer);
        CloseHandle(m_hDBMonBuffer);
        m_hDBMonBuffer = NULL;
    }

    if(m_hEventBufferReady != NULL) {
        CloseHandle(m_hEventBufferReady);
        m_hEventBufferReady = NULL;
    }

    if(m_hEventDataReady != NULL) {
        CloseHandle(m_hEventDataReady);
        m_hEventDataReady = NULL;
    }

    m_pDBBuffer = NULL;
#endif
}

void* OutputDebugStringThread::Entry()
{
#ifdef __WXMSW__
    while(!TestDestroy()) {
        if(m_collecting) {
            DWORD ret = 0;

            // wait for data ready
            ret = ::WaitForSingleObject(m_hEventDataReady, 100);

            if(ret == WAIT_OBJECT_0) {
                // Send the data
                wxString data = m_pDBBuffer->data;
                int processID = m_pDBBuffer->dwProcessId;
                clCommandEvent event(wxEVT_OUTPUT_DEBUG_STRING);
                event.SetInt(processID);
                event.SetString(data);

                EventNotifier::Get()->AddPendingEvent(event);
                // signal buffer ready
                SetEvent(m_hEventBufferReady);
            }

        } else {
            wxMilliSleep(100);
        }
    }
#endif
    return NULL;
}

void OutputDebugStringThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}

void OutputDebugStringThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}
