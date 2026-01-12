# Script to copy DLLs to target directory
# Arguments:
#   ARGV0 = target directory (where to copy DLLs to)
#   ARGV1 = vcpkg_installed directory
#   ARGV2 = build directory

if(ARGC LESS 3)
    message(FATAL_ERROR "copy_dlls.cmake requires 3 arguments: target_dir vcpkg_dir build_dir")
endif()

set(TARGET_DIR "${ARGV0}")
set(VCPKG_DIR "${ARGV1}")
set(BUILD_DIR "${ARGV2}")

# Copy DLLs from vcpkg_installed
if(EXISTS "${VCPKG_DIR}")
    file(GLOB_RECURSE VCPKG_DLLS "${VCPKG_DIR}/*.dll")
    foreach(DLL ${VCPKG_DLLS})
        file(COPY "${DLL}" DESTINATION "${TARGET_DIR}")
    endforeach()
endif()

# Copy DLLs from build directory (e.g., custom libraries)
file(GLOB_RECURSE BUILD_DLLS 
    "${BUILD_DIR}/server/*.dll"
    "${BUILD_DIR}/client/*.dll"
    "${BUILD_DIR}/common/*.dll"
    "${BUILD_DIR}/tests/*.dll"
)
foreach(DLL ${BUILD_DLLS})
    file(COPY "${DLL}" DESTINATION "${TARGET_DIR}")
endforeach()
