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
#include "cppgit/result/lfs/locks.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_view.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

#include "util/profiler.hpp"
#include <QApplication>

// -----------------------------------------------------------------------------
//
class RepoTreeModel::TreeItem
{
public:

    enum class Type : std::uint8_t
    {
        Directory,
        File,
    };

    enum class LfsFlag : std::uint8_t
    {
        NotLfs,
        IsLfs,
    };

    enum class LfsStatus : std::uint8_t
    {
        Unlocked,
        Locked,
        Held,
    };

    TreeItem()
        : parent_(nullptr)
        , type_(Type::Directory)
        , lfs_flag_(LfsFlag::NotLfs)
        , lfs_status_(LfsStatus::Unlocked)
    {
        title_ = QApplication::translate("RepoWindow", "File", Q_NULLPTR);
        status_ = QApplication::translate("RepoWindow", "Status", Q_NULLPTR);
    }

    explicit TreeItem(boost::filesystem::path const& path, Type type, TreeItem* parent = nullptr)
        : parent_(parent)
        , type_(type)
    {
        narrow_path_ = path.string();
        path_ = std::move(path);
        title_ = QString::fromStdString(path_.leaf().string());
    }

    void appendChild(std::unique_ptr<TreeItem> item)
    {
        children_.emplace_back(std::move(item));
    }

    void setIsInLfs()
    {
        lfs_flag_ = LfsFlag::IsLfs;
    }

    void setLocked(boost::string_view holder)
    {
        lfs_status_ = LfsStatus::Locked;
        status_ = QApplication::translate("RepoWindow", "Locked", Q_NULLPTR) + ": " + holder.data();
    }

    void setUnlocked()
    {
        lfs_status_ = LfsStatus::Unlocked;
        status_ = "";
    }

    TreeItem* child(int row) const
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
        case 0: return title_;
        case 1: return status_;
        };
        
        BOOST_ASSERT(false);
        return QVariant();
    }

    TreeItem* parentItem() const
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

    bool isFile() const
    {
        return type_ == Type::File;
    }

    boost::string_view getPathView() const
    {
        return narrow_path_;
    }

    boost::filesystem::path getPath() const
    {
        return path_;
    }

    bool isInLfs() const
    {
        return lfs_flag_ == LfsFlag::IsLfs;
    }


private:

    boost::filesystem::path path_;
    std::string narrow_path_;
    std::vector<std::unique_ptr<TreeItem>> children_;
    QVariant title_;
    QVariant status_;
    TreeItem* parent_;
    Type type_ : 1;
    LfsFlag lfs_flag_ : 1;
    LfsStatus lfs_status_ : 2;
};

// -----------------------------------------------------------------------------
//

namespace boost 
{
    static std::size_t hash_value(boost::string_view s)
    {
        return boost::hash_range(s.begin(), s.end());
    }
}

// -----------------------------------------------------------------------------
//
struct RepoTreeModel::Impl
{
    typedef boost::multi_index::multi_index_container<
      RepoTreeModel::TreeItem*,
      boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
          boost::multi_index::const_mem_fun<TreeItem, boost::string_view, &TreeItem::getPathView>
        >,
        boost::multi_index::hashed_unique<
          boost::multi_index::const_mem_fun<TreeItem, boost::filesystem::path, &TreeItem::getPath>
        >
      >
    > TreeItemDatabase;

    TreeItemDatabase items_by_path_;
};

// -----------------------------------------------------------------------------
//
RepoTreeModel::RepoTreeModel(cppgit::result::ls_files const& files, QObject *parent)
    : QAbstractItemModel(parent)
    , root_(std::make_unique<TreeItem>())
    , impl_(std::make_unique<Impl>())
    , local_user_("local_user")
{
    create_tree(files);
}

RepoTreeModel::~RepoTreeModel()
{

}

void RepoTreeModel::set_lfs_files(cppgit::result::lfs::ls_files const& files)
{
    for(auto&& file : files.files)
    {
        auto existing_item = *impl_->items_by_path_.find(file.file);
        existing_item->setIsInLfs();
    }
}

void RepoTreeModel::update_lock_status(cppgit::result::lfs::locks const& files)
{
    for(auto&& file : files.files)
    {
        auto existing_item = *impl_->items_by_path_.find(file.file);
        existing_item->setLocked(file.holder);
    }
}

void RepoTreeModel::update_lock_status(cppgit::result::lfs::lock const& file)
{
    if(file.success)
    {
        auto existing_item = *impl_->items_by_path_.find(file.file);
        existing_item->setLocked(local_user_);
    }
}

void RepoTreeModel::update_lock_status(boost::string_view path, cppgit::result::lfs::unlock const& file)
{
    if(file.success)
    {
        auto existing_item = *impl_->items_by_path_.find(path);
        existing_item->setUnlocked();
    }
}

bool RepoTreeModel::is_file(QModelIndex const& idx)
{
    return static_cast<TreeItem*>(idx.internalPointer())->isFile();
}

boost::string_view RepoTreeModel::get_path_view(QModelIndex const& idx)
{
    return static_cast<TreeItem*>(idx.internalPointer())->getPathView();
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

    auto& items_by_path = impl_->items_by_path_;
    items_by_path.clear();
    items_by_path.insert(root_.get());

    for(auto&& file : files.files)
    {
        TreeItem* parent = root_.get();
        boost::filesystem::path file_path(file);
        boost::filesystem::path directory_path = file_path.parent_path();
        
        // Find the closest ancestor.
        auto parent_item = items_by_path.find(directory_path.string());

        // Create path to closest ancestor if necessary.
        if(parent_item == items_by_path.end())
        {
            auto closest_path = directory_path;
            while(parent_item == items_by_path.end())
            {
                closest_path = closest_path.parent_path(); 
                parent_item = items_by_path.find(closest_path.string());
            } 

            parent = *parent_item;

            // Create items for the missing path segments.
            auto append = directory_path.begin();
            std::advance(append, std::distance(closest_path.begin(), closest_path.end()));

            while(append != directory_path.end())
            {
                closest_path /= *append;
                auto child = std::make_unique<TreeItem>(closest_path, TreeItem::Type::Directory, parent);
                auto child_ptr = child.get();
                items_by_path.insert(child_ptr);
                parent->appendChild(std::move(child));
                parent = child_ptr;
                ++append;
            }
        }
        else
        {
            parent = *parent_item;
        }

        auto new_item = std::make_unique<TreeItem>(file_path, TreeItem::Type::File, parent);
        items_by_path.insert(new_item.get());
        parent->appendChild(std::move(new_item));
    }
}
