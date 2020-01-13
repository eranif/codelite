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

    virtual JSONItem To(const string& name) const = 0;
    virtual void From(const JSONItem& json) = 0;

    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Any*>(this)); }
    typedef shared_ptr<Any> Ptr_t;
};

/// Base class of requests, responses, and events
struct ProtocolMessage : public Any {
    int number = -1;
    string type;

    ProtocolMessage() {}
    virtual ~ProtocolMessage() {}

    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

/// A client or debug adapter initiated request
/// ->
struct Request : public ProtocolMessage {
    string command;
    Any::Ptr_t arguments;

    Request() { type = "request"; }
    virtual ~Request() = 0; // force to abstract class

    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

/// The ‘cancel’ request is used by the frontend to indicate that it is no longer interested in the result produced by a
/// specific request issued earlier.
/// <->
struct CancelRequest : public Request {
    string command;
    int requestId = -1;

    CancelRequest() { type = "cancel"; }
    virtual ~CancelRequest() {}

    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

/// A debug adapter initiated event
///
struct Event : public ProtocolMessage {
    string event;
    Any::Ptr_t body;

    Event() { type = "event"; }
    virtual ~Event() = 0; // force to abstract class

    virtual JSONItem To(const string& name) const;
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
    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

/// Response to ‘cancel’ request. This is just an acknowledgement, so no body field is required.
/// <-
struct CancelResponse : public Response {
    virtual JSONItem To(const string& name) const { return Response::To(name); }
    virtual void From(const JSONItem& json) { Response::From(json); }
};

/// This event indicates that the debug adapter is ready to accept configuration requests (e.g. SetBreakpointsRequest,
/// SetExceptionBreakpointsRequest).
/// <-
struct InitializedEvent : public Event {
    InitializedEvent() { event = "initialized"; }
    virtual ~InitializedEvent() {}

    virtual JSONItem To(const string& name) const;
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

    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

/// The event indicates that the execution of the debuggee has continued.
/// Please note: a debug adapter is not expected to send this event in response to a request that implies that execution
/// continues, e.g. ‘launch’ or ‘continue’.  It is only necessary to send a ‘continued’ event if there was no previous
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

    virtual JSONItem To(const string& name) const;
    virtual void From(const JSONItem& json);
};

}; // namespace dap

#endif // PROTOCOLMESSAGE_HPP
