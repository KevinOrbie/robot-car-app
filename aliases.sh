
# =================================== Variables ===================================
PROJECT_NAME="engine"

if [[ -z "${APP_ROOT}" ]]; then
    # Default value if not yet defined
    APP_ROOT="/home/kevin/projects/RCA"
fi

# ==================================== Aliases ====================================
# NOTE: cmake --install is from CMAKE 3.15 or later
alias rca-setup-build-release="(cd $APP_ROOT/ && cmake -S . -B./_build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING='OFF')"          # -O2, usable with perf
alias rca-setup-build-debug="(cd $APP_ROOT/ && cmake -S . -B./_build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING='OFF')"     # -O3
alias rca-build-only="(cd $APP_ROOT/ && cmake --build ./_build)" 

rca-install() {
    cmake_ver_req=3.15
    cmake_ver=$(cmake --version | head -1 | cut -f3 -d" ")

    # Sort the two version values
    mapfile -t sorted < <(printf "%s\n" "$cmake_ver" "$cmake_ver_req" | sort -V)

    # If CMake Version is older then version required for "--install"
    if [[ ${sorted[0]} == "$cmake_ver_req" ]]; then
        (cd $APP_ROOT/ && cmake --install ./_build)
    else
        # CMake version older than 3.15, "--install" not supported
        # Assume Make used
        (cd $APP_ROOT/_build/ && make install)
    fi
}

alias rca-build-install="rca-build-only && rca-install"
alias rca-run="(cd $APP_ROOT/_deploy/$PROJECT_NAME && ./$PROJECT_NAME)"

# ----------------- Testing -----------------
alias rca-test-setup-build="(cd $APP_ROOT/ && cmake -B./_build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING='ON')"
alias rca-test-build-install="rca-build-install"

rca-test-run() {
    if [[ $# -eq 1 ]] ; then
        echo ">>> Running tests that match regex: '$1'"
        (cd $APP_ROOT/ && ctest -R $1 -V --test-dir ./_build)
    else
        echo ">>> Running all tests..."
        (cd $APP_ROOT/ && ctest --test-dir ./_build)
    fi

    return 0
}

# ---------------- Profiling ----------------
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

# ------------------ Robot ------------------
rca-sync-to-robot() {
    echo ">>> Updating source code on the robot..."
    # Rsync to Robot:
    #  -a Archive mode; enables other options (-r -l -p -t -g -o -D, etc.)
    #  -v Verbose mode
    #  -R Keep relative paths (add '.' to path where to copy path)
    rsync -avR \
        $APP_ROOT/./source $APP_ROOT/./thirdparty/ffmpeg $APP_ROOT/./thirdparty/v4l2 \
        $APP_ROOT/./CMakeLists.txt $APP_ROOT/./aliases.sh \
        kevin@vehicle0.local:~/RCA
    echo ">>> Finished data transfer."
    return 0
}
