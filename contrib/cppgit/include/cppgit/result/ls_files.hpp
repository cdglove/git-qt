// *****************************************************************************
// 
// cppgit/result/ls_files.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_RESULT_LSFILES_HPP_
#define CPPGIT_RESULT_LSFILES_HPP_

#include <vector>
#include <string>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {

class ls_files
{
public:
    std::vector<std::string> files;
};

}}

#endif // CPPGIT_RESULT_LSFILES_HPP_