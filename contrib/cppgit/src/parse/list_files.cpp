// *****************************************************************************
// 
// cppgit/parse/list_files.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "parse.hpp"
#include "parse_common.hpp"
#include <boost/spirit/home/x3.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse
{
    void list_files_result(boost::asio::basic_streambuf<> const& result_buffer, result::list_files& result)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        auto range = extract_iterators(result_buffer); 
        buffer_iterator current = range.begin();

        namespace x3 = boost::spirit::x3;
        using x3::char_;
        using x3::eol;

        bool r = x3::parse(current, range.end(),
            //  Begin grammar
            (
                (+(char_ - eol)) >> *(eol >> (+(char_ - eol)))
            )
            ,
            //  End grammar
            result.file
        );

        if (!r)
            throw exception("Failed to parse list result", std::string(range.begin(), range.end()));
    }
}}
