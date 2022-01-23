#include <string>

using namespace std;
namespace
{
struct MyAnonStruct {
    string name;      // the name
    string last_name; // last name
};
} // namespace

void foo()
{
    MyAnonStruct c;
