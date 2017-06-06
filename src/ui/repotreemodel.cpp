// *****************************************************************************
//
// ui/repotreemodel.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#include "repotreemodel.hpp"
#include "cppgit/result/ls_files.hpp"
#include "cppgit/result/lfs/ls_files.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/utility/string_view.hpp>
#include "util/profiler.hpp"
#include <QMenu>

// -----------------------------------------------------------------------------
//
class RepoTreeModel::TreeItem
{
public:

    TreeItem()
    {
        file_ = QString::fromWCharArray(L"File");
        status_ = QString::fromWCharArray(L"Status");
    }

    explicit TreeItem(boost::filesystem::path path, TreeItem* parent = nullptr)
        : parent_(parent)
        , path_(std::move(path))
    {
        file_ = QString::fromWCharArray(path_.leaf().c_str());
        status_ = QString::fromWCharArray(L"Pending");
    }

    void appendChild(std::unique_ptr<TreeItem> item)
    {
        children_.emplace_back(std::move(item));
    }

    void set_status(boost::string_view status)
    {
        status_ = status.data();
    }

    TreeItem* child(int row)
    {
        return children_[row].get();
    }

    int childCount() const
    {
        return static_cast<int>(children_.size());
    }

    int columnCount() const
    {
        return 2;
    }

    QVariant data(int column) const
    {
        switch(column)
        {
        case 0: return file_;
        case 1: return status_;
        };
        
        BOOST_ASSERT(false);
        return QVariant();
    }

    TreeItem* parentItem()
    {
        return parent_;
    }

    int row() const
    {
        if (parent_)
        {
            return std::distance(
                parent_->children_.begin(), 
                std::find_if(
                    parent_->children_.begin(),
                    parent_->children_.end(),
                    [this](std::unique_ptr<TreeItem> const& item)
                    {
                        return item.get() == this;
                    }
                )
            );
        }

        return 0;
    }

private:
    boost::filesystem::path path_;
    std::vector<std::unique_ptr<TreeItem>> children_;
    QVariant file_;
    QVariant status_;
    TreeItem* parent_;
};

// -----------------------------------------------------------------------------
//
RepoTreeModel::RepoTreeModel(cppgit::result::ls_files const& files, QObject *parent)
    : QAbstractItemModel(parent)
{
    root_ = std::make_unique<TreeItem>();
    items_by_path_ = std::make_unique<boost::unordered::unordered_map<boost::filesystem::path, TreeItem*>>();
    create_tree(files);
}

RepoTreeModel::~RepoTreeModel()
{

}

void RepoTreeModel::set_lfs_files(cppgit::result::lfs::ls_files const& files)
{
    for(auto&& file : files.files)
    {
        auto existing_item = items_by_path_->find(file.file);
        existing_item->second->set_status("Lockable");
    }
}

void RepoTreeModel::update_lock_status(cppgit::result::lfs::lock_status const& files)
{

}

bool RepoTreeModel::is_file(QModelIndex const& idx)
{
    //return static_cast<TreeItem*>(parent.internalPointer())->
    return false;
}

boost::filesystem::path RepoTreeModel::get_path(QModelIndex const& idx)
{
    return "";
}

int RepoTreeModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return root_->columnCount();
}

QVariant RepoTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags RepoTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant RepoTreeModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root_->data(section);

    return QVariant();
}

QModelIndex RepoTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem* parent_item = root_.get();
    if (parent.isValid())
        parent_item = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* child_item = parent_item->child(row);
    if (child_item)
        return createIndex(row, column, child_item);
    else
        return QModelIndex();
}

QModelIndex RepoTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *child_item = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parent_item = child_item->parentItem();

    if (parent_item == root_.get())
        return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);
}

int RepoTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem* parent_item = root_.get();
    if (parent.isValid())
        parent_item = static_cast<TreeItem*>(parent.internalPointer());

    return parent_item->childCount();
}

void RepoTreeModel::create_tree(cppgit::result::ls_files const& files)
{
    DAILY_AUTO_PROFILE_NODE(create_tree);

    items_by_path_->clear();
    items_by_path_->insert(std::make_pair(boost::filesystem::path(""), root_.get()));

    for(auto&& file : files.files)
    {
        TreeItem* parent = root_.get();
        boost::filesystem::path file_path(file);
        boost::filesystem::path directory_path = file_path.parent_path();
        
        // Find the closest ancestor.
        auto parent_item = items_by_path_->find(directory_path);

        // Create path to closest ancestor if necessary.
        if(parent_item == items_by_path_->end())
        {
            auto closest_path = directory_path;
            while(parent_item == items_by_path_->end())
            {
                closest_path = closest_path.parent_path(); 
                parent_item = items_by_path_->find(closest_path);
            } 

            parent = parent_item->second;

            // Create items for the missing path segments.
            auto append = directory_path.begin();
            std::advance(append, std::distance(closest_path.begin(), closest_path.end()));

            while(append != directory_path.end())
            {
                closest_path /= *append;
                auto child = std::make_unique<TreeItem>(closest_path, parent);
                auto child_ptr = child.get();
                items_by_path_->insert(std::make_pair(closest_path, child_ptr));
                parent->appendChild(std::move(child));
                parent = child_ptr;
                ++append;
            }
        }
        else
        {
            parent = parent_item->second;
        }

        auto new_item = std::make_unique<TreeItem>(file_path.filename(), parent);
        items_by_path_->insert({file_path, new_item.get()});
        parent->appendChild(std::move(new_item));
    }
}
