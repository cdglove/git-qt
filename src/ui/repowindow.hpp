// *****************************************************************************
//
// ui/repowindow.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef UI_REPOWINDOW_HPP_
#define UI_REPOWINDOW_HPP_

#include <QWidget>
#include <memory>
#include "cppgit/repository.hpp"

// -----------------------------------------------------------------------------
//
namespace Ui {
class RepoWindow;
}

// -----------------------------------------------------------------------------
//
class RepoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RepoWindow(cppgit::repository repo, QWidget* parent);
    ~RepoWindow();

private:
    std::unique_ptr<Ui::RepoWindow> ui_;
    cppgit::repository repo_;
    
};

#endif // UI_REPOWINDOW_HPP_
