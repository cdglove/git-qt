// *****************************************************************************
//
// ui/mainwindow.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "ui/mainwindow.hpp"
#include "ui/repowindow.hpp"
#include "util/async.hpp"
#include "ui_mainwindow.h"

#include <boost/utility/string_view.hpp>

#include <QFileDialog>
#include <QMessageBox>

// -----------------------------------------------------------------------------
//
MainWindow::MainWindow()
    : QMainWindow()
    , ui(std::make_unique<Ui::MainWindow>())
{
    start_worker_io_service();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    stop_worker_io_service();
}

void MainWindow::openRepo()
{
    QFileDialog open(this);
    open.setFileMode(QFileDialog::FileMode::Directory);
    while(open.exec())
    {
        QDir selected_dir = open.directory();
        
        // cglover-todo: Check is this is a git repo
        bool is_git_repo = true;
        if(!is_git_repo)
        {
            QMessageBox::critical(this, tr("Not a Git Repository"), selected_dir.path() + tr(" is not a git repository."));
            continue;
        }

        openNewRepo(selected_dir);
        break;
    }   
}

bool MainWindow::event(QEvent *event)
{
    get_ui_io_service().reset();
    get_ui_io_service().run();
    return QMainWindow::event(event);
}
 
void MainWindow::openNewRepo(QDir const& where)
{
    QWidget* tab = new QWidget();
    RepoWindow* repo = new RepoWindow(cppgit::repository(where.absolutePath().toStdString()), tab);
    repo->setGeometry(QRect(10, 10, 781, 521));
    int new_idx = ui->tabWidget->addTab(tab, QString());
    ui->tabWidget->setTabText(new_idx, where.path());
    ui->tabWidget->setCurrentIndex(new_idx);
}
