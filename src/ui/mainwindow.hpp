// *****************************************************************************
//
// ui/mainwindow.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef UI_MAINWINDOW_HPP_
#define UI_MAINWINDOW_HPP_

#include <QMainWindow>
#include <QFutureWatcher>
#include <memory>
#include "async_ui_task.hpp"

// -----------------------------------------------------------------------------
//
namespace Ui {
class MainWindow;
}

class QTreeWidgetItem;

// -----------------------------------------------------------------------------
//
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();
    ~MainWindow();

private slots:

    void filterTextChanged(QString const& filter_text);
    void lfsTreeItemChanged(QTreeWidgetItem* item, int column);
    void openRepo();

private:

    void openNewRepo(QDir const& where);

    std::unique_ptr<Ui::MainWindow> ui;
};

#endif // UI_DIALOG_HPP_
