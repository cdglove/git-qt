// *****************************************************************************
// 
// test/parsers.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#define BOOST_TEST_MODULE Parsers
#include <boost/test/unit_test.hpp>
#include <boost/asio/streambuf.hpp>
#include "cppgit/result/ls_files.hpp"
#include "cppgit/result/lfs/ls_files.hpp"
//#include "cppgit/result/lfs/ls_files.hpp"
#include "../src/parse/parse.hpp"

BOOST_AUTO_TEST_CASE( parse_ls_files )
{
	char const* test_data = 
		".gitattributes\n"
		"bins/small_0.bin\n"
		"main.cpp\n"
	;

    boost::asio::streambuf buf;
    buf.sputn(test_data, std::strlen(test_data));
    cppgit::result::ls_files result;
    cppgit::parse::ls_files(buf, result);

    BOOST_CHECK(result.files[0] == ".gitattributes");
    BOOST_CHECK(result.files[1] == "bins/small_0.bin");
    BOOST_CHECK(result.files[2] == "main.cpp");
}

BOOST_AUTO_TEST_CASE( parse_lfs_ls_files )
{
	char const* test_data = 
		"912c9549c2d4f8fc9eb8d901a6c3785b709181fd9ef650dfe5853f3068830dca * bins/small_0.bin\n"
		"c31243edb4f0540aba85a280dff05b467d2543cca2755600dcec61bd23eae73d * bins/small_1.bin\n"
		"4cbc90d405f15b73e3148cd521360b3f39ae3466c265ed32c1f14d841263316b - bins/small_2.bin\n"
	;

    boost::asio::streambuf buf;
    buf.sputn(test_data, std::strlen(test_data));
    cppgit::result::lfs::ls_files result;
    cppgit::parse::lfs::ls_files(buf, result);

    BOOST_CHECK(result.files[0].file == "bins/small_0.bin");
    BOOST_CHECK(result.files[0].sha1 == "912c9549c2d4f8fc9eb8d901a6c3785b709181fd9ef650dfe5853f3068830dca");
    BOOST_CHECK(result.files[1].file == "bins/small_1.bin");
    BOOST_CHECK(result.files[1].sha1 == "c31243edb4f0540aba85a280dff05b467d2543cca2755600dcec61bd23eae73d");
    BOOST_CHECK(result.files[2].file == "bins/small_2.bin");
    BOOST_CHECK(result.files[2].sha1 == "4cbc90d405f15b73e3148cd521360b3f39ae3466c265ed32c1f14d841263316b");
}