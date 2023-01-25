//
// Created by Mario on 2023/1/19.
//

#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

template<class T>
using IntrusivePtr = boost::intrusive_ptr<T>;

template<class T>
using IntrusiveCounter = boost::intrusive_ref_counter<T, boost::sp_adl_block::thread_safe_counter>;

template<class T>
using IntrusiveUnsafeCounter = boost::intrusive_ref_counter<T, boost::sp_adl_block::thread_unsafe_counter>;