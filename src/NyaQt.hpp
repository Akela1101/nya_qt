#ifndef NYAQT_HPP
#define	NYAQT_HPP

#define ELPP_QT_LOGGING

#include <memory>
#include <nya.hpp>
#include <nya/log.hpp>

#define InvMet QMetaObject::invokeMethod

// Empty object singleton
template<class T> const T& EmptyObject() { static T t; return t; }

#endif	/* NYAQT_HPP */

