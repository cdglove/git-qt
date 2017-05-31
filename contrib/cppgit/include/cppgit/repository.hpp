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
namespace cppgit {

struct invalid_repository_exception
{};

namespace result {
    class list_files;
}

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

    daily::future<result::list_files> list_files(boost::asio::io_service& ios);

private:

    template<typename>
    struct git_cmd;

    template<typename Result, typename Handler>
    std::shared_ptr<git_cmd<Result>> issue_git_cmd(boost::asio::io_service& ios, boost::string_view cmd, Result&& seed_result, Handler&& h);
    std::wstring path_;
};

}
#endif // CPPGIT_REPOSITORY_HPP_