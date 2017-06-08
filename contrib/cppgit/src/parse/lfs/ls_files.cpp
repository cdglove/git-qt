// *****************************************************************************
// 
// cppgit/parse/ls_files.cpp
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
#include "cppgit/result/lfs/ls_files.hpp"
#include <boost/spirit/home/x3.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    cppgit::result::lfs::ls_files::status,
    sha1,
    file
);

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace parse { namespace lfs 
{
    void ls_files(boost::asio::basic_streambuf<> const& result_buffer, result::lfs::ls_files& result)
    {
        boost::asio::streambuf::const_buffers_type bufs = result_buffer.data();
        auto range = extract_iterators(result_buffer); 
        buffer_iterator current = range.begin();

        namespace x3 = boost::spirit::x3;
        using x3::char_;
        using x3::eol;
        using x3::space;
        using x3::lit;

        //auto inserter = [&result](auto ctx)
        //{
        //    auto&& attrib = _attr(ctx);
        //    result.files.push_back(
        //    {
        //        std::move(boost::fusion::at_c<0>(attrib)),
        //        std::move(boost::fusion::at_c<1>(attrib))
        //    });
        //};

        auto sha1 = x3::repeat(64)[char_ - space];
        auto file = (+(char_ - eol));
        auto line = (sha1 >> lit(' ') >> (lit('*') | lit('-')) >> lit(' ') >> file); //[inserter];

        bool r = x3::parse(current, range.end(),
            //  Begin grammar
            (
                line >> *(eol >> line)
            )
            //  End grammar
            , result.files
        );

        if (!r)
            throw exception("Failed to parse list result", std::string(range.begin(), range.end()));
    }
}}}
