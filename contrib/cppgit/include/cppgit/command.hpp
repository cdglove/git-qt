// *****************************************************************************
// 
// cppgit/command.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_COMMAND_HPP_
#define CPPGIT_COMMAND_HPP_

#include <boost/utility/string_view_fwd.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit {

//! Returns the default command used to run git. This is typically just
//! 'git', which is expected to be in the users path.
boost::string_view get_default_git_command();

//! Returns the current git command.
boost::string_view get_git_command();

//! Override the git command. This can be either a fully qualifed path to an 
//! executable, of an exacutable in the user's path.
void set_git_command(boost::string_view cmd);

};


#endif // CPPGIT_COMMAND_HPP_
