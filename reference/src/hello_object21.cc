#include "src_740/hello_object21.hh"

#include <iostream>

namespace gem5
{

HelloObject21::HelloObject21(const HelloObject21Params &params) :
    SimObject(params)
{
    std::cout << "Hello World! From a SimObject!" << std::endl;
}

} // namespace gem5