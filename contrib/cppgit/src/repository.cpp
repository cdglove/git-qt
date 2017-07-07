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
#include "parse/parse.hpp"
#include "cppgit/result/ls_files.hpp"
#include "cppgit/result/lfs/ls_files.hpp"
#include "cppgit/result/lfs/locks.hpp"

#include "daily/future/future.hpp"

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
        git_cmd_base(
            boost::asio::io_service& ios, 
            std::initializer_list<char const*> params, 
            boost::filesystem::path const& working_dir)
          : result_pipe(ios)
          , child(    boost::process::search_path(cppgit::get_git_command().data())
                    , boost::process::args = std::move(params)
                    , boost::process::std_out = result_pipe
                    , boost::process::std_err = result_pipe
                    , boost::process::start_dir = working_dir.c_str()
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
struct repository::impl
{
    boost::filesystem::path path;
};

// -----------------------------------------------------------------------------
//
repository::repository(boost::string_view path)
    : impl_(std::make_unique<impl>())
{
    boost::filesystem::path repo_path(path.data());
    if(!boost::filesystem::exists(repo_path))
        throw exception::invalid_repository();

    if(!boost::filesystem::is_directory(repo_path))
        throw exception::invalid_repository();

    impl_->path = std::move(repo_path);
}

// -----------------------------------------------------------------------------
//
repository::~repository() = default;
repository::repository(repository&&) = default;
repository& repository::operator=(repository&&) = default;

// -----------------------------------------------------------------------------
//
daily::future<result::ls_files> repository::get_file_list(boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, {"ls-fles"}, result::ls_files(),
        [](git_cmd<result::ls_files>& cmd, result::ls_files r, boost::system::error_code const& ec, std::size_t size)
        {
            if(cmd.child.exit_code())
                throw exception::bad_git_cmd();

            if(ec && ec != boost::asio::error::broken_pipe)
                throw exception::bad_git_cmd();

            parse::ls_files(cmd.result_buffer, r);
            cmd.promise.set_value(r);
        }
    );

    return cmd->promise.get_future();
}

daily::future<result::lfs::ls_files> repository::get_lfs_file_list(boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, {"lfs", "ls-files", "--long"}, result::lfs::ls_files(), 
        [](git_cmd<result::lfs::ls_files>& cmd, result::lfs::ls_files r, boost::system::error_code const& ec, std::size_t size)
        {
           if(cmd.child.exit_code())
                throw exception::bad_git_cmd();

            if(ec && ec != boost::asio::error::broken_pipe)
                throw exception::bad_git_cmd();

            parse::lfs::ls_files(cmd.result_buffer, r);
            cmd.promise.set_value(r);
        }
    );

    return cmd->promise.get_future();
}

daily::future<result::lfs::locks> repository::get_lfs_locks(boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, {"lfs", "locks", "--json"}, result::lfs::locks(), 
        [](git_cmd<result::lfs::locks>& cmd, result::lfs::locks r, boost::system::error_code const& ec, std::size_t size)
        {
           if(cmd.child.exit_code())
                throw exception::bad_git_cmd();

            if(ec && ec != boost::asio::error::broken_pipe)
                throw exception::bad_git_cmd();

            parse::lfs::locks(cmd.result_buffer, r);
            cmd.promise.set_value(r);
        }
    );

    return cmd->promise.get_future();
}

daily::future<result::lfs::lock> repository::lock_file(boost::string_view path, boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, {"lfs", "lock", path.data(), "--json"}, result::lfs::lock(), 
        [](git_cmd<result::lfs::lock>& cmd, result::lfs::lock r, boost::system::error_code const& ec, std::size_t size)
        {
            if(cmd.child.exit_code())
                throw exception::bad_git_cmd();

            if(ec && ec != boost::asio::error::broken_pipe)
                throw exception::bad_git_cmd();
            
            parse::lfs::lock(cmd.result_buffer, r);
            cmd.promise.set_value(r);
        }
    );

    return cmd->promise.get_future();
}

daily::future<result::lfs::unlock> repository::unlock_file(boost::string_view path, boost::asio::io_service& ios)
{
    auto cmd = issue_git_cmd(ios, {"lfs", "unlock", path.data(), "--json"}, result::lfs::unlock(path.data()), 
        [](git_cmd<result::lfs::unlock>& cmd, result::lfs::unlock r, boost::system::error_code const& ec, std::size_t size)
        {
            if(cmd.child.exit_code())
                throw exception::bad_git_cmd();

            if(ec && ec != boost::asio::error::broken_pipe)
                throw exception::bad_git_cmd();
            
            parse::lfs::unlock(cmd.result_buffer, r);
            cmd.promise.set_value(r);
        }
    );

    return cmd->promise.get_future();
}

template<typename Result, typename Handler>
std::shared_ptr<
    repository::git_cmd<Result>
> repository::issue_git_cmd(boost::asio::io_service& ios, 
                            std::initializer_list<char const*> params, 
                            Result&& seed_result, 
                            Handler&& h)
{
    auto cmd = std::make_shared<git_cmd<Result>>(ios, params, impl_->path);
    boost::asio::async_read(
        cmd->result_pipe, 
        cmd->result_buffer,
        boost::asio::transfer_all(),
        [cmd, h = std::forward<Handler>(h), r = std::forward<Result>(seed_result)](const boost::system::error_code &ec, std::size_t size)
        { 
            // So we know the status code is updated.
            cmd->child.wait();
            try
            {
                h(*cmd, std::move(r), ec, size);
            }
            catch(...)
            {
                cmd->promise.set_exception(std::current_exception());     
            }
        }
    );

    return cmd;
}

} // nnamespace cppgit