/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef _NYA_HPP
#define	_NYA_HPP

// STD
#define std_hash std::unordered_map

#include <memory>
#define u_p std::unique_ptr
#define s_p std::shared_ptr
#define w_p std::weak_ptr
#define mv_ std::move
#define sp_cast std::static_pointer_cast
#define dp_cast std::dynamic_pointer_cast
#define make_s_p std::make_shared
template<typename T, typename ...Args>
u_p<T> make_u_p(Args&& ...args) { return u_p<T>(new T(std::forward<Args>(args)...)); }


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef long long dint;
typedef unsigned long long udint;

// QT
#ifdef QXT_STATIC
#include "qxtlogger.h"
#endif

#define InvMet QMetaObject::invokeMethod

#endif	/* _NYA_HPP */

