#ifndef __HELLO_OBJECT22_HH__
#define __HELLO_OBJECT22_HH__

#include "params/HelloObject22.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class HelloObject22 : public SimObject
{
  public:
    HelloObject22(const HelloObject22Params &p);
};

} // namespace gem5

#endif //__HELLO_OBJECT22_HH__