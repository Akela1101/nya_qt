/****************************************************
 *
 * Copyright (c) 2013 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef NYA_HPP
#define	NYA_HPP

// Try to include this file to the last of all includes,
// because of some necessary #defines.

#include <limits>
#include <memory>

#define umap std::unordered_map
#define uset std::unordered_set
#define tloc thread_local
#define qnan std::numeric_limits<double>::quiet_NaN()
#define infin std::numeric_limits<double>::infinity()

#define all(x) (x).begin(), (x).end()
#define u_p std::unique_ptr
#define s_p std::shared_ptr
#define w_p std::weak_ptr
#define sp_cast std::static_pointer_cast
#define dp_cast std::dynamic_pointer_cast
#define make_p std::make_pair
#define make_s std::make_shared
template<typename T, typename ...Args>
u_p<T> make_u(Args&& ...args) { return u_p<T>(new T(std::forward<Args>(args)...)); }


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef long long dint;
typedef unsigned long long udint;


#define InvMet QMetaObject::invokeMethod
#define NYA_DT_FORMAT "yyyy-MM-dd hh:mm:ss"
#define NYA_DATE_FORMAT "yyyy-MM-dd"

#include "LogNya.hpp"

// Empty object singleton
template<class T> const T& EmptyObject() { static T t; return t; }

#endif	/* NYA_HPP */

