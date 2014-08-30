#include "php_parser_thread.h"
#include "php_storage.h"

PHPParserThread* PHPParserThread::ms_instance = 0;

PHPParserThread::PHPParserThread()
    : m_progress(NULL)
{
}

PHPParserThread::~PHPParserThread()
{
    wxDELETE(m_progress);
}

PHPParserThread* PHPParserThread::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new PHPParserThread();
    }
    return ms_instance;
}

void PHPParserThread::Release()
{
    ms_instance->Stop();
    if (ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void PHPParserThread::ProcessRequest(ThreadRequest* request)
{
    // Lock the parser
    wxCriticalSectionLocker locker( PHPParser.GetLocker() );
    PHPParserThreadRequest* r = dynamic_cast<PHPParserThreadRequest*>( request );
    if ( r ) {
        
        PHPStorage myStorage;
        myStorage.OpenWorksace( wxFileName(r->workspaceFile), false, false );
        if ( r->bIsFullRetag ) {
            // Delete the entire database content
            PHPStorage::AutoCommitter ac(&myStorage, true);
            myStorage.DeleteAllContent();
        }
        
        // Now parse the files
        ParseFiles( r, myStorage);
        myStorage.Close();
    }
}

void PHPParserThread::ParseFiles(PHPParserThreadRequest* request, PHPStorage& myStorage)
{
    PHPStorage::AutoCommitter ac(&myStorage, true);
    const wxArrayString& files = request->files;
    for(size_t i=0; i<files.GetCount(); ++i) {
        // Parse the current file content, delete the file content only when not in full-retag (otherwise we have deleted the entire db earlier)
        myStorage.ParseAndStoreFileNoLock(files.Item(i), false, !request->bIsFullRetag);
        if ( m_progress ) {
            m_progress->CallAfter( &PHPParserThreadProgressCB::OnProgress, i, files.GetCount() );
        }
    }
    if ( m_progress ) {
        m_progress->CallAfter( &PHPParserThreadProgressCB::OnCompleted );
    }
}
