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
#include "ui_repowindow.h"

// -----------------------------------------------------------------------------
RepoWindow::RepoWindow(cppgit::repository repo, QWidget *parent)
	: QWidget(parent)
	, ui_(std::make_unique<Ui::RepoWindow>())
    , repo_(std::move(repo))
{
    ui_->setupUi(this);

    repo.list_files();
}

RepoWindow::~RepoWindow()
{}
