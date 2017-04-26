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
#include <boost/utility/string_view.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/process.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit {

repository::repository(boost::string_view path)
    : path_(path.data())
{
    boost::filesystem::path repo_path(path_);
    repo_path /= ".git";
    if(!boost::filesystem::exists(repo_path))
        throw invalid_repository_exception();

    if(!boost::filesystem::is_directory(repo_path))
        throw invalid_repository_exception();
}

void repository::list_files()
{
    namespace bp = boost::process;
    bp::ipstream is;

    std::vector<std::string> outline;

    //we just use the same pipe, so the 
    bp::child git(bp::search_path(get_git_command().data()), "ls-files",  bp::std_out > is);
   
    std::string line;
    while (git.running() && std::getline(is, line)) //when nm finished the pipe closes and c++filt exits
        outline.push_back(line);

    git.wait();
}

}