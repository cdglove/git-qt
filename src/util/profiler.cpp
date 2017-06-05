// *****************************************************************************
// 
// main.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "profiler.hpp"

#if DAILY_ENABLE_PROFILER

#include <fstream>

std::ostream& daily::get_profiler_ostream()
{
    static std::ofstream profout("profile.log");
    return profout;
}

#endif
