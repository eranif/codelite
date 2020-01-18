#ifndef PROTOCOLMESSAGE_HPP
#define PROTOCOLMESSAGE_HPP

#include "json.hpp"
#include <memory>
#include <string>

/// Implementation of Debug Adapter Protocol (DAP)
/// The Debug Adapter Protocol defines the protocol used between an editor or IDE and a debugger or runtime
/// The implementation is based on the specifications described here:
/// https://microsoft.github.io/debug-adapter-protocol/specification

using namespace std;
namespace dap
{
// base class representing anything
struct Any {
    Any() {}
    virtual ~Any() {}

    virtual JSONItem To(const string& name = "") const = 0;
    virtual void From(const JSONItem& json) = 0;

    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Any*>(this)); }
    typedef shared_ptr<Any> Ptr_t;
};

/// Base class of requests, responses, and events
struct ProtocolMessage : public Any {
    int seq = -1;
    string type;

    ProtocolMessage() {}
    virtual ~ProtocolMessage() {}

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// A client or debug adapter initiated request
/// ->
struct Request : public ProtocolMessage {
    string command;
    Any::Ptr_t arguments;

    Request() { type = "request"; }
    virtual ~Request() = 0; // force to abstract class

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The 'cancel' request is used by the frontend to indicate that it is no longer interested in the result produced by a
/// specific request issued earlier.
/// <->
struct CancelRequest : public Request {
    string command;
    int requestId = -1;

    CancelRequest() { type = "cancel"; }
    virtual ~CancelRequest() {}

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// A debug adapter initiated event
///
struct Event : public ProtocolMessage {
    string event;
    Any::Ptr_t body;

    Event() { type = "event"; }
    virtual ~Event() = 0; // force to abstract class

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Response for a request
/// <-
struct Response : public ProtocolMessage {
    int request_seq = -1;
    bool success = true;

    /**
     * Contains the raw error in short form if 'success' is false.
     * This raw error might be interpreted by the frontend and is not shown in the UI.
     * Some predefined values exist.
     * Values:
     * 'cancelled': request was cancelled.
     * etc.
     */
    string message;

    Response() { type = "response"; }
    virtual ~Response() = 0; // force to abstract class
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Response to 'cancel' request. This is just an acknowledgement, so no body field is required.
/// <-
struct CancelResponse : public Response {
    virtual JSONItem To(const string& name = "") const { return Response::To(name); }
    virtual void From(const JSONItem& json) { Response::From(json); }
};

/// This event indicates that the debug adapter is ready to accept configuration requests (e.g. SetBreakpointsRequest,
/// SetExceptionBreakpointsRequest).
/// <-
struct InitializedEvent : public Event {
    InitializedEvent() { event = "initialized"; }
    virtual ~InitializedEvent() {}

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that the execution of the debuggee has stopped due to some condition.
/// This can be caused by a break point previously set, a stepping action has completed, by executing a debugger
/// statement etc.
struct StoppedEvent : public Event {
    /**
     * The reason for the event.
     * For backward compatibility this string is shown in the UI if the 'description' attribute is missing (but it must
     * not be translated). Values: 'step', 'breakpoint', 'exception', 'pause', 'entry', 'goto', 'function breakpoint',
     * 'data breakpoint', etc.
     */
    string reason;
    /**
     * Additional information. E.g. if reason is 'exception', text contains the exception name. This string is shown in
     * the UI.
     */
    string text;
    StoppedEvent() { event = "stopped"; }
    virtual ~StoppedEvent() {}

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that the execution of the debuggee has continued.
/// Please note: a debug adapter is not expected to send this event in response to a request that implies that execution
/// continues, e.g. 'launch' or 'continue'.  It is only necessary to send a 'continued' event if there was no previous
/// request that implied this
struct ContinuedEvent : public Event {
    /**
     * The thread which was continued.
     */
    int threadId = -1;
    /**
     * If 'allThreadsContinued' is true, a debug adapter can announce that all threads have continued.
     */
    bool allThreadsContinued = true;

    ContinuedEvent() { event = "continued"; }
    virtual ~ContinuedEvent() {}

    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that the debuggee has exited and returns its exit code.
struct ExitedEvent : public Event {
    int exitCode = 0;
    ExitedEvent() { event = "exited"; }
    virtual ~ExitedEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that debugging of the debuggee has terminated. This does not mean that the debuggee itself has
/// exited
struct TerminatedEvent : public Event {
    TerminatedEvent() { event = "terminated"; }
    virtual ~TerminatedEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that a thread has started or exited.
struct ThreadEvent : public Event {
    string reason;
    int threadId = 0;

    ThreadEvent() { event = "thread"; }
    virtual ~ThreadEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that a thread has started or exited.
// <-
struct OutputEvent : public Event {
    /**
     * The output category. If not specified, 'console' is assumed.
     * Values: 'console', 'stdout', 'stderr', 'telemetry', etc.
     */
    string category;
    string output;

    OutputEvent() { event = "output"; }
    virtual ~OutputEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// A Source is a descriptor for source code. It is returned from the debug adapter as part of a StackFrame and it is
/// used by clients when specifying breakpoints
struct Source : public Any {
    string name;
    string path;
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Information about a Breakpoint created in setBreakpoints or setFunctionBreakpoints.
struct Breakpoint : public Any {
    int id = -1;
    bool verified = false;
    string message;
    Source source;
    int line = -1;
    int column = -1;
    int endLine = -1;
    int endColumn = -1;
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that some information about a breakpoint has changed.
// <-
struct BreakpointEvent : public Event {
    /**
     * The output category. If not specified, 'console' is assumed.
     * Values: 'console', 'stdout', 'stderr', 'telemetry', etc.
     */
    string reason;
    /**
     * The 'id' attribute is used to find the target breakpoint and the other attributes are used as the new values.
     */
    Breakpoint breakpoint;

    BreakpointEvent() { event = "breakpoint"; }
    virtual ~BreakpointEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

#define PTR_SIZE (sizeof(void*))
/// The event indicates that the debugger has begun debugging a new process. Either one that it has launched, or one
/// that it has attached to
struct ProcessEvent : public Event {
    /**
     * The logical name of the process. This is usually the full path to process's executable file. Example:
     * /home/example/myproj/program.exe
     */
    string name;
    /**
     * The system process id of the debugged process. This property will be missing for non-system processes.
     */
    int systemProcessId = -1;
    /**
     * If true, the process is running on the same computer as the debug adapter.
     */
    bool isLocalProcess = true;
    /**
     * Describes how the debug engine started debugging this process.
     * 'launch': Process was launched under the debugger.
     * 'attach': Debugger attached to an existing process.
     * 'attachForSuspendedLaunch': A project launcher component has launched a new process in a suspended state and then
     * asked the debugger to attach.
     */
    string startMethod;
    int pointerSize = PTR_SIZE;
    ProcessEvent() { event = "process"; }
    virtual ~ProcessEvent() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

struct InitializeRequestArguments : public Any {
    /**
     * The ID of the (frontend) client using this adapter.
     */
    string clientID;
    /**
     * The human readable name of the (frontend) client using this adapter.
     */
    string clientName;
    /**
     * The ID of the debug adapter.
     */
    string adapterID;
    /**
     * The ISO-639 locale of the (frontend) client using this adapter, e.g. en-US or de-CH.
     */
    string locale = "en-US";
    bool linesStartAt1 = true;
    bool columnsStartAt1 = true;
    /**
     * Determines in what format paths are specified. The default is 'path', which is the native format.
     * Values: 'path', 'uri', etc.
     */
    string pathFormat = "path";
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The 'initialize' request is sent as the first request from the client to the debug adapter in order to configure it
/// with client capabilities and to retrieve capabilities from the debug adapter.  Until the debug adapter has responded
/// to with an 'initialize' response, the client must not send any additional requests or events to the debug adapter.
/// In addition the debug adapter is not allowed to send any requests or events to the client until it has responded
/// with an 'initialize' response.  The 'initialize' request may only be sent once.
/// <->
struct InitializeRequest : public Request {
    InitializeRequestArguments arguments;
    InitializeRequest() { command = "initialize"; }
    virtual ~InitializeRequest() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Response to 'initialize' request.
/// <-
struct InitializeResponse : public Response {
    InitializeResponse() {}
    virtual ~InitializeResponse() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The client of the debug protocol must send this request at the end of the sequence of configuration requests (which
/// was started by the 'initialized' event).
/// <->
struct ConfigurationDoneRequest : public Request {
    ConfigurationDoneRequest() { command = "configurationDone"; }
    virtual ~ConfigurationDoneRequest() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

struct EmptyAckResponse : public Response {
    EmptyAckResponse() {}
    virtual ~EmptyAckResponse() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Response to 'configurationDone' request. This is just an acknowledgement, so no body field is required.
/// <-
typedef EmptyAckResponse ConfigurationDoneResponse;

/// Arguments for 'launch' request. Additional attributes are implementation specific.
struct LaunchRequestArguments : public Any {
    /**
     * If noDebug is true the launch request should launch the program without enabling debugging.
     */
    bool noDebug = false;
    LaunchRequestArguments() {}
    virtual ~LaunchRequestArguments() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// The launch request is sent from the client to the debug adapter to start the debuggee with or without debugging (if
/// 'noDebug' is true). Since launching is debugger/runtime specific, the arguments for this request are not part of
/// this specification
struct LaunchRequest : public Request {
    LaunchRequestArguments arguments;
    LaunchRequest() { command = "launch"; }
    virtual ~LaunchRequest() {}
    virtual JSONItem To(const string& name = "") const;
    virtual void From(const JSONItem& json);
};

/// Response to 'launch' request. This is just an acknowledgement, so no body field is required.
/// <-
typedef EmptyAckResponse LaunchResponse;
}; // namespace dap

#endif // PROTOCOLMESSAGE_HPP
