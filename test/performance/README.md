# Recording the Application Performance (Linux)
This file describes how we can record this program's performance on linux.

- TODO: Give an overview of the PIPELINE TIMINGS, and specific sub function timings (without requiring a camera to be connected).
    - We should be able to verify what the max achievable FPS is on any platform.
- TODO: Give an overview of the MEMORY usage after running a representative example program (stack, heap, static) (without requiring a camera to be connected).
    - We should be able to find the memory requirements, this should be more or less independent from the platform used.

<span style="color: red">
!!! NOTE: The steps in this document are depricated, as they were too complex for what they did. <br>
!!! For now, to save time, we will just use `top` as normal, with a custom configuration.
</span>

Usefull Resources:
- https://man7.org/linux/man-pages/man1/top.1.html
- https://stackoverflow.com/questions/2229336/linux-application-profiling
- https://stackoverflow.com/questions/7998302/graphing-a-processs-memory-usage


## Information Gathered
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


## Run Instrutions
Record application with top:
```shell
# Trick top in selecting custom config
export HOME=/home/kevin/projects/20240824_video_viewer/test/performance/

# Start top
# -b          : Batch mode (non-interactive).
# -w <int>    : Char width limit (by default limited by terminal size).
# -p <PID>    : PID of the process to monitor.
# -d <float>  : [Optional] Delay between performance samples (seconds).
# -E k        : Force summary area memory to be scaled as (KiB), no automatic downscaling
top -b -w256 -d 0.25 -E k -e k -p 12638 > test.txt
```

Visualize top Logs:
```shell
# Activate Venv
cd <root>/test/
source /venv/bin/activate

# Run Logger
python logger.py <logfile>
```

## Setup Instrutions

### Setting up python
Adding virtual python environment:
```shell
# Create Evironment
cd <root>/test/
python3 -m venv venv

# Activate Venv
source /venv/bin/activate

# Setup Venv
pip install -r requirements.txt
``` 


### Re-Configuring top
Change top's columns:
- Start top in interactive mode: `top`
- Configure columns with `F`
- Press `shift + w` to save the current view

Change refresh rate ([source](https://askubuntu.com/questions/82171/update-rate-of-top))
- Open [top's config file](./.config/procps/toprc).
- Update `Delay_time=...` at the top if the file (in seconds).

## Notes
- In the first iteration, `top` takes a lot more CPU resources, then the following iterations, that is why I did not opt for using `-n1` option in a loop. 
- We use a custom Top Config:
    - Has no generalized header
    - Has all the fields needed
