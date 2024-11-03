# File: aliases_host.sh
# Author: Kevin Orbie
#
# Brief: Aliases providing a uniform interface to aid in the development on a host device.
#        A host device refers to the device that is used to run the docker dev environment on.


# =================================== Variables ===================================
if [[ -z "${APP_ROOT}" ]]; then
    # Default value if not yet defined, use this script's directory as root
    APP_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi


# ==================================== Aliases ====================================

# ------------------- Docker -------------------
alias rca-docker-build="sudo docker build -f ${APP_ROOT}/Dockerfile -t rca:v1 ${APP_ROOT}"
alias rca-docker-run="\
sudo docker run -it --rm \
    --detach \
	--gpus all \
    --privileged \
	--network=host \
	--name rca_env \
	--workdir /home/user/RCA \
	--runtime=nvidia \
    -v /.vscode/vscode-server:$HOME/.vscode-server \
	-v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY -e XAUTHORITY -e NVIDIA_DRIVER_CAPABILITIES=all \
	-h rca -v ${APP_ROOT}:/home/user/RCA \
	rca:v1\
"
alias rca-docker-stop="sudo docker stop rca_env"
alias rca-docker-exec="sudo docker exec -it -e 'TERM=xterm-256color' rca_env bash"


# ------------------- Syncing -------------------
rca-sync-to-robot() {
    echo ">>> Updating source code on the robot..."
    # Rsync to Robot:
    #  -a Archive mode; enables other options (-r -l -p -t -g -o -D, etc.)
    #  -v Verbose mode
    #  -R Keep relative paths (add '.' to path where to copy path)
    rsync -avR \
        $APP_ROOT/./source $APP_ROOT/./thirdparty/ffmpeg $APP_ROOT/./thirdparty/v4l2 \
        $APP_ROOT/./CMakeLists.txt $APP_ROOT/./aliases_target.sh \
        kevin@vehicle0.local:~/RCA
    echo ">>> Finished data transfer."
    return 0
}
