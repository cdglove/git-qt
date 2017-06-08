// *****************************************************************************
// 
// cppgit/parse/lfs/lock_status.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "parse/parse.hpp"
#include "parse/parse_common.hpp"
#include "cppgit/result/lfs/lock_status.hpp"
#include "ciere/json/io.hpp"
#include "ciere/json/value.hpp"
#include "ciere/json/parser/grammar.hpp"
#include "ciere/json/parser/grammar_def.hpp"

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse { namespace lfs 
{
    void lock_status(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::ls_files& result)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        auto range = extract_iterators(result_buffer); 
        buffer_iterator current = range.begin();

        boost::spirit::ascii::space_type space;
        ciere::json::parser::grammar<buffer_iterator> grammar; grammar;
        ciere::json::value value;

        bool r = boost::spirit::qi::phrase_parse( current, range.end(), grammar, space, value ); 

        if (!r)
            throw exception("Failed to parse list result", std::string(range.begin(), range.end()));
    }
}}}
