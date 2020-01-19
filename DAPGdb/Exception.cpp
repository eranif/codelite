#include "Exception.hpp"

Exception::Exception(const string& what)
    : m_what(what)
{
}

Exception::~Exception() {}
