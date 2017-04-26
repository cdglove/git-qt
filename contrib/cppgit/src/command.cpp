// *****************************************************************************
// 
// cppgit/command.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#include <boost/utility/string_view.hpp>

static char const* default_git_command_ = "git";
static std::string git_command_ = default_git_command_;

// -----------------------------------------------------------------------------
//
namespace cppgit {

boost::string_view get_git_command()
{
    return git_command_;
}

boost::string_view get_default_git_command()
{
    return default_git_command_;
}

void set_git_command(boost::string_view cmd)
{
    git_command_ = cmd.data();
}

};

