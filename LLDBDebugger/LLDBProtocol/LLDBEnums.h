#ifndef LLDB_ENUMS_H
#define LLDB_ENUMS_H

// defines the various reasons why the debugger
// was inerrupted / stopped
enum eInterruptReason {
    kInterruptReasonNone,
    kInterruptReasonApplyBreakpoints,
    kInterruptReasonDeleteAllBreakpoints,
    kInterruptReasonDeleteBreakpoint,
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
    kTypeAllBreakpointsDeleted,
    kTypeBreakpointsUpdated,
};

// LLDBCommand types
enum eCommandType {
    kCommandInvalid = -1,
    kCommandStart,
    kCommandRun,
    kCommandStop,
    kCommandApplyBreakpoints,
    kCommandDeleteBreakpoint,
    kCommandDeleteAllBreakpoints,
    kCommandContinue,
    kCommandNext,
    kCommandStepIn,
    kCommandStepOut,
    kCommandInterrupt,
};

#endif

