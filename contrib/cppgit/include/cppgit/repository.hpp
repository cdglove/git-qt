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

// -----------------------------------------------------------------------------
//
namespace cppgit {

struct invalid_repository_exception
{};

class repository
{
public:

    repository(boost::string_view path);

    // No copying.
    repository(repository const&) = delete;
    repository& operator=(repository const&) = delete;

    // Move OK.
    repository(repository&&) = default;
    repository& operator=(repository&&) = default;

    void list_files();

private:

    std::string path_;
};

}
#endif // CPPGIT_REPOSITORY_HPP_