/*!
    \file phase_metrics.cpp
    \brief Benchmark phase metrics implementation
    \author Ivan Shynkarenka
    \date 03.07.2015
    \copyright MIT License
*/

#include "benchmark/phase_metrics.h"

#include "benchmark/system.h"

namespace CppBenchmark {

int64_t PhaseMetrics::avg_time() const noexcept
{
    return (_total_iterations > 0) ? (_total_time / _total_iterations) : 0;
}

int64_t PhaseMetrics::min_time() const noexcept
{
    return (_total_iterations > 0) ? (_min_time / _total_iterations) : _min_time;
}

int64_t PhaseMetrics::max_time() const noexcept
{
    return (_total_iterations > 0) ? (_max_time / _total_iterations) : _max_time;
}

int64_t PhaseMetrics::iterations_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    if (_total_iterations < 1000000000)
        return (_total_iterations * 1000000000) / _total_time;
    else
        return (_total_iterations / _total_time) * 1000000000;
}

int64_t PhaseMetrics::items_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    if (_total_items < 1000000000)
        return (_total_items * 1000000000) / _total_time;
    else
        return (_total_items / _total_time) * 1000000000;
}

int64_t PhaseMetrics::bytes_per_second() const noexcept
{
    if (_total_time <= 0)
        return 0;

    if (_total_bytes < 1000000000)
        return (_total_bytes * 1000000000) / _total_time;
    else
        return (_total_bytes / _total_time) * 1000000000;
}

void PhaseMetrics::StartCollecting() noexcept
{
    _iterstamp = _total_iterations;
    _timestamp = System::Timestamp();
}

void PhaseMetrics::StopCollecting() noexcept
{
    // Get iterations count & duration
    int64_t total_duration = System::Timestamp() - _timestamp;

    // Update time counters
    if (total_duration < _min_time)
        _min_time = total_duration;
    if (total_duration > _max_time)
        _max_time = total_duration;
    _total_time += total_duration;
}

void PhaseMetrics::MergeMetrics(const PhaseMetrics& metrics)
{
    // Choose best min time
    if (metrics._min_time < _min_time)
        _min_time = metrics._min_time;

    // Choose best max time
    if (metrics._max_time > _max_time)
        _max_time = metrics._max_time;

    // Merge custom hash tables
    _custom_int.insert(metrics._custom_int.begin(), metrics._custom_int.end());
    _custom_uint.insert(metrics._custom_uint.begin(), metrics._custom_uint.end());
    _custom_int64.insert(metrics._custom_int64.begin(), metrics._custom_int64.end());
    _custom_uint64.insert(metrics._custom_uint64.begin(), metrics._custom_uint64.end());
    _custom_flt.insert(metrics._custom_flt.begin(), metrics._custom_flt.end());
    _custom_dbl.insert(metrics._custom_dbl.begin(), metrics._custom_dbl.end());
    _custom_str.insert(metrics._custom_str.begin(), metrics._custom_str.end());

    // Choose best total time with iterations, items and bytes
    if (metrics._total_time < _total_time)
    {
        _total_time = metrics._total_time;
        _total_iterations = metrics._total_iterations;
        _total_items = metrics._total_items;
        _total_bytes = metrics._total_bytes;
        // Overwrite metrics custom tables
        for (auto& it : metrics._custom_int)
            _custom_int[it.first] = it.second;
        for (auto& it : metrics._custom_uint)
            _custom_uint[it.first] = it.second;
        for (auto& it : metrics._custom_int64)
            _custom_int64[it.first] = it.second;
        for (auto& it : metrics._custom_uint64)
            _custom_uint64[it.first] = it.second;
        for (auto& it : metrics._custom_flt)
            _custom_flt[it.first] = it.second;
        for (auto& it : metrics._custom_dbl)
            _custom_dbl[it.first] = it.second;
        for (auto& it : metrics._custom_str)
            _custom_str[it.first] = it.second;
    }
}

} // namespace CppBenchmark
