// *****************************************************************************
// 
// cppgit/repository.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_REPOSITORY_HPP_
#define CPPGIT_REPOSITORY_HPP_

#include <boost/utility/string_view_fwd.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <memory>
#include <vector>

// -----------------------------------------------------------------------------
//
namespace boost { namespace asio {

    class io_service;
}}

// -----------------------------------------------------------------------------
//
namespace daily { 

    template<typename>
    class future;
}

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace exception {

struct invalid_repository
{};

struct bad_git_cmd
{};

} // exception {

namespace result {
    class ls_files;
}

namespace result { namespace lfs {
    class ls_files;
    class locks;
    class lock;
    class unlock;
}}

// -----------------------------------------------------------------------------
//
class repository
{
public:

    repository(boost::string_view path);
    ~repository();

    // No copying.
    repository(repository const&) = delete;
    repository& operator=(repository const&) = delete;

    // Move OK.
    repository(repository&&);
    repository& operator=(repository&&);

    daily::future<result::ls_files> get_file_list(
        boost::asio::io_service& ios);

    daily::future<result::lfs::ls_files> get_lfs_file_list(
        boost::asio::io_service& ios);

    daily::future<result::lfs::locks> get_lfs_locks(
        boost::asio::io_service& ios);

    daily::future<result::lfs::lock> lock_file(
        boost::string_view path,
        boost::asio::io_service& ios);

    daily::future<result::lfs::unlock> unlock_file(
        boost::string_view path,
        boost::asio::io_service& ios);
private:

    template<typename>
    struct git_cmd;

    template<typename Result, typename Handler>
    std::shared_ptr<git_cmd<Result>> issue_git_cmd(
        boost::asio::io_service& ios, 
        std::initializer_list<char const*> params, 
        Result&& seed_result, 
        Handler&& h);
    
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
#endif // CPPGIT_REPOSITORY_HPP_