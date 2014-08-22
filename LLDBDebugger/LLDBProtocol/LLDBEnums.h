//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBEnums.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
    kReplyTypeWatchEvaluated,
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
    kCommandAddWatch,
    kCommandDeleteWatch,
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
