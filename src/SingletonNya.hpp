/****************************************************
 *
 * Author: Akela1101 <akela1101@gmail.com>
 *
 ****************************************************/

#ifndef SINGLETONNYA_HPP
#define SINGLETONNYA_HPP

namespace Nya
{
	// Simple singleton.
	template<typename T>
	class Singleton
	{
	private:
		static T *instance;
		static bool isAllocated;

	protected:
		Singleton() {}

	private:
		Singleton(const Singleton<T>&);
		Singleton& operator=(const Singleton<T>&);

	public:
		// get singleton and create if need
		static T& GS()
		{
			if( !instance )
			{
				instance = (T*)::operator new(sizeof(T)); // allocate
				if( !isAllocated )
				{
					isAllocated = true;
					new(instance) T; // construct
				}
			}
			return *instance;
		}
		// delete (in case it must be deleted before exit)
		static void Delete() { delete instance; }
	};	
	template<typename T> T* Singleton<T>::instance = 0;
	template<typename T> bool Singleton<T>::isAllocated = false;


	// Base class for derived singleton.
	template<typename T>
	class SingletonBase
	{
	protected:
		static T *instance;
		static bool isAllocated;

	public:
		// get singleton
		static T& GS() { return *instance; }
		// create in derived class
		template<typename S>
		static S& GS()
		{
			if( !instance )
			{
				instance = (S*)::operator new(sizeof(S)); // allocate
				if( !isAllocated )
				{
					isAllocated = true;
					new(instance) S; // construct
				}
			}
			return *(S*)instance;
		}
		// delete (in case it must be deleted before exit)
		static void Delete() { delete instance; }
	};
	template<typename T> T* SingletonBase<T>::instance = 0;
	template<typename T> bool SingletonBase<T>::isAllocated = false;
}

#endif // SINGLETONNYA_HPP
