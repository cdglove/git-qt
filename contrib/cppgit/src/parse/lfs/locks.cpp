// *****************************************************************************
// 
// cppgit/parse/lfs/locks.cpp
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
#include "cppgit/result/lfs/locks.hpp"
#include "ciere/json/io.hpp"
#include "ciere/json/value.hpp"
#include "ciere/json/parser/grammar.hpp"
#include "ciere/json/parser/grammar_def.hpp"

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse { namespace lfs 
{
    void locks(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::locks& result)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        auto range = extract_iterators(result_buffer); 
        buffer_iterator current = range.begin();

        boost::spirit::ascii::space_type space;
        ciere::json::parser::grammar<buffer_iterator> grammar; grammar;
        ciere::json::value value;

        bool r = boost::spirit::qi::phrase_parse( current, range.end(), grammar, space, value ); 

        if (!r)
            throw exception("Failed to parse lock status result", std::string(range.begin(), range.end()));

        std::for_each(value.begin_array(), value.end_array(),
            [&result](ciere::json::value const& json_lock)
            {
                result::lfs::locks::status lock;
                lock.id = json_lock["id"].get_as<int>();
                lock.file = json_lock["path"].get_as<std::string>();
                lock.locked_at = json_lock["locked_at"].get_as<std::string>();
                lock.holder = json_lock["owner"]["name"].get_as<std::string>();
                result.files.push_back(std::move(lock));
            }
        );
    }
}}}
