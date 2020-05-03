#pragma once

#ifndef COVIDSIM_PERF_H_
#    define COVIDSIM_PERF_H_

#    include <cstdint>
#    include <initializer_list>
#    include <sstream>
#    include <string>

/// \fn PerfInit
/// \brief Initializates the performance profiler
void
PerfInit(int*, char***);

/// \fn PerfFinalize
/// \brief Finalizes the performance profiler
void
PerfFinalize();

/// \fn PerfRegionStart
/// \brief Starts a performance region by a string id
void
PerfRegionStart(const std::string&);

/// \fn PerfRegionStop
/// \brief Stops a performance region by a string id
void
PerfRegionStop(const std::string&);

/// \fn PerfStart
/// \brief Starts a performance region by a string id and returns an integer
/// which is used to stop it
uint64_t
PerfStart(const std::string&);

/// \fn PerfStop
/// \brief Stops a performance region via an index provided by \ref PerfStart
void PerfStop(uint64_t);

//--------------------------------------------------------------------------------------//
/// \fn PerfConsumeParameters
/// \brief Initializates the performance profiler
template <typename... Args>
void
PerfConsumeParameters(Args&&...)
{}

//--------------------------------------------------------------------------------------//
/// \macro FOLD_EXPRESSION
/// \brief This is a pre-C++17 technique for doing operations on a variadic list
///
#    if !defined(FOLD_EXPRESSION)
#        define FOLD_EXPRESSION(...)                                                     \
            PerfConsumeParameters(::std::initializer_list<int>{ (__VA_ARGS__, 0)... })
#    endif

//--------------------------------------------------------------------------------------//
/// \fn PerfJoin
/// \brief This is a variadic function for converting a bunch of arguments into a single
/// string
///
/// \code
///  std::string example = PerfJoin("input_", 10, "_region_", 20);
///  // example == "input_10_region_20"
///
template <typename... Args>
std::string
PerfJoin(Args&&... args)
{
    std::stringstream ss;
    FOLD_EXPRESSION(ss << args);
    return ss.str();
}

//--------------------------------------------------------------------------------------//
/// \class PerfRegion
/// \brief A RAII (runtime-acquisition-is-initialization) object which uses scoping rules
/// to start and stop a region via \ref PerfRegionStart and \ref PerfRegionStop
struct PerfRegion
{
    template <typename... Args>
    PerfRegion(Args&&... args)
    : m_tag(PerfJoin(std::forward<Args>(args)...))
    {
        PerfRegionStart(m_tag);
    }

    ~PerfRegion() { PerfRegionStop(m_tag); }

private:
    std::string m_tag;
};

//--------------------------------------------------------------------------------------//
/// \class PerfRecord
/// \brief A RAII (runtime-acquisition-is-initialization) object which uses scoping rules
/// to start and stop a region via \ref PerfStart and \ref PerfStop
struct PerfRecord
{
    template <typename... Args>
    PerfRecord(Args&&... args)
    : m_idx(PerfStart(PerfJoin(std::forward<Args>(args)...)))
    {}

    ~PerfRecord() { PerfStop(m_idx); }

private:
    uint64_t m_idx;
};

#    define PERF_INIT(...) PerfInit(__VA_ARGS__)
#    define PERF_FINALIZE() PerfFinalize()

#    if !defined(COVIDSIM_USE_TIMEMORY)
#        define PERF_REGION(...)
#        define PERF_RECORD(...)
#        define PERF_FUNCTION(...)
#    endif

//--------------------------------------------------------------------------------------//
// clang-format off
#    if !defined(PERF_FUNCTION)
#        define PERF_FUNCTION(...) PerfRecord perf_region_var__##__LINE__(__FUNCTION__, __VA_ARGS__)
#    endif
//
#    if !defined(PERF_REGION)
#        define PERF_REGION(...) PerfRegion perf_region_var__##__LINE__(__VA_ARGS__)
#    endif
//
#    if !defined(PERF_RECORD)
#        define PERF_RECORD(...) PerfRecord perf_record_var__##__LINE__(__VA_ARGS__)
#    endif
// clang-format on
//--------------------------------------------------------------------------------------//

#endif  // COVIDSIM_PERF_H_
