#pragma once

#ifdef CCONTAINER_EXPORTS
#define CCONTAINER_API __declspec(dllexport)
#else
#define CCONTAINER_API __declspec(dllimport)
#endif

#include <iterator>
#include <random>
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")

constexpr int CONT_OK{ 5000 };
constexpr int BAD_PTR{ 5001 };
constexpr int BAD_INDEX{ 5002 };
constexpr int BAD_PARAM{ 5003 };
constexpr int BAD_ERR{ 5004 };

namespace contlib
{
	class CCONTAINER_API RAND
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RAND();
		~RAND();

		int operator()(int min, int max);
		float operator()(float min, float max);
	};
	class CCONTAINER_API EXCEPTION
	{
	private:
		int what{ BAD_ERR };

	public:

		EXCEPTION(int err);

		const wchar_t* eGet()const;
	};

	template<typename T> class BAG
	{
	private:
		T* mPtr{ nullptr };
		size_t max_size{ 1 };
		size_t next_pos{ 0 };

	public:

		BAG()
		{
			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}
		BAG(size_t lenght)
		{
			max_size = lenght;
			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}
		BAG(BAG& other)
		{
			if (other.mPtr == nullptr || other.max_size == 1)mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;
				
				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (mPtr == nullptr)throw EXCEPTION(BAD_PTR);
				else for (size_t count = 0; count < next_pos; ++count)mPtr[count] = other.mPtr[count];
			}
		}
		BAG(BAG&& other)
		{
			if (other.mPtr == nullptr || other.max_size == 1)mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;
				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}
		}

		~BAG()
		{
			free(mPtr);
		}

		size_t size() const
		{
			return next_pos;
		}
		size_t capacity() const
		{
			return max_size;
		}

		bool empty() const
		{
			return (next_pos == 0);
		}

		bool operator == (BAG<T>& other)
		{
			return (mPtr == other.mPtr);
		}
		bool operator != (BAG<T>& other)
		{
			return (mPtr != other.mPtr);
		}
		bool operator < (BAG<T>&other)
		{
			return (max_size < other.max_size);
		}
		bool operator > (BAG<T>& other)
		{
			return (max_size > other.max_size);
		}
		bool operator <= (BAG<T>& other)
		{
			return (max_size <= other.max_size);
		}
		bool operator >= (BAG<T>& other)
		{
			return (max_size >= other.max_size);
		}

		T& operator[] (size_t index)
		{
			if (index < 0 || index >= next_pos)throw EXCEPTION(BAD_INDEX);

			if (!mPtr)throw EXCEPTION(BAD_PTR);
			
			return mPtr[index];	
		}

		T& front()
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			if (empty())throw EXCEPTION(BAD_INDEX);

			return (*mPtr);
		}
		T& back()
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			if (empty())throw EXCEPTION(BAD_INDEX);

			return mPtr[next_pos - 1];
		}

		void clear()
		{
			free(mPtr);

			max_size = 1;
			next_pos = 0;

			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}

		BAG<T>& operator= (BAG<T>& other)
		{
			free(mPtr);

			if (!other.mPtr)throw EXCEPTION(BAD_PARAM);
			else
			{
				if (mPtr == other.mPtr)throw EXCEPTION(BAD_PARAM);
				else
				{
					max_size = other.max_size;
					next_pos = other.next_pos;

					if (other.empty())mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
					else
					{
						mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

						if (!mPtr)throw EXCEPTION(BAD_PTR);
						else for (size_t count = 0; count < next_pos; ++count)mPtr[count] = other.mPtr[count];
					}
				}
			}

			return (*this);
		}
		BAG<T>& operator= (BAG<T>&& other)
		{
			if (other.mPtr == nullptr)throw EXCEPTION(BAD_PTR);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}

			return (*this);
		}

		void push_back(T element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr[next_pos] = element;
						++next_pos;
					}
				}

			}
		}
		void push_back(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr[next_pos] = *element;
						++next_pos;
					}
				}
			}
		}

		void push_front(T element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					*mPtr = element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						for (size_t count = next_pos; count > 0; ++count)mPtr[count] = mPtr[count - 1];

						*mPtr = element;
						++next_pos;
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					*mPtr = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						for (size_t count = next_pos; count > 0; ++count)mPtr[count] = mPtr[count - 1];

						*mPtr = *element;
						++next_pos;
					}
				}
			}
		}

		void erase(size_t index)
		{
			if (index < 0 || index >= next_pos)return;

			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				for (size_t count = index; count < next_pos - 1; ++count)mPtr[count] = mPtr[count + 1];
				--next_pos;
			}
		}
		
		void insert(T element, size_t index)
		{
			if (index < 0 || index >= next_pos)return;

			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];

					mPtr[index] = element;

					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];

						mPtr[index] = element;

						++next_pos;
					}
				}
			}
		}
		void insert(T* element, size_t index)
		{
			if (index < 0 || index >= next_pos)return;

			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];

					mPtr[index] = *element;

					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(BAD_PTR);
					else
					{
						for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];

						mPtr[index] = *element;

						++next_pos;
					}
				}
			}
		}

		class iterator
		{
		private:
			T* it_ptr{ nullptr };

		public:

			using iterator_category = std::bidirectional_iterator_tag;
			using diferrence_type = ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			friend class BAG<T>;

			iterator(T* init) :it_ptr{ init } {};

			T& operator * ()
			{
				return *it_ptr;
			}
			T* operator -> ()
			{
				return it_ptr;
			}

			iterator& operator ++ ()
			{
				++it_ptr;

				return (*this);
			}
			iterator operator ++ (int)
			{
				iterator temp = (*this);

				++(*this);

				return temp;
			}

			iterator& operator -- ()
			{
				--it_ptr;

				return (*this);
			}
			iterator operator -- (int)
			{
				iterator temp = (*this);

				--(*this);

				return temp;
			}

			iterator& operator + (const size_t addent)
			{
				it_ptr += addent;

				return (*this);
			}
			iterator& operator - (const size_t substracter)
			{
				it_ptr -= substracter;

				return (*this);
			}

			friend bool operator == (const iterator& current, const iterator& other)
			{
				return (current.it_ptr == other.it_ptr);
			}
			friend bool operator != (const iterator& current, const iterator& other)
			{
				return (current.it_ptr != other.it_ptr);
			}
			
			bool operator < (const iterator& other)
			{
				return (it_ptr < other.it_ptr);
			}
			bool operator > (const iterator& other)
			{
				return (it_ptr > other.it_ptr);
			}
			bool operator <= (const iterator& other)
			{
				return (it_ptr <= other.it_ptr);
			}
			bool operator >= (const iterator& other)
			{
				return (it_ptr >= other.it_ptr);
			}
		};

		iterator begin()
		{
			return iterator(mPtr);
		}
		iterator end()
		{
			return iterator(&mPtr[next_pos]);
		}

		void erase(iterator it)
		{
			for (size_t ind = 0; ind < next_pos; ++ind)
				if (mPtr + ind == it.it_ptr)erase(ind);
		}
	};

	// FUNCTIONS **************************

	float CCONTAINER_API Distance(D2D1_POINT_2F init_point, D2D1_POINT_2F ref);
	
	int CCONTAINER_API SortBag(BAG<D2D1_POINT_2F>& bag, D2D1_POINT_2F ref, bool ascending = true);

	int CCONTAINER_API SortBag(BAG<D2D1_POINT_2F>& bag, D2D1_RECT_F ref_rect, bool ascending = true);
}