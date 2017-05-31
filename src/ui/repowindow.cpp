// *****************************************************************************
//
// ui/repowindow.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#include "repowindow.hpp"
#include "repotreemodel.hpp"
#include "ui_repowindow.h"
#include "daily/future/future.hpp"
#include "util/async.hpp"
#include "cppgit/result/list_files.hpp"

// -----------------------------------------------------------------------------
//
RepoWindow::RepoWindow(cppgit::repository repo, QWidget *parent)
	: QWidget(parent)
	, ui_(std::make_unique<Ui::RepoWindow>())
    , repo_(std::move(repo))
{
    ui_->setupUi(this);

    daily::future<cppgit::result::list_files> list = repo_.list_files(get_worker_io_service());
    list.then(daily::execute::dispatch, get_worker_executor(), 
        [this](cppgit::result::list_files files)
        {
            return new RepoTreeModel(std::move(files), this);
        }
    ).then(daily::execute::dispatch, get_ui_executor(),
        [this](RepoTreeModel* tree_model)
        {
            ui_->treeView->setModel(tree_model);
        }
    );
}

RepoWindow::~RepoWindow()
{}
