# CMakeLists.txt

# CMake setup
cmake_minimum_required (VERSION 3.8)

# Set a default build type if none was specified
set(default_build_type "RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Project initialisation
project("CovidSim")

option(USE_OPENMP "Compile with OpenMP parallelism enabled" ON)
option(USE_TIMEMORY "Compile with timemory performance tracking" OFF)

# Add an interface library that will provide timemory if available
# linking to this interface library will have no effect if timemory is not used
add_library(covidsim-packages INTERFACE)

if(USE_TIMEMORY)
    # specify the C++ standard -- timemory requires a minimum of C++14
    set(CMAKE_CXX_STANDARD 14)

    # default set of timemory components that will be required
    set(_COMPONENTS
        headers)
    # default set of optional components that will included if available
    set(_OPTIONAL_COMPONENTS
        arch cxx-shared gperftools caliper ompt ompt-library
        plotting statistics allinea-map craypat gotcha cpu-roofline likwid
        papi tau)

    # cache these variables so subsequent invocations of cmake can change them
    set(TIMEMORY_COMPONENTS "${_COMPONENTS}" CACHE STRING "timemory required components")
    set(TIMEMORY_OPTIONAL_COMPONENTS "${_OPTIONAL_COMPONENTS}" CACHE STRING "timemory required components")

    # configure timemory install to use this interface library
    set(timemory_FIND_COMPONENTS_INTERFACE covidsim-packages)

    # find the timemory package
    find_package(timemory 3.1.0 REQUIRED
        COMPONENTS          ${TIMEMORY_COMPONENTS}
        OPTIONAL_COMPONENTS ${TIMEMORY_OPTIONAL_COMPONENTS})

    # set pre-processor to use timemory
    target_compile_definitions(covidsim-packages INTERFACE COVIDSIM_USE_TIMEMORY)
    # if the shared/static library was found, make sure we have pre-processor to use those calls
    if(TARGET timemory::timemory-cxx-shared)
        target_compile_definitions(covidsim-packages INTERFACE COVIDSIM_USE_TIMEMORY_LIBRARY)
        target_link_libraries(covidsim-packages INTERFACE timemory::timemory-cxx-shared)
    endif()
else()
    # specify the C++ standard
    set(CMAKE_CXX_STANDARD 11)
endif()

# find after timemory bc if timemory is enabled, we want to find the
# timemory install of OpenMP if possible since that includes
# OpenMP tools performance info
if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  target_link_libraries(covidsim-packages INTERFACE OpenMP::OpenMP_CXX)
endif()

# CovidSim target
add_executable(CovidSim CovidSim.cpp CovidSim.h binio.cpp binio.h Rand.cpp Rand.h Constants.h Country.h MachineDefines.h Error.cpp Error.h Dist.cpp Dist.h Kernels.cpp Kernels.h Bitmap.cpp Bitmap.h Model.h Param.h SetupModel.cpp SetupModel.h SharedFuncs.h ModelMacros.h InfStat.h CalcInfSusc.cpp CalcInfSusc.h Sweep.cpp Sweep.h Update.cpp Update.h Perf.h Perf.cpp)
target_include_directories(CovidSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CovidSim PUBLIC covidsim-packages)
if(WIN32)
  target_link_libraries(CovidSim PUBLIC Gdiplus.lib Vfw32.lib)
  target_compile_definitions(CovidSim PUBLIC  "_CRT_SECURE_NO_WARNINGS")
elseif(UNIX)
  target_compile_definitions(CovidSim PUBLIC UNIX)
else()
  message(FATAL_ERROR "Unknown operating system type: ${CMAKE_SYSTEM_NAME}")
endif()
