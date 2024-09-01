# Profiling the Application (Linux)
This document describes what tools we use to dynamically profile different aspects of our application.

# General Real-Time Profiling
To get a first indication of how our code is running, we can run `top` or `htop` while running our code.<br>
Using `top` allows us to watch both the CPU usage, and different type of MEMORY usage.

Metrics available with top:
- CPU Usage over time
    - **%CPU**: CPU usage since last sample.
    - **%CUU**: Total CPU time / run time.

- Memory Usage over time (Primary)
    - **%MEM**: RES in percentage.
    - **RES**:  Non-swapped physical memory (RAM).
    - **SWAP**: RAM overflow, written to the Swap Space.
    - **VIRT**: Total amount of virtual memory used by the task.
- Memory Usage over time (Extra)
    - **SHR**:  Shared subset of resident memory (< RES).
    - **DATA**: Private memory reserved by a process (> RES, < VIRT).
    - **CODE**: Code in RAM.
    - **USED**: RES + SWAP

Usefull Resources:
- https://man7.org/linux/man-pages/man1/top.1.html

## Run Instrutions (With Aliases)
After starting the application, run the following in a different terminal:
```shell
# Run top, showing only information relevant to our aplication
rca-top
```

## Run Instrutions (Vanilla)
```shell
# Run top (possibly with following options)
# -p <PID>    : PID of the process to monitor.
# -d <float>  : Delay between performance samples (seconds).
# -E k        : Force summary area memory to be scaled as (KiB), no automatic downscaling
top [options]
```

# CPU Usage Profiling
To find bottlenecks in the code, this section describes how we can find which percentage of CPU time, each of the functions take up. 
For this we make use of **perf**.

Usefull Resources:
- https://profiler.firefox.com/docs/#/./guide-perf-profiling
- https://stackoverflow.com/questions/2229336/linux-application-profiling

## Run Instrutions (With Aliases)
```shell
# Compile the C++ program with RelWithDebInfo:
rca-setup-build-debug

# Run Program with Perf
rca-perf-run

# Run firefox profiler with Perf
rca-perf-check
```

## Run Instrutions (Vanilla)
Compile the C++ program with RelWithDebInfo.

Run profiler (two options):
- Run program with **perf**:
```shell
sudo perf record -g -F 999 ./$PROJECT_NAME -o ./perf.data
sudo chown $USER:$USER ./perf.data
```
- Attach to running program with **perf**:
```shell
sudo perf record -g -F 999 -p $PROGRAM_PID -o ./perf.data
sudo chown $USER:$USER ./perf.data
```

### Perf Tools
Viewing with Perf Tools:
- Analyze the load per module:
```shell
perf report --stdio -g none --sort comm,dso -i ./perf.data
```
- Analyse Call Chains
```shell
perf report --stdio -g graph -i ./perf.data | c++filt
```

### Firefox Profiler
When viewing in Firefox Profiler: ([source](https://profiler.firefox.com/docs/#/./guide-perf-profiling))
1. Convert the generated `perf.data` file into text form:
```shell
perf script -F +pid > /tmp/test.perf
```
2. View the resuls here: https://profiler.firefox.com/ 


## Setup Instrutions
No setup required for this on Ubuntu 24.04.

# Memory Usage Profiling
Possibly use this: https://github.com/KDE/heaptrack

