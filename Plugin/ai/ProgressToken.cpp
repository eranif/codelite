#include "ai/ProgressToken.hpp"
namespace llm
{
CancellationToken::CancellationToken(size_t max_tokens)
    : m_max_tokens(max_tokens)
{
}

CancellationToken::~CancellationToken() {}

} // namespace llm
