#pragma once

namespace llm
{
enum class ChatState {
    kReady,
    kThinking,
    kWorking,
};

enum class UserAnswer {
    kYes,
    kNo,
    kTrust,
};

} // namespace llm