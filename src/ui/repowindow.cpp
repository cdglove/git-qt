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
    QModelIndex idx = ui_->treeView->indexAt(pos);
    tree_model_->create_context_menu(idx);
}

bool RepoWindow::eventFilter(QObject *target, QEvent *event)
{
    //if (target == ui_->treeView)
    //{
    //    if(event->type() == QEvent::ContextMenu)
    //    {
    //        QContextMenuEvent* m = static_cast<QContextMenuEvent*>(event);
    //        //Create context menu here
    //        return true;
    //    }
    //}
    return false;
}