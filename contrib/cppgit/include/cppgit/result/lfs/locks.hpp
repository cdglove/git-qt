// *****************************************************************************
// 
// cppgit/result/lfs/locks.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_RESULT_LFS_LOCKS_HPP_
#define CPPGIT_RESULT_LFS_LOCKS_HPP_

#include <vector>
#include <string>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result { namespace lfs {

class locks
{
public:

    struct status
    {
        std::string file;
        std::string holder;
        std::string locked_at;
        std::uint32_t id = 0;
    };

    std::vector<status> files;
};


class lock
{
public:

    std::string file;
    std::string locked_at;
    std::uint32_t id = 0;
    bool success = false;
};

class unlock
{
public:

    unlock(std::string path)
        : file(std::move(path))
    {}

    std::string file;
    bool success = false;
};

}}}

#endif // CPPGIT_RESULT_LFS_LOCKS_HPP_