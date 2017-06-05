// *****************************************************************************
// 
// util/profiler.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef UTIL_PROFILER_HPP_
#define UTIL_PROFILER_HPP_

#ifndef DAILY_ENABLE_PROFILER
#  define DAILY_ENABLE_PROFILER 1
#endif

#if DAILY_ENABLE_PROFILER

#include <boost/timer/timer.hpp>
#include <iosfwd>

namespace daily {

std::ostream& get_profiler_ostream();

struct always_report_cpu_timer : boost::timer::auto_cpu_timer
{
    using boost::timer::auto_cpu_timer::auto_cpu_timer;

    ~always_report_cpu_timer()
    {
        stop();
        report();
    }
};

struct auto_resume_cpu_timer
{
    auto_resume_cpu_timer(boost::timer::cpu_timer& timer)
        : timer_(timer)
    {
        timer_.resume();
    }

    ~auto_resume_cpu_timer()
    {
        timer_.stop();
    }

    boost::timer::cpu_timer& timer_;
};

#define DAILY_AUTO_PROFILE_NODE(name) \
    boost::timer::auto_cpu_timer _timer_scope_##name( \
        daily::get_profiler_ostream(), \
        #name" %ws wall, %us user + %ss system = %ts CPU (%p%)\n")

#define DAILY_DECLARE_STATIC_PROFILE_NODE(name) \
    static daily::always_report_cpu_timer _timer_scope_##name( \
        daily::get_profiler_ostream(), \
        #name" %ws wall, %us user + %ss system = %ts CPU (%p%)\n")

#define DAILY_DECLARE_PROFILE_NODE(name) \
    daily::always_report_cpu_timer _timer_scope_##name( \
        daily::get_profiler_ostream(), \
        #name" %ws wall, %us user + %ss system = %ts CPU (%p%)\n"); \
        _timer_scope_##name.stop()

#define DAILY_START_PROFILE_NODE(name) \
    _timer_scope_##name.start()

#define DAILY_STOP_PROFILE_NODE(name) \
    _timer_scope_##name.stop()

#define DAILY_RESUME_PROFILE_NODE(name) \
    _timer_scope_##name.resume()

#define DAILY_AUTO_RESUME_PROFILE_NODE(name) \
    daily::auto_resume_cpu_timer _resume_timer_scope_##name(_timer_scope_##name)

#define DAILY_STATIC_AUTO_PROFILE_NODE(name) \
    DAILY_DECLARE_STATIC_PROFILE_NODE(name); \
    daily::auto_resume_cpu_timer _resume_timer_scope_##name(_timer_scope_##name)
} 

#else

#define DAILY_AUTO_PROFILE_NODE(name)
#define DAILY_DECLARE_STATIC_PROFILE_NODE(name)
#define DAILY_DECLARE_PROFILE_NODE(name)
#define DAILY_START_PROFILE_NODE(name)
#define DAILY_STOP_PROFILE_NODE(name)
#define DAILY_RESUME_PROFILE_NODE(name)
#define DAILY_AUTO_RESUME_PROFILE_NODE(name)

#endif // DAILY_ENABLE_PROFILER 

#endif // UTIL_PROFILER_HPP_