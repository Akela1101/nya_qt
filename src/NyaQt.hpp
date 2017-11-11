#ifndef NYAQT_HPP
#define	NYAQT_HPP

#include <memory>
#include "nya.hpp"
#include "LogNya.hpp"

#define InvMet QMetaObject::invokeMethod

// Empty object singleton
template<class T> const T& EmptyObject() { static T t; return t; }

#endif	/* NYAQT_HPP */

