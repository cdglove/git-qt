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
#include "cppgit/result/list_files.hpp"
#include <boost/unordered/unordered_map.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_view.hpp>
#include "util/profiler.hpp"

// -----------------------------------------------------------------------------
//
class RepoTreeModel::TreeItem
{
public:

    explicit TreeItem(boost::wstring_view file, boost::wstring_view status, TreeItem* parent = nullptr)
        : parent_(parent)
    {
        column_data_[0] = QString::fromWCharArray(file.data());
        column_data_[1] = QString::fromWCharArray(status.data());
    }

    void TreeItem::appendChild(std::unique_ptr<TreeItem> item)
    {
        children_.emplace_back(std::move(item));
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
        return static_cast<int>(column_data_.size());
    }

    QVariant data(int column) const
    {
        return column_data_[column];
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
    std::vector<std::unique_ptr<TreeItem>> children_;
    std::array<QVariant, 2> column_data_;
    TreeItem* parent_;
};

// -----------------------------------------------------------------------------
//
RepoTreeModel::RepoTreeModel(cppgit::result::list_files const& files, QObject *parent)
    : QAbstractItemModel(parent)
{
    root_ = std::make_unique<TreeItem>(L"File", L"Status");
    create_tree(files);
}

RepoTreeModel::~RepoTreeModel() = default;

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

void RepoTreeModel::create_tree(cppgit::result::list_files const& files)
{
    DAILY_AUTO_PROFILE_NODE(create_tree);

    boost::unordered::unordered_map<boost::filesystem::path, TreeItem*> items_by_path;
    items_by_path.insert(std::make_pair(boost::filesystem::path(""), root_.get()));
    for(auto&& file : files.file)
    {
        TreeItem* parent = root_.get();
        boost::filesystem::path file_path(file);
        boost::filesystem::path directory_path = file_path.parent_path();
        auto directory_item = items_by_path.find(directory_path);

        // Find the closest ancestor.
        auto closest_path = directory_path;
        auto parent_item = items_by_path.find(closest_path);
        while(parent_item == items_by_path.end())
        {
            closest_path = closest_path.parent_path(); 
            parent_item = items_by_path.find(closest_path);
        }

        parent = parent_item->second;

        // Create items for the missing path segments.
        if(closest_path != directory_path)
        {
            auto append = directory_path.begin();
            std::advance(append, std::distance(closest_path.begin(), closest_path.end()));

            while(append != directory_path.end())
            {
                closest_path /= *append;
                auto child = std::make_unique<TreeItem>(append->c_str(), L"", parent);
                items_by_path.insert(std::make_pair(closest_path, child.get()));
                parent->appendChild(std::move(child));
                ++append;
            }
        }

        parent->appendChild(std::make_unique<TreeItem>(file_path.filename().c_str(), L"", parent));
    }
}
