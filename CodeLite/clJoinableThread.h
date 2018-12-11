#ifndef CSJOINABLETHREAD_H
#define CSJOINABLETHREAD_H

#include "clJoinableThread.h"
#include "codelite_exports.h"
#include <wx/thread.h>
class WXDLLIMPEXP_CL clJoinableThread : public wxThread
{
protected:
    void Stop();

public:
    clJoinableThread();
    virtual ~clJoinableThread();

    /**
     * Start the thread as joinable thread.
     * This call must be called from the context of other thread (e.g. main thread)
     */
    void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);
};

#endif // CSJOINABLETHREAD_H
