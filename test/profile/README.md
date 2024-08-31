# Profiling the Application (Linux)
This document describes how to use perf tool to profile this application, and visualize the reslts.

Usefull Resources:
- https://profiler.firefox.com/docs/#/./guide-perf-profiling
- https://stackoverflow.com/questions/2229336/linux-application-profiling

## Information Gathered
In which function the program spends the most time.

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
No setup required on Ubuntu 24.04.