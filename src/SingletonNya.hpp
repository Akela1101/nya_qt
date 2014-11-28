/*
 * File:   SingletonNya.hpp
 * Author: Akela1101
 *
 */

#ifndef SINGLETONNYA_HPP
#define SINGLETONNYA_HPP

namespace Nya
{
	template<typename T>
	class Singleton
	{
	private:
		static T *instance;
		static bool isAllocated;

	protected:
		Singleton() {}

	private:
		Singleton(const Singleton<T> &);
		Singleton &operator=(const Singleton<T> &);

	public:
		// Get singleton
		static T &GS()
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

		// Delete (in case it must be deleted before exit).
		static void Delete()
		{
			delete instance;
		}
	};

	template<typename T> T* Singleton<T>::instance = 0;
	template<typename T> bool Singleton<T>::isAllocated = false;
}

#endif // SINGLETONNYA_HPP
