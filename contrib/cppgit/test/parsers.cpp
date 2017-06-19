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
#include "cppgit/result/lfs/locks.hpp"
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

BOOST_AUTO_TEST_CASE( parse_lfs_locks )
{
	char const* test_data = 
		"["
            R"({"id":"1397","path":"bins/small_0.bin","owner":{"name":"cdglove"},"locked_at":"2017-06-07T02:40:34Z"},)"
            R"({"id":"1401","path":"bins/small_1000.bin","owner":{"name":"cdglove1"},"locked_at":"2017-06-08T02:20:14Z"},)"
            R"({"id":"1402","path":"bins/small space.bin","owner":{"name":"cdg love"},"locked_at":"2017-06-08T02:22:09Z"})"
        "]"
	;

    boost::asio::streambuf buf;
    buf.sputn(test_data, std::strlen(test_data));
    cppgit::result::lfs::locks result;
    cppgit::parse::lfs::locks(buf, result);

    BOOST_CHECK(result.files[0].file == "bins/small_0.bin");
    BOOST_CHECK(result.files[0].id == 1397);
    BOOST_CHECK(result.files[0].locked_at == "2017-06-07T02:40:34Z");
    BOOST_CHECK(result.files[0].holder == "cdglove");
    BOOST_CHECK(result.files[1].file == "bins/small_1000.bin");
    BOOST_CHECK(result.files[1].id == 1401);
    BOOST_CHECK(result.files[1].locked_at == "2017-06-08T02:20:14Z");
    BOOST_CHECK(result.files[1].holder == "cdglove1");
    BOOST_CHECK(result.files[2].file == "bins/small space.bin");
    BOOST_CHECK(result.files[2].id == 1402);
    BOOST_CHECK(result.files[2].locked_at == "2017-06-08T02:22:09Z");
    BOOST_CHECK(result.files[2].holder == "cdg love");
}

BOOST_AUTO_TEST_CASE( parse_lfs_lock )
{
	char const* test_data = 
        R"({"id":"1397","path":"bins/small_0.bin","locked_at":"2017-06-07T02:40:34Z"},)"
	;

    boost::asio::streambuf buf;
    buf.sputn(test_data, std::strlen(test_data));
    cppgit::result::lfs::lock result;
    cppgit::parse::lfs::lock(buf, result);

    BOOST_CHECK(result.file == "bins/small_0.bin");
    BOOST_CHECK(result.id == 1397);
    BOOST_CHECK(result.locked_at == "2017-06-07T02:40:34Z");
}

BOOST_AUTO_TEST_CASE( parse_lfs_unlock )
{
    {
	    char const* test_data = 
            R"({"unlocked":true})"
        ;

        boost::asio::streambuf buf;
        buf.sputn(test_data, std::strlen(test_data));
        cppgit::result::lfs::unlock result("");
        cppgit::parse::lfs::unlock(buf, result);

        BOOST_CHECK(result.success);
    }

    {
        char const* test_data = 
            R"({"unlocked":false})"
        ;

        boost::asio::streambuf buf;
        buf.sputn(test_data, std::strlen(test_data));
        cppgit::result::lfs::unlock result("");
        cppgit::parse::lfs::unlock(buf, result);

        BOOST_CHECK(result.success == false);
    }
}