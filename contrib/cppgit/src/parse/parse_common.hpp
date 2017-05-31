// *****************************************************************************
// 
// cppgit/parse/parse_common.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef CPPGIT_PARSE_PARSECOMMON_HPP_
#define CPPGIT_PARSE_PARSECOMMON_HPP_

#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse
{
    typedef boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> buffer_iterator;

    inline boost::iterator_range<buffer_iterator> extract_iterators(boost::asio::streambuf const& result_buffer)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        return { boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs) };
    }
}}


#endif // CPPGIT_PARSE_PARSECOMMON_HPP_