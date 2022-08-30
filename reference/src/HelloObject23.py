from m5.params import *
from m5.SimObject import SimObject

class HelloObject23(SimObject):
    type = 'HelloObject23'
    cxx_header = "src_740/hello_object23.hh"
    cxx_class = "gem5::HelloObject23"