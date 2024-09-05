
# =================================== Variables ===================================
APP_ROOT="/home/kevin/projects/RCA"
PROJECT_NAME="main_controller"


# ==================================== Aliases ====================================
alias rca-setup-build-release="(cd $APP_ROOT/ && cmake -B./_build -DCMAKE_BUILD_TYPE=Release)"          # -O2, usable with perf
alias rca-setup-build-debug="(cd $APP_ROOT/ && cmake -B./_build -DCMAKE_BUILD_TYPE=RelWithDebInfo)"     # -O3
alias rca-build-install="(cd $APP_ROOT/ && cmake --build ./_build && cmake --install ./_build)"
alias rca-run="(cd $APP_ROOT/_deploy/$PROJECT_NAME && ./$PROJECT_NAME)"

alias rca-perf-run="(cd $APP_ROOT/_deploy/$PROJECT_NAME && sudo perf record -g -F 999 ./$PROJECT_NAME)"
alias rca-perf-check="(
    cd $APP_ROOT/_deploy/$PROJECT_NAME && 
    sudo chown $USER:$USER ./perf.data && 
    perf script -F +pid > ./test.perf &&
    firefox -new-tab "https://profiler.firefox.com/" &&
    nautilus .
)"

rca-top() {
    local pid=$(pgrep -u $USER $PROJECT_NAME)

    # Check if var is empty
    if [[ -z "$pid" ]]; then
        echo "ERROR: No active '$PROJECT_NAME' process was found!"
        return 1
    fi

    # Run top
    top -p $pid
    return 0
}
