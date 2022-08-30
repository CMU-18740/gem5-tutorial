from m5.params import *
from m5.SimObject import SimObject

class HelloObject22(SimObject):
    type = 'HelloObject22'
    cxx_header = "src_740/hello_object22.hh"
    cxx_class = "gem5::HelloObject22"