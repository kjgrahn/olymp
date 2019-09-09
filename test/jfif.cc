#include <vector>
#include <orchis.h>
#include "hexread.h"

#include <jfif.h>


namespace {

    std::vector<uint8_t> h(const std::string& s)
    {
	std::vector<uint8_t> v(s.size()/2);
	const char* a = s.data();
	auto b = s.data() + s.size();
	auto n = hexread(v.data(), &a, b);
	orchis::assert_eq(a, b);
	v.resize(n);
	return v;
    }
}

namespace jfif {

    std::vector<Segment> parse(const uint8_t *a, const uint8_t *b,
			       const size_t stepping)
    {
	Decoder decoder;
	while(a!=b) {
	    auto c = std::min(a+stepping, b);
	    decoder.feed(a, c);
	    a = c;
	}
	return decoder.end();
    }

    namespace good {

	void assert_parses(const std::vector<uint8_t>& v,
			   const std::vector<Segment>& ref)
	{
	    const auto a = v.data();
	    const auto b = a + v.size();

	    try {
		for(size_t n = v.size(); n; n--) {
		    const auto v = parse(a, b, n);
		    orchis::assert_(v == ref);
		}
	    }
	    catch (const Decoder::Error&) {
		throw orchis::Failure("Decoder::Error");
	    }
	}

	void standalone(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "0123456789abcdef");

	    assert_parses(v, {{0xd8, h("")}});
	}

	void simple(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "ffe0 0003 69"
			     "0123456789abcdef"
			     "ffe1 0006 00112233"
			     "ffd9");

	    assert_parses(v,
			  {{0xd8, h("")},
			   {0xe0, h("69")},
			   {0xe1, h("00112233")},
			   {0xd9, {}}});
	}

	void padding(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "ffffff"
			     "ffe0 0003 69");

	    assert_parses(v,
			  {{0xd8, h("")},
			   {0xe0, h("69")}});
	}

	void empty_segment(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "ffe0 0002"
			     "ffd9");

	    assert_parses(v,
			  {{0xd8, h("")},
			   {0xe0, h("")},
			   {0xd9, h("")}});
	}

	void ff00(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "ffe0 0003 69"
			     "ff00ff00"
			     "0123456789abcdef"
			     "ff00");

	    assert_parses(v,
			  {{0xd8, h("")},
			   {0xe0, h("69")}});
	}

	void trailing_eoi(orchis::TC)
	{
	    const auto v = h("ffd8"
			     "ffe0 0003 69"
			     "ffd9"
			     "ffe0 0042 69"
			     "ffd9");

	    assert_parses(v,
			  {{0xd8, h("")},
			   {0xe0, h("69")},
			   {0xd9, h("")}});
	}
    }

    namespace bad {

	void empty(orchis::TC)
	{
	    try {
		parse(nullptr, nullptr, 0);
	    }
	    catch (const Decoder::Error&) {
		return;
	    }
	    throw orchis::Failure{};
	}

	void assert_fail(const std::vector<uint8_t>& v)
	{
	    const auto a = v.data();
	    const auto b = a + v.size();

	    for(size_t n = v.size(); n; n--) {
		try {
		    const auto v = parse(a, b, n);
		}
		catch (const Decoder::Error&) {
		    continue;
		}
		throw orchis::Failure{};
	    }
	}

	void length(orchis::TC)
	{
	    auto v = h("ffd8"
		       "ffe0 0001");
	    assert_fail(v);

	    v = h("ffd8"
		  "ffe0 0000");
	    assert_fail(v);
	}

	void start(orchis::TC)
	{
	    auto v = h("ff00 1234"
		       "ffd8"
		       "ffe0 0002");
	    assert_fail(v);
	}

	namespace truncated {

	    void a(orchis::TC)
	    {
		const auto v = h("ffd8"
				 "ffe0 0004 4711"
				 "ffe0");
		assert_fail(v);
	    }

	    void b(orchis::TC)
	    {
		const auto v = h("ffd8"
				 "ffe0 0004 4711"
				 "ffe0 00");
		assert_fail(v);
	    }

	    void c(orchis::TC)
	    {
		const auto v = h("ffd8"
				 "ffe0 0004 4711"
				 "ffe0 0004");
		assert_fail(v);
	    }

	    void d(orchis::TC)
	    {
		const auto v = h("ffd8"
				 "ffe0 0004 4711"
				 "ffe0 0004 69");
		assert_fail(v);
	    }

	    void e(orchis::TC)
	    {
		const auto v = h("ffd8"
				 "ffe0 0004 4711"
				 "ffe0 0005 6970");
		assert_fail(v);
	    }
	}
    }
}
