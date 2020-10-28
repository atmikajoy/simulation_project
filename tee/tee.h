#ifndef TEE_H_INCLUDED
#define TEE_H_INCLUDED

#include <iostream>

namespace atmik
{
	namespace util
	{
		// CHAR_TYPE is the character type eg. char
		// TRAITS_TYPE is the charater traits type eg. std::char_traits<char>
		template < typename CHAR_TYPE, typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >
		struct basic_teebuf : std::basic_streambuf<CHAR_TYPE, TRAITS_TYPE>
		{
			using streambuf_type = std::basic_streambuf<CHAR_TYPE, TRAITS_TYPE>;
			// the two stream buffers which make up the  tee
			// invariant: these buffers must be open for output
			struct basic_teebuf(streambuf_type* first, streambuf_type* second)
				: first(first), second(second) {}

			~basic_teebuf() { sync(); } // flush both buffers at the end


		protected:

			using int_type = typename TRAITS_TYPE::int_type;

			virtual int_type overflow(int_type ich = TRAITS_TYPE::eof()) override
			{
				static constexpr auto eof = TRAITS_TYPE::eof();

				// EOF, nothing needs to be done
				if (TRAITS_TYPE::eq_int_type(ich, eof)) return TRAITS_TYPE::not_eof(ich);

				const char ccc = TRAITS_TYPE::to_char_type(ich);
				if (first->sputc(ccc) != eof && second->sputc(ccc) != eof)  return TRAITS_TYPE::not_eof(ich);
				else return eof; // sputc failed; return eof
			}

			virtual int sync() override
			{
				return first->pubsync() | second->pubsync();
			}

		private:
			// the two stream buffers which make up the  tee
			streambuf_type* first;
			streambuf_type* second;
		};

		// the tee_stream uses the two stream buffers (of the ywo streams passed to the constructor)
		// ideally, do not use these two streams till after the tee_stream is destroyed. 
		template < typename CHAR_TYPE, typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >
		struct basic_teestream : std::basic_ostream<CHAR_TYPE,TRAITS_TYPE>
		{
			using base = std::basic_ostream<CHAR_TYPE, TRAITS_TYPE>;
			using teebuf_type = basic_teebuf<CHAR_TYPE, TRAITS_TYPE>;
			using stdbuf_type = std::basic_streambuf<CHAR_TYPE, TRAITS_TYPE>;
			basic_teestream( base& a, base& b) : base(std::addressof(stmbuf)), stmbuf(a.rdbuf(), b.rdbuf())
			{}

			basic_teestream( stdbuf_type* a, stdbuf_type* b ) : base(std::addressof(stmbuf)), stmbuf(a,b) {}

		    private: teebuf_type stmbuf;
		};

		// type aliases for syntactic sugar
		using teebuf = basic_teebuf<char>;
		using wteebuf = basic_teebuf<wchar_t>;
		using teestream = basic_teestream<char>;
		using wteestream = basic_teestream<wchar_t>;

	} // util 
} // atmik

#endif // TEE_H_INCLUDED