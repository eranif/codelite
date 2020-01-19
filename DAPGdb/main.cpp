#include "dap/Process.hpp"
#include "dap/StringUtils.hpp"
#include "dap/dap.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    // Test to see how the various objects are printed
    dap::CancelRequest cancelRequest;
    dap::InitializedEvent initEvent;
    dap::StoppedEvent stoppedEvent;
    dap::ContinuedEvent contEvent;
    dap::BreakpointEvent bpEvent;
    dap::ProcessEvent procEvent;
    dap::InitializeRequest initReq;
    dap::InitializeResponse initRespo;
    dap::ConfigurationDoneRequest configDone;
    dap::ConfigurationDoneResponse configDoneRespo;
    dap::LaunchRequest launchReq;
    dap::LaunchResponse launchRespo;
    dap::DisconnectRequest dcReq;
    dap::DisconnectResponse dcRespo;
    dap::BreakpointLocationsRequest bpLocReq;
    dap::BreakpointLocationsResponse bpLocRespo;
    bpLocReq.arguments.source.path = "/home/eran/test2.cpp";
    bpLocReq.arguments.line = 0;
    bpLocReq.arguments.endLine = 20;
    bpLocRespo.breakpoints.push_back(dap::BreakpointLocation());
    bpLocRespo.breakpoints.push_back(dap::BreakpointLocation());

    // Build a setBreakpoint request
    dap::SetBreakpointsRequest setBreakpointReq;
    setBreakpointReq.arguments.source.path = "/home/eran/test.cpp";
    {
        dap::SourceBreakpoint sb;
        sb.condition = "i == 0";
        sb.line = 14;
        setBreakpointReq.arguments.breakpoints.push_back(sb);
    }
    {
        dap::SourceBreakpoint sb;
        sb.line = 35;
        setBreakpointReq.arguments.breakpoints.push_back(sb);
    }

    stoppedEvent.reason = "Breakpoint Hit";
    stoppedEvent.text = "User hit breakpoint";

    cout << cancelRequest.To().Format() << endl;
    cout << initEvent.To().Format() << endl;
    cout << stoppedEvent.To().Format() << endl;
    cout << contEvent.To().Format() << endl;
    cout << bpEvent.To().Format() << endl;
    cout << procEvent.To().Format() << endl;
    cout << initReq.To().Format() << endl;
    cout << initRespo.To().Format() << endl;
    cout << configDone.To().Format() << endl;
    cout << configDoneRespo.To().Format() << endl;
    cout << launchReq.To().Format() << endl;
    cout << launchRespo.To().Format() << endl;
    cout << dcReq.To().Format() << endl;
    cout << dcRespo.To().Format() << endl;
    cout << bpLocReq.To().Format() << endl;
    cout << bpLocRespo.To().Format() << endl;
    cout << setBreakpointReq.To().Format() << endl;
    return 0;
}
