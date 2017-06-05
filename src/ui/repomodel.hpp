// *****************************************************************************
//
// ui/repotreemodel.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#pragma once
#ifndef UI_REPOTREEMODEL_HPP_
#define UI_REPOTREEMODEL_HPP_

#include <QAbstractItemModel>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {
    class file_list;
}}

// -----------------------------------------------------------------------------
//
class RepoTreeModel : public QAbstractItemModel
{
public:

    RepoTreeModel(cppgit::result::file_list const& files);

protected:


    QModelIndex index(int,int,const QModelIndex &) const override;
    QModelIndex parent(const QModelIndex &) const override;
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &,int) const override;

private:

    class TreeItem;
    TreeItem* root_;
};

#endif // UI_REPOTREEMODEL_HPP_