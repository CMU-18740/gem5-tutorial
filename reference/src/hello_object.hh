#include <string>

#include "src_740/goodbye_object.hh"
#include "params/HelloObject.hh"
#include "sim/sim_object.hh"

namespace gem5 {
class HelloObject : public SimObject
{
  private:
    void processEvent();

    EventFunctionWrapper event;

    /// Pointer to the corresponding GoodbyeObject. Set via Python
    GoodbyeObject* goodbye;

    /// The name of this object in the Python config file
    const std::string myName;

    /// Latency between calling the event (in ticks)
    const Tick latency;

    /// Number of times left to fire the event before goodbye
    int timesLeft;

  public:
    HelloObject(const HelloObjectParams &p);

    void startup();
};

}