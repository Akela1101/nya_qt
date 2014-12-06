/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
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
switchD( s )
{
	std::cout << "Non-non\n";
}

// or
...
switch_( s )

*/

#include <functional>

#define c___c(MT, T, op) static const MT<T, std::function<void(void)>> m___m = { { op, [&]()
#define case_(op)        }, { op, [&]()
#define switch_(s)       } }; auto i___i = m___m.find(s); auto i___end = m___m.end(); if( i___i != i___end ) i___i->second();
#define switchD(s)       switch_(s) else

#define caseS(op) c___c(std::unordered_map, std::string, op)
// For QT 5:
#define caseQ(op) c___c(QHash, QString, op)


#endif // SWITCHNYA_H
