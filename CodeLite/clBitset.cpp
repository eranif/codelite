#include "clBitset.hpp"

#define CHECK_SIZE_RET_FALSE(pos) \
    if(pos >= BITS_SIZE)          \
    return false

#define CHECK_SIZE_RET(pos) \
    if(pos >= BITS_SIZE)    \
    return

clBitset::clBitset() { memset(bits, 0, sizeof(bits)); }

clBitset::~clBitset() {}

void clBitset::set(size_t pos, bool b)
{
    CHECK_SIZE_RET(pos);
    bits[pos] = b ? 1 : 0;
}

bool clBitset::test(size_t pos) const
{
    CHECK_SIZE_RET_FALSE(pos);
    return bits[pos] == 1;
}

void clBitset::rese() { memset(bits, 0, sizeof(bits)); }

wxString clBitset::to_string() const
{
    wxString str;
    for(auto ch : bits) {
        str << (ch == 1 ? "1" : "0");
    }
    return str;
}

void clBitset::from_string(const wxString& str)
{
    if(str.length() < BITS_SIZE)
        return;

    for(size_t i = 0; i < BITS_SIZE; ++i) {
        bits[i] = (str[i] == '1' ? 1 : 0);
    }
}
