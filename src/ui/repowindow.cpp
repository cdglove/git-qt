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
#include "cppgit/result/ls_files.hpp"
#include "cppgit/result/lfs/ls_files.hpp"
#include <boost/filesystem/path.hpp>
#include <QEvent>
#include <QContextMenuEvent>
#include <QMenu>

// -----------------------------------------------------------------------------
//
RepoWindow::RepoWindow(cppgit::repository repo, QWidget *parent)
	: QWidget(parent)
	, ui_(std::make_unique<Ui::RepoWindow>())
    , repo_(std::move(repo))
{
    ui_->setupUi(this);

    file_context_menu_.addAction(ui_->actionLockFile);
    file_context_menu_.addAction(ui_->actionUnlockFile);
    file_context_menu_.addAction(ui_->actionPullFile);
    file_context_menu_.addAction(ui_->actionDeleteLocalFile);

    repo_.get_file_list(get_worker_io_service()
    ).then(daily::execute::dispatch, get_worker_executor(), 
        [this](cppgit::result::ls_files files)
        {
            tree_model_ = std::make_unique<RepoTreeModel>(files, this);
        }
    ).then(daily::execute::dispatch, get_ui_executor(),
        [this]
        {
            ui_->treeView->setModel(tree_model_.get()); 
            repo_.get_lfs_file_list(get_worker_io_service()).then(daily::execute::dispatch, get_worker_executor(),
                [this](cppgit::result::lfs::ls_files files)
                {
                    tree_model_->set_lfs_files(files);
                }
            );
        }
    );
}

RepoWindow::~RepoWindow()
{}

void RepoWindow::showCustomContextMenu(QPoint const& pos)
{
    enableAllFileActions();
    QModelIndex idx = ui_->treeView->indexAt(pos);
    if(tree_model_->is_file(idx))
    {
        context_idx_ = idx;
        file_context_menu_.popup(ui_->treeView->viewport()->mapToGlobal(pos));
    }
}

void RepoWindow::deleteLocalFile()
{
    boost::filesystem::path file = tree_model_->get_path(context_idx_);
}

void RepoWindow::lockFile()
{
    boost::filesystem::path file = tree_model_->get_path(context_idx_);
}

void RepoWindow::unlockFile()
{
    boost::filesystem::path file = tree_model_->get_path(context_idx_);
}

void RepoWindow::pullFile()
{
    boost::filesystem::path file = tree_model_->get_path(context_idx_);
}

void RepoWindow::enableAllFileActions()
{
    ui_->actionLockFile->setEnabled(true);
    ui_->actionUnlockFile->setEnabled(true);
    ui_->actionPullFile->setEnabled(true);
    ui_->actionDeleteLocalFile->setEnabled(true);
}
