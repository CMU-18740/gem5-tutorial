#include "src_740/hello_object23.hh"

#include <iostream>
#include "base/trace.hh"
#include "debug/HelloExample.hh"

namespace gem5
{

HelloObject23::HelloObject23(const HelloObject23Params &params) :
    SimObject(params), event([this] {processEvent();}, name()),
    latency(100), timesLeft(10)
{
    DPRINTF(HelloExample, "Created the hello object\n");
}

void HelloObject23::processEvent()
{
    timesLeft--;
    DPRINTF(HelloExample, "Hello world! Processing the event! %d left\n", timesLeft);

    if (timesLeft <= 0) {
        DPRINTF(HelloExample, "Done firing!\n");
    } else {
        schedule(event, curTick() + latency);
    }
}

void HelloObject23::startup()
{
    schedule(event, latency);
}

} // namespace gem5