#include "src_740/hello_object22.hh"

#include <iostream>
#include "base/trace.hh"
#include "debug/HelloExample.hh"

namespace gem5
{

HelloObject22::HelloObject22(const HelloObject22Params &params) :
    SimObject(params)
{
    DPRINTF(HelloExample, "Created the hello object\n");
}

} // namespace gem5