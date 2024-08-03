#ifndef STDIOTRANSPORT_HPP
#define STDIOTRANSPORT_HPP

#include "dap/Client.hpp"

namespace dap
{

class StdioTransport : public dap::Transport
{
private:
    StdioTransport(const StdioTransport&) = delete;
    StdioTransport& operator=(const StdioTransport&) = delete;
    StdioTransport(StdioTransport&&) = delete;
    StdioTransport& operator=(StdioTransport&&) = delete;

public:
    StdioTransport();
    virtual ~StdioTransport();

    /**
     * @brief return a buffer from the network with a given timeout
     * @returns true on success, false in case of an error. True is also returned when timeout occurs, The caller should
     * check the buffer length: if it is 0, than timeout occurred.
     */
    bool Read(wxString& buffer, int msTimeout) override;

    /**
     * @brief send data over the network
     * @return number of bytes written
     */
    size_t Send(const wxString& buffer) override;
};

} // namespace dap

#endif // STDIOTRANSPORT_HPP
