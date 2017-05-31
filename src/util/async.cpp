// *****************************************************************************
//
// util/async.hpp
//
// Copyright Chris Glover 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// *****************************************************************************
#include "async.hpp"
#include <thread>
#include <memory>

static boost::asio::io_service worker_io_service;
static std::thread worker;
static std::unique_ptr<boost::asio::io_service::work> work;
void start_worker_io_service()
{
    work = std::make_unique<boost::asio::io_service::work>(worker_io_service);
    worker = std::thread([]
    {
        worker_io_service.run();
    });
}

void stop_worker_io_service()
{
    work.reset();
    worker.join();
}

boost::asio::io_service& get_worker_io_service()
{
    return worker_io_service;
}

static boost::asio::io_service ui_io_service;
boost::asio::io_service& get_ui_io_service()
{
    return ui_io_service;
}