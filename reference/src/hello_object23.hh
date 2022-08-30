#ifndef __HELLO_OBJECT23_HH__
#define __HELLO_OBJECT23_HH__

#include "params/HelloObject23.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class HelloObject23 : public SimObject
{
  private:
    void processEvent();

    EventFunctionWrapper event;

    const Tick latency;

    int timesLeft;

  public:
    HelloObject23(const HelloObject23Params &p);

    void startup();
};

} // namespace gem5

#endif //__HELLO_OBJECT23_HH__