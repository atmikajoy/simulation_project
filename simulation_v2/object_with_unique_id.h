#ifndef OBJECT_WITH_UNIQUE_ID_H_INCLUDED
#define OBJECT_WITH_UNIQUE_ID_H_INCLUDED

#include <map>
#include <type_traits>

namespace atmik
{
	namespace util
	{
		template < typename DERIVED, unsigned int FIRST = 0 > struct object_with_unique_id
		{
			//below is a c++ 17 update, not allowed here as DERIVED isnt a complete type yet
			// static_assert(std::is_base_of_v< object_with_unique_id<DERIVED, FIRST>, DERIVED> );

			unsigned int id() const noexcept { return id_; }

			object_with_unique_id() = default;
			~object_with_unique_id() = default;

			// non-copyable, non-moveable, non-assignable
			object_with_unique_id(const object_with_unique_id& that) = delete ;
			object_with_unique_id(object_with_unique_id&& that) = delete;

			template < typename... CTOR_ARGS >
			static DERIVED& create(CTOR_ARGS&&... args)
			{
				return objects().try_emplace( sl_no+1, std::forward< CTOR_ARGS>(args)... ).first->second;
			}

			static DERIVED& look_up(unsigned int id ) // will throw if the id is invalid
			{
				return objects().at(id);
			}

			static void erase(unsigned int id) { objects().erase(id); }

			static bool exists(unsigned int id)
			{
				return objects().find(id) != objects().end();
			}

			const unsigned int id_ = ++sl_no;

			static unsigned int sl_no;
			static std::map<unsigned int, DERIVED>& objects() ;
		};

		template < typename DERIVED, unsigned int FIRST >
		unsigned int object_with_unique_id<DERIVED,FIRST>::sl_no = FIRST;

		template < typename DERIVED, unsigned int FIRST >
		std::map<unsigned int, DERIVED>& object_with_unique_id<DERIVED, FIRST>::objects()
		{
			static std::map<unsigned int, DERIVED> map;
			return map;
		}
	}
}

#endif // OBJECT_WITH_UNIQUE_ID_H_INCLUDED

