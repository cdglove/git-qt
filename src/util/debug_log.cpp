// *****************************************************************************
//
// util/debug_log.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "util/debug_log.hpp"
#include <fstream>

struct log_holder
{
	log_holder()
		: fout_("debug.log")
	{}

	std::ofstream fout_;
};

std::ostream& log()
{
	static log_holder log;
	return log.fout_;
}