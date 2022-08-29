---
title: Creating a very simple SimObject
doc: Learning gem5
author: Jason Lowe-Power (modified by Siddharth Sahay)
---


Creating a *very* simple SimObject
==================================

**Note**: gem5 has SimObject named `SimpleObject`. Implementing another
`SimpleObject` SimObject will result in confusing compiler issues.

Almost all objects in gem5 inherit from the base SimObject type.
SimObjects export the main interfaces to all objects in gem5. SimObjects
are wrapped `C++` objects that are accessible from the `Python`
configuration scripts.

SimObjects can have many parameters, which are set via the `Python`
configuration files. In addition to simple parameters like integers and
floating point numbers, they can also have other SimObjects as
parameters. This allows you to create complex system hierarchies, like
real machines.

In this chapter, we will walk through creating a simple "HelloWorld"
SimObject. The goal is to introduce you to how SimObjects are created
and the required boilerplate code for all SimObjects. We will also
create a simple `Python` configuration script which instantiates our
SimObject.

In the next few chapters, we will take this simple SimObject and expand
on it to include debugging support, dynamic events, and parameters.

Step 1: Create a Python class for your new SimObject
----------------------------------------------------

Each SimObject has a Python class which is associated with it. This
Python class describes the parameters of your SimObject that can be
controlled from the Python configuration files. For our simple
SimObject, we are just going to start out with no parameters. Thus, we
simply need to declare a new class for our SimObject and set its name
and the C++ header that will define the C++ class for the SimObject.

Once again, we will navigate to the *work* directory.
We can create a file, `HelloObject.py`, in `work/src`.

```python
from m5.params import *
from m5.SimObject import SimObject

class HelloObject(SimObject):
    type = 'HelloObject'
    cxx_header = "src_740/hello_object.hh"
    cxx_class = "gem5::HelloObject"
```

It is not required that the `type` be the same as the name of the class,
but it is convention. The `type` is the C++ class that you are wrapping
with this Python SimObject. Only in special circumstances should the
`type` and the class name be different.

The `cxx_header` is the file that contains the declaration of the class
used as the `type` parameter. Again, the convention is to use the name
of the SimObject with all lowercase and underscores, but this is only
convention. You can specify any header file here. Due to a technicality, you will need to prepend "src_740" to all the `cxx_header` paths - this is because the `work/src` directory is copied into the gem5 installation as `gem5/src/src_740`.

The `cxx_class` is an attribute specifying the newly created SimObject
is declared within the gem5 namespace. Most SimObjects in the gem5 code
base are declared within the gem5 namespace!

Step 2: Implement your SimObject in C++
---------------------------------------

Next, we need to create `hello_object.hh` and `hello_object.cc` in
`work/src` directory which will implement the `HelloObject`.

We'll start with the header file for our `C++` object. By convention,
gem5 wraps all header files in `#ifndef/#endif` with the name of the
file and the directory its in so there are no circular includes.

SimObjects should be declared within the gem5 namespace. Therefore,
we declare our class within the `namespace gem5` scope.

The only thing we need to do in the file is to declare our class. Since
`HelloObject` is a SimObject, it must inherit from the C++ SimObject
class. Most of the time, your SimObject's parent will be a subclass of
SimObject, not SimObject itself.

The SimObject class specifies many virtual functions. However, none of
these functions are pure virtual, so in the simplest case, there is no
need to implement any functions except for the constructor.

The constructor for all SimObjects assumes it will take a parameter
object. This parameter object is automatically created by the build
system and is based on the `Python` class for the SimObject, like the
one we created above. The name for this parameter type is generated
automatically from the name of your object. For our "HelloObject" the
parameter type's name is "HelloObjectParams".

The code required for our simple header file is listed below.

```cpp
#ifndef __HELLO_OBJECT_HH__
#define __HELLO_OBJECT_HH__

#include "params/HelloObject.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class HelloObject : public SimObject
{
  public:
    HelloObject(const HelloObjectParams &p);
};

} // namespace gem5

#endif //__HELLO_OBJECT_HH__
```

Next, we need to implement *two* functions in the associated `hello_object.cc` file, not just
one. The first function, is the constructor for the `HelloObject`. Here
we simply pass the parameter object to the SimObject parent and print
"Hello world!"

Normally, you would **never** use `std::cout` in gem5. Instead, you
should use debug flags. In the next chapter, we
will modify this to use debug flags instead. However, for now, we'll
simply use `std::cout` because it is simple.

Notice again that the `#include` path to your header has a `src_740` prepended to it.

```cpp
#include "src_740/hello_object.hh"

#include <iostream>

namespace gem5
{

HelloObject::HelloObject(const HelloObjectParams &params) :
    SimObject(params)
{
    std::cout << "Hello World! From a SimObject!" << std::endl;
}

} // namespace gem5
```

**Note**: If the constructor of your SimObject follows the following
signature,

```cpp
Foo(const FooParams &)
```

then a `FooParams::create()` method will be automatically defined. The purpose
of the `create()` method is to call the SimObject constructor and return an
instance of the SimObject. Most SimObject will follow this pattern; however,
if your SimObject does not follow this pattern,
[the gem5 SimObject documetation](http://doxygen.gem5.org/release/current/classSimObject.html#details)
provides more information about manually implementing the `create()` method.


Step 3: Register the SimObject and C++ file
-------------------------------------------

In order for the `C++` file to be compiled and the `Python` file to be
parsed we need to tell the build system about these files. gem5 uses
SCons as the build system, so you simply have to create a SConscript
file in the directory with the code for the SimObject. If there is
already a SConscript file for that directory, simply add the following
declarations to that file.

This file is simply a normal `Python` file, so you can write any
`Python` code you want in this file. Some of the scripting can become
quite complicated. gem5 leverages this to automatically create code for
SimObjects and to compile the domain-specific languages like SLICC and
the ISA language.

In the SConscript file, there are a number of functions automatically
defined after you import them. See the section on that...

To get your new SimObject to compile, you simply need to create a new
file with the name "SConscript" in the `work/src` directory. In
this file, you have to declare the SimObject and the `.cc` file. Below
is the required code.

```python
Import('*')

SimObject('HelloObject.py', sim_objects=['HelloObject'])
Source('hello_object.cc')
```

Step 4: (Re)-build gem5
-----------------------

To compile and link your new files you simply need to recompile gem5. You have already done this once in Task 1. We use a wrapper script to copy the src files to the gem5 directory and pull in a different toolchain and settings to make the build process much faster. From inside the *work* directory run

```
./rebuild_gem5
```

Step 5: Create the config scripts to use your new SimObject
-----------------------------------------------------------

Now that you have implemented a SimObject, and it has been compiled into
gem5, you need to create or modify a `Python` config file `run_hello.py` in
`work/configs` to instantiate your object. Since your object
is very simple a system object is not required! CPUs are not needed, or
caches, or anything, except a `Root` object. All gem5 instances require a
`Root` object.

Walking through creating a *very* simple configuration script, first,
import m5 and all of the objects you have compiled.

```python
import m5
from m5.objects import *
```

Next, you have to instantiate the `Root` object, as required by all gem5
instances.

```python
root = Root(full_system = False)
```

Now, you can instantiate the `HelloObject` you created. All you need to
do is call the `Python` "constructor". Later, we will look at how to
specify parameters via the `Python` constructor. In addition to creating
an instantiation of your object, you need to make sure that it is a
child of the root object. Only SimObjects that are children of the
`Root` object are instantiated in `C++`.

```python
root.hello = HelloObject()
```

Finally, you need to call `instantiate` on the `m5` module and actually
run the simulation!

```python
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick {} because {}'
      .format(m5.curTick(), exit_event.getCause()))
```

Remember to rebuild gem5 after modifying files in the src/ directory.

Run `run_hello.py` like before

```
./gem5.opt configs/run_hello.py
```

It should give you output resembling the below
```
    gem5 Simulator System.  http://gem5.org
    gem5 is copyrighted software; use the --copyright option for details.

    gem5 compiled May  4 2016 11:37:41
    gem5 started May  4 2016 11:44:28
    gem5 executing on mustardseed.cs.wisc.edu, pid 22480
    command line: build/X86/gem5.opt configs/learning_gem5/part2/run_hello.py

    Global frequency set at 1000000000000 ticks per second
    Hello World! From a SimObject!
    Beginning simulation!
    info: Entering event queue @ 0.  Starting simulation...
    Exiting @ tick 18446744073709551615 because simulate() limit reached
```
Congrats! You have written your first SimObject. In the next chapters,
we will extend this SimObject and explore what you can do with
SimObjects.
