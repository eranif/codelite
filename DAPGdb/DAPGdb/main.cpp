#include "Process.hpp"
#include "StringUtils.hpp"
#include "dap/dap.hpp"
#include "dap/json.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    // Test to see how the various objects are printed
    dap::CancelRequest req;
    dap::InitializedEvent initEvent;
    dap::StoppedEvent stoppedEvent;
    dap::ContinuedEvent contEvent;
    stoppedEvent.reason = "Breakpoint Hit";
    stoppedEvent.text = "User hit breakpoint";

    dap::JSONItem j = req.To("");
    cout << j.Format() << endl;
    cout << initEvent.To("").Format() << endl;
    cout << stoppedEvent.To("").Format() << endl;
    cout << contEvent.To("").Format() << endl;
    return 0;
}
