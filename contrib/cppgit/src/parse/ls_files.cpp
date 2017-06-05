// *****************************************************************************
// 
// cppgit/parse/file_list.hpp
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
#include "cppgit/result/ls_files.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/filesystem/path.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse 
{
    void ls_files(boost::asio::basic_streambuf<> const& result_buffer, result::ls_files& result)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        auto range = extract_iterators(result_buffer); 
        buffer_iterator current = range.begin();

        namespace x3 = boost::spirit::x3;
        using x3::char_;
        using x3::eol;

        auto file = +(char_ - eol);

        bool r = x3::parse(current, range.end(),
            //  Begin grammar
            (
                (file) >> *(eol >> (file))
            )
            ,
            //  End grammar
            result.files
        );

        if (!r)
            throw exception("Failed to parse list result", std::string(range.begin(), range.end()));
    }
}}
