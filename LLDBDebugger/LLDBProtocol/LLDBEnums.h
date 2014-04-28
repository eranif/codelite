#ifndef LLDB_ENUMS_H
#define LLDB_ENUMS_H

// defines the various reasons why the debugger
// was inerrupted / stopped
enum eInterruptReason {
    kInterruptReasonNone,
    kInterruptReasonApplyBreakpoints,
    kInterruptReasonDeleteAllBreakpoints,
    kInterruptReasonDeleteBreakpoint,
    kInterruptReasonDetaching,
};

// the various network LLDBReply type
enum eReplyType {
    kReplyTypeInvalid,
    kReplyTypeDebuggerStartedSuccessfully,
    kReplyTypeDebuggerStopped,
    kReplyTypeDebuggerStoppedOnFirstEntry,
    kReplyTypeDebuggerExited,
    kReplyTypeDebuggerRunning,
    kReplyTypeBacktraceReceived,
    kReplyTypeAllBreakpointsDeleted,
    kReplyTypeBreakpointsUpdated,
    kReplyTypeLocalsUpdated,
    kReplyTypeVariableExpanded,
    kReplyTypeExprEvaluated,
};

// LLDBCommand types
enum eCommandType {
    kCommandInvalid = -1,
    kCommandStart,
    kCommandRun,
    kCommandStop,
    kCommandDetach,
    kCommandApplyBreakpoints,
    kCommandDeleteBreakpoint,
    kCommandDeleteAllBreakpoints,
    kCommandContinue,
    kCommandNext,
    kCommandStepIn,
    kCommandStepOut,
    kCommandInterrupt,
    kCommandGetLocals,
    kCommandExpandVariable,
    kCommandSelectFrame,
    kCommandSelectThread,
    kCommandEvalExpression,
    kCommandDebugCoreFile,
    kCommandAttachProcess,
    kCommandNextInstruction,
    kCommandCurrentFileLine,
};

enum eLLDBOptions {
    kLLDBOptionRaiseCodeLite    = 0x00000001,
    kLLDBOptionUseRemoteProxy   = 0x00000002,
};

enum eLLDBDebugSessionType {
    kDebugSessionTypeNormal,
    kDebugSessionTypeCore,
    kDebugSessionTypeAttach,
};

enum eLLDBStopReason {
    kStopReasonInvalid = 0,
    kStopReasonNone,
    kStopReasonTrace,
    kStopReasonBreakpoint,
    kStopReasonWatchpoint,
    kStopReasonSignal,
    kStopReasonException,
    kStopReasonExec,        // Program was re-exec'ed
    kStopReasonPlanComplete,
    kStopReasonThreadExiting,
};
#endif
