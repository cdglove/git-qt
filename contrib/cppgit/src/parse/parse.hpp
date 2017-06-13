// *****************************************************************************
// 
// cppgit/parse/parse.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_PARSE_PARSE_HPP_
#define CPPGIT_PARSE_PARSE_HPP_

#include <boost/range/iterator_range_core.hpp>
#include <boost/asio/basic_streambuf_fwd.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {

    class ls_files;

}}

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse
{
    struct exception : std::runtime_error
    {
        exception(char const* msg, std::string response)
            : std::runtime_error(msg)
            , response(std::move(response))
        {}

        std::string response;
    };

    void ls_files(boost::asio::basic_streambuf<> const& result_buffer, result::ls_files& result);
}}

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result { namespace lfs {

    class ls_files;
    class locks;
    class lock;
    class unlock;

}}}

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse { namespace lfs
{
    void ls_files(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::ls_files& result);
    void locks(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::locks& result);
    void lock(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::lock& result);
    void unlock(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::unlock& result);
}}}


#endif // CPPGIT_PARSE_LISTFILES_HPP_