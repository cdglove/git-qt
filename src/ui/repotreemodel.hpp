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
#include <memory>
#include <boost/utility/string_view_fwd.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {
    class ls_files;
}}

namespace cppgit { namespace result { namespace lfs {
    class ls_files;
    class locks;
    class lock;
    class unlock;
}}}

class QMenu;

// -----------------------------------------------------------------------------
//
class RepoTreeModel : public QAbstractItemModel
{
public:

    RepoTreeModel(cppgit::result::ls_files const& files, QObject *parent);
    ~RepoTreeModel();

    void set_lfs_files(cppgit::result::lfs::ls_files const& files);
    void update_lock_status(cppgit::result::lfs::locks const& files);
    void update_lock_status(cppgit::result::lfs::lock const& file);
    void update_lock_status(boost::string_view path, cppgit::result::lfs::unlock const& file);
    
    bool is_file(QModelIndex const& idx);
    boost::string_view get_path_view(QModelIndex const& idx);

protected:
    
    QVariant data(QModelIndex const& index, int role) const override;
    Qt::ItemFlags flags(QModelIndex const& index) const override;

    QVariant headerData(
        int section, 
        Qt::Orientation orientation, 
        int role = Qt::DisplayRole) const override;

    QModelIndex index(
        int row,
        int column,
        QModelIndex const& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:

    class TreeItem;
    struct Impl;

    void create_tree(cppgit::result::ls_files const& files);
    std::unique_ptr<TreeItem> root_;
    std::unique_ptr<Impl> impl_;
    std::string local_user_;
};

#endif // UI_REPOTREEMODEL_HPP_