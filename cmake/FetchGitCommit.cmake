# Download an exact git commit into SOURCE_DIR.
#
# Some projects occasionally rewrite history, so a pinned commit may no longer
# be reachable from any branch or tag. GitHub still allows fetching commits by
# SHA, so clone the repository and fetch the requested revision directly.

if(NOT GIT_EXECUTABLE OR NOT SOURCE_DIR OR NOT REPOSITORY OR NOT TAG)
    message(FATAL_ERROR "FetchGitCommit.cmake requires GIT_EXECUTABLE, SOURCE_DIR, REPOSITORY and TAG")
endif()

file(REMOVE_RECURSE "${SOURCE_DIR}")

execute_process(
    COMMAND "${GIT_EXECUTABLE}" clone --no-checkout "${REPOSITORY}" "${SOURCE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Cloning ${REPOSITORY} failed:\n${output}")
endif()

execute_process(
    COMMAND "${GIT_EXECUTABLE}" -C "${SOURCE_DIR}" fetch --no-tags origin "${TAG}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Fetching ${TAG} from ${REPOSITORY} failed:\n${output}")
endif()

execute_process(
    COMMAND "${GIT_EXECUTABLE}" -C "${SOURCE_DIR}" checkout --detach FETCH_HEAD
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Checking out ${TAG} failed:\n${output}")
endif()
