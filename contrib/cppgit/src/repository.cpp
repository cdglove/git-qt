// *****************************************************************************
// 
// cppgit/repository.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#include "cppgit/repository.hpp"
#include "cppgit/command.hpp"
#include "daily/future/future.hpp"
#include "parse/parse.hpp"
#include <boost/utility/string_view.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/process.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <thread>
#include <sstream>
#include <iostream>

#if BOOST_PLAT_WINDOWS_DESKTOP
#  include <boost/process/windows.hpp>
#endif

// -----------------------------------------------------------------------------
//
namespace {
    
    struct git_cmd_base
    {
        git_cmd_base(boost::asio::io_service& ios, boost::string_view cmd, std::wstring const& working_dir)
            : result_pipe(ios)
            , child(  boost::process::search_path(cppgit::get_git_command().data())
                    , boost::process::args = cmd.data()
                    , boost::process::std_out = result_pipe
                    , boost::process::start_dir = working_dir.data()
                #if BOOST_PLAT_WINDOWS_DESKTOP
                    , boost::process::windows::hide
                #endif
            )
        {}

        boost::asio::streambuf result_buffer;
        boost::process::async_pipe result_pipe;
        boost::process::child child;
    };    
}

// -----------------------------------------------------------------------------
//
namespace cppgit {

// -----------------------------------------------------------------------------
//
template<typename Result>
struct repository::git_cmd : git_cmd_base
{
    using git_cmd_base::git_cmd_base;
    daily::promise<Result> promise;
};

// -----------------------------------------------------------------------------
//
repository::repository(boost::string_view path)
{
    boost::filesystem::path repo_path(path.data());
    repo_path /= ".git";
    if(!boost::filesystem::exists(repo_path))
        throw invalid_repository_exception();

    if(!boost::filesystem::is_directory(repo_path))
        throw invalid_repository_exception();

    path_ = repo_path.native();
}

// -----------------------------------------------------------------------------
//
repository::~repository() = default;
repository::repository(repository&&) = default;
repository& repository::operator=(repository&&) = default;

// -----------------------------------------------------------------------------
//
daily::future<result::list_files> repository::list_files(boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, "ls-files", result::list_files(),
        [](git_cmd<result::list_files>& cmd, result::list_files r, boost::system::error_code const& ec, std::size_t size)
        { 
            //if(!ec)
            {

                parse::list_files_result(cmd.result_buffer, r);
                cmd.promise.set_value(r);
            }
        }
    );

    return cmd->promise.get_future();
}

template<typename Result, typename Handler>
std::shared_ptr<
    repository::git_cmd<Result>
> repository::issue_git_cmd(boost::asio::io_service& ios, 
                            boost::string_view params, 
                            Result&& seed_result, 
                            Handler&& h)
{
    auto cmd = std::make_shared<git_cmd<Result>>(ios, params, path_);
    boost::asio::async_read(
        cmd->result_pipe, 
        cmd->result_buffer,
        [cmd, h = std::forward<Handler>(h), r = std::forward<Result>(seed_result)](const boost::system::error_code &ec, std::size_t size)
        { 
            h(*cmd, std::move(r), ec, size);
        }
    );

    return cmd;
}

} // nnamespace cppgit