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

#include "cppgit/result/list_files.hpp"
#include <boost/range/iterator_range_core.hpp>
#include <boost/asio/basic_streambuf_fwd.hpp>

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

    void list_files_result(boost::asio::basic_streambuf<> const& result_buffer, result::list_files& result);
}}


#endif // CPPGIT_PARSE_LISTFILES_HPP_