// *****************************************************************************
// 
// cppgit/result/list_files.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_RESULT_LISTFILES_HPP_
#define CPPGIT_RESULT_LISTFILES_HPP_

#include <vector>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {

class list_files
{
public:
    std::vector<std::string> file;
};

}}

#endif // CPPGIT_RESULT_LISTFILES_HPP_