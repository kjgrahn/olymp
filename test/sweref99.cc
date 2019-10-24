#include <orchis.h>

#include <sweref99.h>

namespace sweref99 {

    void assert_fmts(const Coordinate& c,
		     const char* ref)
    {
	std::ostringstream oss;
	oss << c;
	orchis::assert_eq(oss.str(), ref);
    }

    void assert_invalid(const Coordinate& c)
    {
	orchis::assert_false(c.valid());
    }

    void valid(orchis::TC)
    {
	assert_fmts(Coordinate{7281446,   478656},   "7281446 478656");
	assert_fmts(Coordinate{7281446.4, 478656.4}, "7281446 478656");
	assert_fmts(Coordinate{7281445.6, 478655.6}, "7281446 478656");
    }

    void invalid(orchis::TC)
    {
	assert_invalid(Coordinate{0, 0});
	assert_invalid(Coordinate{478656, 7281446});
    }
}
