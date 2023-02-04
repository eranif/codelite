#ifndef LSPNETWORKREMOTESTDIO_HPP
#define LSPNETWORKREMOTESTDIO_HPP

#include "LSP/LSPNetwork.h"
#include "ssh/clSSHInteractiveChannel.hpp"

class LSPNetworkRemoteSTDIO : public LSPNetwork
{
    clSSHInteractiveChannel::Ptr_t m_process;
    bool m_eventsBound = false;

private:
    void DoClose();
    void DoStartRemoteProcess();
    void BindEvents();
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

public:
    LSPNetworkRemoteSTDIO();
    ~LSPNetworkRemoteSTDIO() override;

    //===-----------------------------------
    // Interface
    //===-----------------------------------

    /**
     * @brief send data to the LSP server
     */
    void Send(const std::string& data) override;

    /**
     * @brief close the connection to the LSP server
     */
    void Close() override;

    /**
     * @brief open connection to the LSP server
     * @throws this method throws a clException incase an error occured
     */
    void Open(const LSPStartupInfo& info) override;

    /**
     * @brief are we connected to the LSP server?
     */
    bool IsConnected() const override;
};

#endif // LSPNETWORKREMOTESTDIO_HPP
