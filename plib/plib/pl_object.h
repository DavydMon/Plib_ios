#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace pl
{

	class PLObject
	{
		public:
			virtual std::int32_t get_id() const = 0;

			PLObject() = default;
#if defined(ANDROID_WRAP)
            PLObject(const PLObject &) = default;
#else
            PLObject(const PLObject &) = delete;
#endif
			PLObject &operator=(const PLObject &) = delete;
			PLObject(PLObject &&) = default;
			PLObject &operator=(PLObject &&) = default;

			virtual ~PLObject() = default;
	};

	template <class Type>
	using tl_object_ptr = std::unique_ptr<Type>;

	template <class Type, class... Args>
	tl_object_ptr<Type> make_tl_object(Args &&... args)
	{
		return tl_object_ptr<Type>(new Type(std::forward<Args>(args)...));
	}

	template <class ToT, class FromT>
	tl_object_ptr<ToT> move_tl_object_as(tl_object_ptr<FromT> &from)
	{
		return tl_object_ptr<ToT>(static_cast<ToT *>(from.release()));
	}

	template <class ToT, class FromT>
	tl_object_ptr<ToT> move_tl_object_as(tl_object_ptr<FromT> &&from)
	{
		return tl_object_ptr<ToT>(static_cast<ToT *>(from.release()));
	}

}  
