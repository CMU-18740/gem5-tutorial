---
title: Adding cache to configuration script
doc: Learning gem5
author: Jason Lowe-Power (modified by Siddharth Sahay)
---


Adding cache to the configuration script
========================================

Using the previous configuration script as a starting point,
this chapter will walk through a more complex configuration. We will add
a cache hierarchy to the system as shown in
the figure below. We will also see how to add command-line parameters to the script.

![A system configuration with a two-level cache
hierarchy.](figures/advanced_config.png)

Creating cache objects
----------------------

We are going to use the classic caches, instead of Ruby,
since we are modeling a single CPU system and we don't care about
modeling cache coherence. We will extend the `Cache` SimObject and
configure it for our system. First, we must understand the parameters
that are used to configure Cache objects.

> **Classic caches and Ruby**
>
> gem5 currently has two completely distinct subsystems to model the
> on-chip caches in a system, the "Classic caches" and "Ruby". The
> historical reason for this is that gem5 is a combination of m5 from
> Michigan and GEMS from Wisconsin. GEMS used Ruby as its cache model,
> whereas the classic caches came from the m5 codebase (hence
> "classic"). The difference between these two models is that Ruby is
> designed to model cache coherence in detail. Part of Ruby is SLICC, a
> language for defining cache coherence protocols. On the other hand,
> the classic caches implement a simplified and inflexible MOESI
> coherence protocol.
>
> To choose which model to use, you should ask yourself what you are
> trying to model. If you are modeling changes to the cache coherence
> protocol or the coherence protocol could have a first-order impact on
> your results, use Ruby. Otherwise, if the coherence protocol isn't
> important to you, use the classic caches.
>
> A long-term goal of gem5 is to unify these two cache models into a
> single holistic model.

### Cache

The Cache SimObject declaration can be found in *buildspace/gem5/src/mem/cache/Cache.py*.
This Python file defines the parameters which you can set of the
SimObject. Under the hood, when the SimObject is instantiated these
parameters are passed to the C++ implementation of the object. The
`Cache` SimObject inherits from the `BaseCache` object.

Within the `BaseCache` class, there are a number of *parameters*. For
instance, `assoc` is an integer parameter. Some parameters, like
`write_buffers` have a default value, 8 in this case. The default
parameter is the first argument to `Param.*`, unless the first argument
is a string. The string argument of each of the parameters is a
description of what the parameter is (e.g.,
`tag_latency = Param.Cycles("Tag lookup latency")` means that the
`` tag_latency `` controls "The hit latency for this cache").

Many of these parameters do not have defaults, so we are required to set
these parameters before calling `m5.instantiate()`.

* * * * *

Now, to create caches with specific parameters, we are first going to
create a new file, `caches.py`, in the same directory as simple.py,
`work/configs`. The first step is to import the SimObject(s)
we are going to extend in this file.

```python
from m5.objects import Cache
```

Next, we can treat the `Cache` object just like any other Python class
and extend it. We can name the new cache anything we want. Let's start
by making an L1 cache.

```python
class L1Cache(Cache):
    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20
```

Here, we are setting some of the parameters of the `Cache` that do not
have default values. To see all of the possible configuration options,
and to find which are required and which are optional, you have to look
at the source code of the SimObject.

We have extended `Cache` and set most of the parameters that do not
have default values in the `Cache` SimObject. Next, let's two more
sub-classes of L1Cache, an L1DCache and L1ICache

```python
class L1ICache(L1Cache):
    size = '16kB'

class L1DCache(L1Cache):
    size = '64kB'
```

Let's also create an L2 cache with some reasonable parameters.

```python
class L2Cache(Cache):
    size = '256kB'
    assoc = 8
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12
```

Now that we have specified all of the necessary parameters required for
`Cache`, all we have to do is instantiate our sub-classes and
connect the caches to the interconnect. However, connecting lots of
objects up to complex interconnects can make configuration files quickly
grow and become unreadable. Therefore, let's first add some helper
functions to our sub-classes of `Cache`. Remember, these are just Python
classes, so we can do anything with them that you can do with a Python
class.

To the L1 cache let's add two functions, `connectCPU` to connect a CPU
to the cache and `connectBus` to connect the cache to a bus. We need to
add the following code to the `L1Cache` class.

```python
def connectCPU(self, cpu):
    # need to define this in a base class!
    raise NotImplementedError

def connectBus(self, bus):
    self.mem_side = bus.cpu_side_ports
```

Next, we have to define a separate `connectCPU` function for the
instruction and data caches, since the I-cache and D-cache ports have a
different names. Our `L1ICache` and `L1DCache` classes now become:

```python
class L1ICache(L1Cache):
    size = '16kB'

    def connectCPU(self, cpu):
        self.cpu_side = cpu.icache_port

class L1DCache(L1Cache):
    size = '64kB'

    def connectCPU(self, cpu):
        self.cpu_side = cpu.dcache_port
```

Finally, let's add functions to the `L2Cache` to connect to the
memory-side and CPU-side bus, respectively.

```python
def connectCPUSideBus(self, bus):
    self.cpu_side = bus.mem_side_ports

def connectMemSideBus(self, bus):
    self.mem_side = bus.cpu_side_ports
```

Adding caches to the simple config file
------------------------------------

Now, let's add the caches we just created to the configuration script we
created in the last chapter.

First, let's copy the script to a new name. Assuming you are in the *work* directory

```
cp configs/simple.py configs/two_level.py
```

First, we need to import the names from the `caches.py` file into the
namespace. We can add the following to the top of the file (after the
m5.objects import), as you would with any Python source.

```python
from caches import *
```

Now, after creating the CPU, let's create the L1 caches:

```python
system.cpu.icache = L1ICache()
system.cpu.dcache = L1DCache()
```

And connect the caches to the CPU ports with the helper function we
created.

```python
system.cpu.icache.connectCPU(system.cpu)
system.cpu.dcache.connectCPU(system.cpu)
```

You need to *remove* the following two lines which connected the cache
ports directly to the memory bus.

```python
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports
```

We can't directly connect the L1 caches to the L2 cache since the L2
cache only expects a single port to connect to it. Therefore, we need to
create an L2 bus to connect our L1 caches to the L2 cache. The, we can
use our helper function to connect the L1 caches to the L2 bus.

```python
system.l2bus = L2XBar()

system.cpu.icache.connectBus(system.l2bus)
system.cpu.dcache.connectBus(system.l2bus)
```

Next, we can create our L2 cache and connect it to the L2 bus and the
memory bus.

```python
system.l2cache = L2Cache()
system.l2cache.connectCPUSideBus(system.l2bus)
system.membus = SystemXBar()
system.l2cache.connectMemSideBus(system.membus)
```

Note that `system.membus = SystemXBar()` has been defined before
`system.l2cache.connectMemSideBus` so we can pass it to
`system.l2cache.connectMemSideBus`. Everything else in the file
stays the same! Now we have a complete configuration with a
two-level cache hierarchy. If you run the current file, `hello`
should now finish in 57467000 ticks (or similar). 

Adding parameters to your script
--------------------------------

When performing experiments with gem5, you don't want to edit your
configuration script every time you want to test the system with
different parameters. To get around this, you can add command-line
parameters to your gem5 configuration script. Again, because the
configuration script is just Python, you can use the Python libraries
that support argument parsing. Although pyoptparse is officially
deprecated, many of the configuration scripts that ship with gem5 use it
instead of pyargparse since gem5's minimum Python version used to be
2.5. The minimum Python version is now 3.6, so Python's argparse is a better
option when writing new scripts that don't need to interact with the
current gem5 scripts. To get started using :pyoptparse, you can consult
the online Python documentation.

To add options to our two-level cache configuration, after importing our
caches, let's add some options. First, make a copy of `two_level.py`, say `two_level_opt.py`.
Also make a copy of `caches.py`, say `caches_opt.py` (all of these in `work/configs` of course). We also need to change the cache import in `two_level_opt.py` to use `caches_opt.py` since we'll be editing the caches too.

```python
from caches_opt import *
import argparse

parser = argparse.ArgumentParser(description='A simple system with 2-level cache.')
parser.add_argument("binary", default="tests/test-progs/hello/bin/arm/linux/hello", nargs="?", type=str,
                    help="Path to the binary to execute.")
parser.add_argument("--l1i_size",
                    help=f"L1 instruction cache size. Default: 16kB.")
parser.add_argument("--l1d_size",
                    help="L1 data cache size. Default: Default: 64kB.")
parser.add_argument("--l2_size",
                    help="L2 cache size. Default: 256kB.")

options = parser.parse_args()
```
Note that if you wanted to pass the binary file's path the way shown above
and use it through options, you should specify it as `options.binary`.
For example:

```python
system.workload = SEWorkload.init_compatible(options.binary)
```
You'll also need to change the `process.cmd` value similarly. You could set the old binary path as the default value for `options.binary` in the `add_argument` call at the top of the file.
Now, you can run
`./gem5.opt configs/two_level_opt.py --help` which
will display the options you just added.

Next, we need to pass these options onto the caches that we create in
the configuration script. To do this, we'll simply change two\_level.py
to pass the options into the caches as a parameter to their constructor
and add an appropriate constructor, next.

```python
system.cpu.icache = L1ICache(options)
system.cpu.dcache = L1DCache(options)
...
system.l2cache = L2Cache(options)
```

In `caches_opt.py` (rememeber, we aren't using `caches.py` anymore), we need to add constructors (`__init__` functions in
Python) to each of our classes. Starting with our base L1 cache, we'll
just add an empty constructor since we don't have any parameters which
apply to the base L1 cache. However, we can't forget to call the super
class's constructor in this case. If the call to the super class
constructor is skipped, gem5's SimObject attribute finding function will
fail and the result will be
"`RuntimeError: maximum recursion depth exceeded`" when you try to
instantiate the cache object. So, in `L1Cache` we need to add the
following after the static class members.

```python
def __init__(self, options=None):
    super(L1Cache, self).__init__()
    pass
```

Next, in the `L1ICache`, we need to use the option that we created
(`l1i_size`) to set the size. In the following code, there is guards for
if `options` is not passed to the `L1ICache` constructor and if no
option was specified on the command line. In these cases, we'll just use
the default we've already specified for the size.

```python
def __init__(self, options=None):
    super(L1ICache, self).__init__(options)
    if not options or not options.l1i_size:
        return
    self.size = options.l1i_size
```

We can use the same code for the `L1DCache`:

```python
def __init__(self, options=None):
    super(L1DCache, self).__init__(options)
    if not options or not options.l1d_size:
        return
    self.size = options.l1d_size
```

And the unified `L2Cache`:

```python
def __init__(self, options=None):
    super(L2Cache, self).__init__()
    if not options or not options.l2_size:
        return
    self.size = options.l2_size
```

With these changes, you can now pass the cache sizes into your script
from the command line like below. You can also pass the binary to execute as a command line param, and if you don't it'll fall back to the `hello` binary.

```sh
./gem5.opt configs/two_level_opt.py --l2_size='1MB' --l1d_size='128kB'
```

As before the gem5 version, command line, and ticks may differ

    gem5 Simulator System.  http://gem5.org
    gem5 is copyrighted software; use the --copyright option for details.

    gem5 version 21.0.0.0
    gem5 compiled May 17 2021 18:05:59
    gem5 started May 18 2021 00:00:33
    gem5 executing on amarillo, pid 83118
    command line: build/X86/gem5.opt configs/tutorial/two_level.py --l2_size=1MB --l1d_size=128kB

    Global frequency set at 1000000000000 ticks per second
    warn: No dot file generated. Please install pydot to generate the dot file and pdf.
    warn: DRAM device capacity (8192 Mbytes) does not match the address range assigned (512 Mbytes)
    0: system.remote_gdb: listening for remote gdb on port 7005
    Beginning simulation!
    info: Entering event queue @ 0.  Starting simulation...
    Hello world!
    Exiting @ tick 57467000 because exiting with last active thread context
 
### Next Page
[Part 1.3: Understanding Stats](part1_3_gem5_stats.md)
