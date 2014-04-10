#ifndef LLDB_ENUMS_H
#define LLDB_ENUMS_H

// defines the various reasons why the debugger
// was inerrupted / stopped
enum eInterruptReason {
    kInterruptReasonNone,
    kInterruptReasonApplyBreakpoints,
    kInterruptReasonDeleteAllBreakpoints,
    kInterruptReasonDeleteBreakpoints,
};

// the various network LLDBReply type
enum eReplyType {
    kTypeInvalid,
    kTypeDebuggerStartedSuccessfully,
    kTypeDebuggerStopped,
    kTypeDebuggerStoppedOnFirstEntry,
    kTypeDebuggerExited,
    kTypeDebuggerRunning,
    kTypeBacktraceReceived,
};

enum eCommandType {
    kCommandInvalid = -1,
    kCommandStart,
    kCommandRun,
    kCommandStop,
};
    
#endif // 
