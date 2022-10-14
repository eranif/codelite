#ifndef CLCONCURRENT_HPP
#define CLCONCURRENT_HPP

#include "codelite_exports.h"

#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include <wx/msgqueue.h>

class WXDLLIMPEXP_CL clConcurrent
{
    typedef std::function<void()> Callback;
    std::vector<std::thread*> m_thread_pool;
    size_t m_pool_size = 1;
    wxMessageQueue<Callback> m_queue;
    std::atomic_bool m_shutdown;

public:
    clConcurrent(size_t pool_size = 1);
    virtual ~clConcurrent();

    void run();
    void shutdown();
    void set_pool_size(size_t size) { m_pool_size = size; }
    bool is_running() const { return !m_thread_pool.empty(); }
    void queue(Callback callback) { m_queue.Post(callback); }
};

#endif // CLCONCURRENT_HPP
