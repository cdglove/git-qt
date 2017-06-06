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
#include <boost/unordered/unordered_map_fwd.hpp>

// -----------------------------------------------------------------------------
//
namespace cppgit { namespace result {
    class ls_files;
}}

namespace cppgit { namespace result { namespace lfs {
    class ls_files;
    class lock_status;
}}}

namespace boost { namespace filesystem { 
    class path;
}}

class QMenu;

// -----------------------------------------------------------------------------
//
class RepoTreeModel : public QAbstractItemModel
{
public:

    RepoTreeModel(cppgit::result::ls_files const& files, QObject *parent);
    ~RepoTreeModel();

    void set_lfs_files(cppgit::result::lfs::ls_files const& files);
    void update_lock_status(cppgit::result::lfs::lock_status const& files);
    
    bool is_file(QModelIndex const& idx);
    boost::filesystem::path get_path(QModelIndex const& idx);

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
    void create_tree(cppgit::result::ls_files const& files);
    std::unique_ptr<boost::unordered::unordered_map<boost::filesystem::path, TreeItem*>> items_by_path_;
    std::unique_ptr<TreeItem> root_;
};

#endif // UI_REPOTREEMODEL_HPP_