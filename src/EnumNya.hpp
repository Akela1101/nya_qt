/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef ENUMNYA_HPP
#define ENUMNYA_HPP

/*
// If you want smart enum:
enum MyEnum
{
	One,
	Two = 20,
	Three,     // It's the third one
	Four = 40, // It's the fourth one
	Five,
	Six
};

// Write something like this:
#define MyEnumDef(K, L, M, N) \
	K(One) \
	L(Two, 20) \
	M(Three,    "It's the third one") \
	N(Four, 40, "It's the fourth one") \
	K(Five) \
	K(Six)
NYA_ENUM(MyEnum, MyEnumDef)

// Getting string:
const char* s = etos(One);

// Getting number:
MyEnum e = stoe("One");

*/

#define ENUM_K(KEY) KEY,
#define ENUM_KV(KEY, VALUE) KEY = VALUE,
#define ENUM_KC(KEY, COMMENT) ENUM_K(KEY)
#define ENUM_KVC(KEY, VALUE, COMMENT) ENUM_KV(KEY, VALUE)

#define CASE_K(KEY) case KEY: return #KEY;
#define CASE_KV(KEY, VALUE) CASE_K(KEY)
#define CASE_KC(KEY, COMMENT) CASE_K(KEY)
#define CASE_KVC(KEY, VALUE, COMMENT) CASE_K(KEY)

#define MAP_K(KEY) { #KEY, KEY },
#define MAP_KV(KEY, VALUE) MAP_K(KEY)
#define MAP_KC(KEY, COMMENT) MAP_K(KEY)
#define MAP_KVC(KEY, VALUE, COMMENT) MAP_K(KEY)

#if __cplusplus >= 201103L
#include <unordered_map>
#include <string>
#define _prefix_nya template<typename ...>
#define NYA_ENUM(ENUM_NAME, ENUM_DEF) \
	enum ENUM_NAME { ENUM_DEF(ENUM_K, ENUM_KV, ENUM_KC, ENUM_KVC) }; \
	_prefix_nya const char* etos(ENUM_NAME e) \
	{ switch( e ) { ENUM_DEF(CASE_K, CASE_KV, CASE_KC, CASE_KVC) default: return "!~" #ENUM_NAME "~"; } } \
	_prefix_nya ENUM_NAME stoe(const char* s) \
	{ static std::unordered_map<std::string, ENUM_NAME> m = { ENUM_DEF(MAP_K, MAP_KV, MAP_KC, MAP_KVC) }; \
	  auto it = m.find(s); if( it != m.end() ) return it->second; return (ENUM_NAME)-1; }
#else
// C or C++03
// It'll cause warnings. You can supress them with -Wno-unused-function
#define NYA_ENUM(ENUM_NAME, ENUM_DEF) \
	enum ENUM_NAME { ENUM_DEF(ENUM_K, ENUM_KV, ENUM_KC, ENUM_KVC) }; \
	static const char* etos(ENUM_NAME e) \
	{  switch( e ) { ENUM_DEF(CASE_K, CASE_KV, CASE_KC, CASE_KVC) default: return "!~" #ENUM_NAME "~"; } }
#endif


#endif // ENUMNYA_HPP
