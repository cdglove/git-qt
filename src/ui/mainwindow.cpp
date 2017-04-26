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
#include "ui_mainwindow.h"

#include <boost/utility/string_view.hpp>

#include <QFileDialog>
#include <QMessageBox>

// -----------------------------------------------------------------------------
//
//template<typename Fn>
//void for_all_children(LfsTreeWidgetItem* item, Fn fn)
//{
//    fn(item);
//    for(int i = 0; i < item->childCount(); ++i)
//    {
//        for_all_children(static_cast<LfsTreeWidgetItem*>(item->child(i)), fn);
//    }
//}
//
//template<typename Fn>
//void for_all_ancestors(LfsTreeWidgetItem* item, Fn fn)
//{
//    do 
//    {
//        fn(item);
//        item = item->parent();
//    }
//    while(item);    
//}

// -----------------------------------------------------------------------------
//
MainWindow::MainWindow()
    : QMainWindow()
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{}

// -----------------------------------------------------------------------------
//
void MainWindow::filterTextChanged(QString const& filter_text)
{
    //auto do_filter = [filter_text, this](QTreeWidget* tree)
    {
        //std::vector<std::string> match_list;
        //std::string filter_text_std = filter_text.toStdString();
        //boost::algorithm::split(
        //    match_list, filter_text_std,
        //    boost::algorithm::is_any_of(" \0\t\r"),
        //    boost::algorithm::token_compress_on);

        //match_list.erase(
        //    std::remove_if(
        //        match_list.begin(),
        //        match_list.end(),
        //        [](std::string const& s)
        //        {
        //            return s.empty();
        //        }
        //    ),
        //    match_list.end()
        //);

        //if(match_list.empty())
        //{
        //    for(auto&& item : all_include_tree_items_)
        //        item->setHidden(false);
        //}
        //else
        //{
        //    auto match_all_substrings = [&match_list](
        //        cpp_dep::include_vertex_descriptor_t const& v,
        //        cpp_dep::include_graph_t const& g)
        //    {
        //        cpp_dep::include_vertex_t const& file = g[v];
        //        return std::all_of(
        //            match_list.begin(),
        //            match_list.end(),
        //            [&file](std::string const& sub_str)
        //            {
        //                return file.name.find(sub_str) != std::string::npos;
        //            }
        //        );
        //    };

        //    tree_view_filter graph_filter;
        //    graph_filter(*include_graph_, all_include_tree_items_, match_all_substrings);
        //}

        //return tree;
    };

    //if(include_graph_)
    //    update_tree_widget_.run_or_enqueue(do_filter, new QTreeWidget);
}
// -----------------------------------------------------------------------------
//
void MainWindow::lfsTreeItemChanged(QTreeWidgetItem* changed_item_, int column)
{
    //if(building_include_tree_)
    //    return;

    //if(column != IncludeTreeWidgetItem::Column::File)
    //    return;

    //tree_view_include_size_calculator calculate_sizes(all_include_tree_items_);
    //calculate_sizes(*include_graph_);
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

void MainWindow::openNewRepo(QDir const& where)
{
    QWidget* tab = new QWidget();
    RepoWindow* repo = new RepoWindow(cppgit::repository(where.absolutePath().toStdString()), tab);
    repo->setGeometry(QRect(10, 10, 781, 521));
    int new_idx = ui->tabWidget->addTab(tab, QString());
    ui->tabWidget->setTabText(new_idx, where.path());
    ui->tabWidget->setCurrentIndex(new_idx);
}

// -----------------------------------------------------------------------------
//
//void Dialog::dropEvent(QDropEvent* event)
//{
//    QMimeData const* mime_data = event->mimeData();
//    // check for our needed mime type, here a file or a list of files
//    if (mime_data->hasUrls())
//    {
//        QList<QUrl> urls = mime_data->urls();
//        if(urls.size() > 0)
//        {
//            // Just take the first one in case the user dragged multiple.
//            QString file = urls.at(0).toLocalFile();
//            try
//            {
//                cpp_dep::include_graph_t includes =
//                    cpp_dep::read_deps_file(file.toStdString().c_str());
//
//                cpp_dep::include_graph_t paths =
//                    cpp_dep::invert_to_paths(includes);
//
//                // Save the two graphs.
//                include_graph_ = std::make_unique<
//                    cpp_dep::include_graph_t
//                >(std::move(includes));
//
//                filesystem_graph_ = std::make_unique<
//                    cpp_dep::include_graph_t
//                >(std::move(paths));
//
//                populateTrees();
//            }
//            catch(std::exception& e)
//            {
//                QString msg;
//                msg += "Failed to load \"" + file + "\"\n"
//                    +  "Error: " + e.what();
//
//                QMessageBox msg_box;
//                msg_box.setText(msg);
//                msg_box.setIcon(QMessageBox::Critical);
//                msg_box.exec();
//            }
//        }
//    }
//
//    // Reapply the filter text.
//    filterTextChanged(ui->filter_text->text());
//}
//
//// -----------------------------------------------------------------------------
////
//void Dialog::dragEnterEvent(QDragEnterEvent* event)
//{
//    // if some actions should not be usable, like move, this code must be adopted
//    event->acceptProposedAction();
//}
//
//// -----------------------------------------------------------------------------
////
//void Dialog::dragMoveEvent(QDragMoveEvent* event)
//{
//    // if some actions should not be usable, like move, this code must be adopted
//    event->acceptProposedAction();
//}
//
//// -----------------------------------------------------------------------------
////
//void Dialog::dragLeaveEvent(QDragLeaveEvent* event)
//{
//    event->accept();
//}

// -----------------------------------------------------------------------------
//
//void Dialog::populateTrees()
//{
//    // Clear both trees first so that if parsing fails
//    // both are empty instead of just one.
//    all_include_tree_items_.clear();
//    ui->include_tree->clear();
//    ui->filesystem_tree->clear();
//
//    // Populate the include tree
//    {
//        BOOST_SCOPE_EXIT_ALL(this)
//        {
//            building_include_tree_ = false;
//        };
//
//        building_include_tree_ = true;
//        include_tree_view_builder build_tree(all_include_tree_items_);
//        build_tree(*include_graph_, ui->include_tree);
//
//        tree_view_include_size_calculator calculate_sizes(all_include_tree_items_);
//        calculate_sizes(*include_graph_);
//    }
//
//    // Populate the filesystem tree
//    {
//        tree_view_builder build_tree(tree_view_builder::option::none);
//        build_tree(*filesystem_graph_, ui->filesystem_tree);
//    }
//}
//
//// -----------------------------------------------------------------------------
////
//void Dialog::filterTreeBuilt(QTreeWidget* new_widget)
//{
//    ui->include_tree->update();
//}
