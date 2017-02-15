/****************************************************
 *
 * Copyright (c) 2014 Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef SWITCHNYA_H
#define SWITCHNYA_H

/*
// If you want switch for std::string:
switch( s )
{
	case "one": { std::cout << "!\n"; }
	case "two": { std::cout << "!!\n"; }
	case "three": { std::cout << "!!!\n"; }
	default: { std::cout << "Non-non\n"; }
}

// Write something like this:
#include "SwitchNya.hpp"
#include <string>
#include <unordered_map>

caseS( "one" )
{
	std::cout << "!\n";
}
case_( "two" )
{
	std::cout << "!!\n";
}
case_( "three" )
{
	std::cout << "!!!\n";
}
switchD( s ) // default
{
	std::cout << "Non-non\n";
}

// or
...
switch_( s )

*/

#include <functional>

// first case must define types
#define c___c(MT, T, op) const MT<T, std::function<void(void)>> m___m = { { op, [&]()
#define case_(op)        }, { op, [&]()
#define switch_(s)       } }; auto i___i = m___m.find(s); auto i___end = m___m.end(); if( i___i != i___end ) i___i->second();
#define switchD(s)       switch_(s) else

// std::string
#define caseS(op) c___c(std::unordered_map, std::string, op)
// any pointer
#define caseP(op) c___c(std::unordered_map, void*, op)
// QString
#if QT_VERSION >= 0x050000
#    define caseQ(op) c___c(QHash, QString, op)
#endif

#endif // SWITCHNYA_H
