from m5.params import *
from m5.SimObject import SimObject

class HelloObject21(SimObject):
    type = 'HelloObject21'
    cxx_header = "src_740/hello_object21.hh"
    cxx_class = "gem5::HelloObject21"