#ifndef __HELLO_OBJECT21_HH__
#define __HELLO_OBJECT21_HH__

#include "params/HelloObject21.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class HelloObject21 : public SimObject
{
  public:
    HelloObject21(const HelloObject21Params &p);
};

} // namespace gem5

#endif //__HELLO_OBJECT21_HH__