// *****************************************************************************
// 
// cppgit/result/lock_status.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_RESULT_LFS_LOCK_STATUS_HPP_
#define CPPGIT_RESULT_LFS_LOCK_STATUS_HPP_

#include <vector>
#include <string>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result { namespace lfs {

class lock_status
{
public:

    struct status
    {
        std::string file;
        std::string locked_by;

        enum 
        {
            unknown,
            unlocked,
            locked,
            held,
        };

    };

    std::vector<status> files;
};

}}}

#endif // CPPGIT_RESULT_LFS_LOCK_STATUS_HPP_