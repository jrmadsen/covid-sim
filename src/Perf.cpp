
#include "Perf.h"

#if defined(COVIDSIM_USE_TIMEMORY)
#    include <timemory/timemory.hpp>
#endif

#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
#    include <timemory/library.h>
#endif

#include <limits>
#include <unordered_map>

//--------------------------------------------------------------------------------------//

#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
extern "C"
{
    void timemory_register_ompt();
    void timemory_deregister_ompt();
}
#endif

//--------------------------------------------------------------------------------------//

using key_map_t = std::unordered_map<std::string, uint64_t>;

namespace
{
static thread_local uint64_t  counter;
static thread_local key_map_t key_map;
}  // namespace

//--------------------------------------------------------------------------------------//

void
PerfInit(int* argc, char*** argv)
{
#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    timemory_set_default("wall_clock,cpu_clock,cpu_util,peak_rss");
    timemory_init_library(*argc, *argv);
    timemory_register_ompt();
#elif defined(COVIDSIM_USE_TIMEMORY)
    tim::timemory_init(argc, argv);
#else
    PerfConsumeParameters(argc, argv);
#endif
}

//--------------------------------------------------------------------------------------//

void
PerfFinalize()
{
#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    timemory_deregister_ompt();
    timemory_finalize_library();
#elif defined(COVIDSIM_USE_TIMEMORY)
    tim::timemory_finalize();
#endif
}

//--------------------------------------------------------------------------------------//

void
PerfRegionStart(const std::string& key)
{
#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    timemory_push_region(key.c_str());
#elif defined(COVIDSIM_USE_TIMEMORY)
#else
    PerfConsumeParameters(key);
#endif
}

//--------------------------------------------------------------------------------------//

void
PerfRegionStop(const std::string& key)
{
#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    timemory_pop_region(key.c_str());
#elif defined(COVIDSIM_USE_TIMEMORY)
#else
    PerfConsumeParameters(key);
#endif
}

//--------------------------------------------------------------------------------------//

uint64_t
PerfStart(const std::string& key)
{
#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    return timemory_get_begin_record(key.c_str());
#elif defined(COVIDSIM_USE_TIMEMORY)
#else
    PerfConsumeParameters(key);
    return std::numeric_limits<uint64_t>::max();
#endif
}

//--------------------------------------------------------------------------------------//

void
PerfStop(uint64_t idx)
{
    if(idx == std::numeric_limits<uint64_t>::max())
        return;

#if defined(COVIDSIM_USE_TIMEMORY_LIBRARY)
    timemory_end_record(idx);
#elif defined(COVIDSIM_USE_TIMEMORY)

#endif
}

//--------------------------------------------------------------------------------------//
