// *****************************************************************************
// 
// main.cpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************

#include "ui/mainwindow.hpp"
#include "boost/nowide/integration/filesystem.hpp"
#include <QApplication>
#include <QtCore/QtPlugin>

#if defined(_WIN32) && defined(QT_STATIC)
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

int main(int argc, char *argv[])
{
    // Make filesystem::path user utf8 encoding.
    boost::nowide::nowide_filesystem();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
