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
	
    stoppedEvent.reason = "Breakpoint Hit";
    stoppedEvent.text = "User hit breakpoint";

    cout << cancelRequest.To().Format() << endl;
    cout << initEvent.To().Format() << endl;
    cout << stoppedEvent.To().Format() << endl;
    cout << contEvent.To().Format() << endl;
    cout << bpEvent.To().Format() << endl;
    cout << procEvent.To().Format() << endl;
    cout << initReq.To().Format() << endl;
    return 0;
}
