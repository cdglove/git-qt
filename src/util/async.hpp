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
#pragma once
#ifndef UTIL_ASYNC_HPP_
#define UTIL_ASYNC_HPP_

#include <boost/asio/io_service.hpp>

void start_worker_io_service();
void stop_worker_io_service();
boost::asio::io_service& get_worker_io_service();
boost::asio::io_service& get_ui_io_service();

struct io_service_executor
{
    boost::asio::io_service& service_;
    io_service_executor(boost::asio::io_service& service)
        : service_(service)
    {}

    io_service_executor& get_executor()
    {
        return *this;
    }
 
    template<typename Handler, typename Allocator>
    auto dispatch(Handler&& handler, Allocator&&)
    {
        return service_.dispatch(std::forward<Handler>(handler));
    }

    template<typename Handler, typename Allocator>
    auto post(Handler&& handler, Allocator&&)
    {
        return service_.post(std::forward<Handler>(handler));
    }

    template<typename Handler, typename Allocator>
    auto defer(Handler&& handler, Allocator&&)
    {
        return service_.defer(std::forward<Handler>(handler));
    }
};

inline io_service_executor get_worker_executor()
{
    return get_worker_io_service();
}

inline io_service_executor get_ui_executor()
{
    return get_ui_io_service();
}

#endif // UTIL_ASYNC_HPP_