cmake_minimum_required(VERSION 3.5)

message("------Start generating version info-----")

execute_process(
    COMMAND bash "${CMAKE_CURRENT_LIST_DIR}/../scripts/check_version_from_git.sh"
)

message("----------------- done -----------------")
