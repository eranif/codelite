#include "clConcurrent.hpp"

#include "file_logger.h"

#include <sstream>

clConcurrent::clConcurrent(size_t pool_size)
    : m_pool_size(pool_size)
{
    m_shutdown.store(false);
}

clConcurrent::~clConcurrent() { shutdown(); }

void clConcurrent::run()
{
    shutdown();
    m_thread_pool.reserve(m_pool_size);

    for(size_t i = 0; i < m_pool_size; ++i) {
        std::thread* thr = new std::thread([&]() {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            clDEBUG() << "worker thread #" << ss.str() << endl;
            Callback command;
            while(!m_shutdown.load()) {
                Callback cb;
                if(m_queue.ReceiveTimeout(100, cb) == wxMSGQUEUE_NO_ERROR) {
                    cb();
                }
            }
            clDEBUG() << "worker thread #" << ss.str() << "is going down" << endl;
        });
        m_thread_pool.push_back(thr);
    }
}

void clConcurrent::shutdown()
{
    m_shutdown.store(true);
    for(auto thr : m_thread_pool) {
        thr->join();
        wxDELETE(thr);
    }
    m_thread_pool.clear();
    m_shutdown.store(false);
}
