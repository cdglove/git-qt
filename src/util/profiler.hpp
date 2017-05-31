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

std::ostream& get_profiler_ostream();

#define DAILY_AUTO_PROFILE_NODE(name) \
    boost::timer::auto_cpu_timer _timer_scope_##name##__LINE__##( \
        get_profiler_ostream(), \
        #name" %ws wall, %us user + %ss system = %ts CPU (%p%)\n")

#define DAILY_AUTO_PROFILE_NODE_ACCUMULATE(name) \
    static boost::timer::auto_cpu_timer _timer_scope_##name##__LINE__##( \
        get_profiler_ostream(), \
        #name" %ws wall, %us user + %ss system = %ts CPU (%p%)\n") \
    _timer_scope_##name##__LINE__##.resume();

#else

#define DAILY_AUTO_PROFILE_NODE(name)

#endif // DAILY_ENABLE_PROFILER 

#endif // UTIL_PROFILER_HPP_