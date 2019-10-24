#include <orchis.h>

#include <transform.h>

namespace transform {

    template <class T>
    std::string format(const T& val)
    {
	std::ostringstream oss;
	oss << val;
	return oss.str();
    }

    void assert_fmts(const sweref99::Coordinate& c,
		     const char* ref)
    {
	orchis::assert_true(c.valid());
	orchis::assert_eq(format(c), ref);
    }

    void simple(orchis::TC)
    {
	const Transform t;
	const wgs84::Coordinate c {65.6542, 14.5360};
	assert_fmts(t(c), "7281446 478656");
    }

    void lmv(orchis::TC)
    {
	const Transform t;

	auto wgs = [] (double a, double b,
		       double c, double d) {
		       return wgs84::Coordinate {
			   a + b/60,
			   c + d/60 };
		   };

	/* Lantmäteriet Informationsförsörjning Geodesi,
	 * "Kontrollpunkter för SWEREF 99 TM", 2007-11-20.
	 *
	 * It's really a mapping for some selected points from SWEREF
	 * 99 to SWEREF 99 TM, not from WGS 84, but I suppose it works
	 * as a test for that latter case too, when you perform the
	 * test with 1 m resolution.
	 *
	 * I had to leave out the two first reference points since
	 * they lie outside the area sweref99::Coordinate counts as
	 * valid (they are in the sea to the south of Sweden):
	 *
	 *    55° 00' N 12° 45' E   6097106.672 356083.438
	 *    55° 00' N 14° 15' E   6095048.642 452024.069
	 */
	const struct {
	    wgs84::Coordinate a;
	    sweref99::Coordinate ref;
	} points[] = {
	    {wgs(57, 00, 12, 45), {6319636.937, 363331.554}},
	    {wgs(57, 00, 19, 30), {6326392.707, 773251.054}},
	    {wgs(59, 00, 11, 15), {6546096.724, 284626.066}},
	    {wgs(59, 00, 19, 30), {6548757.206, 758410.519}},
	    {wgs(61, 00, 12, 45), {6764877.311, 378323.440}},
	    {wgs(61, 00, 18, 45), {6768593.345, 702745.127}},
	    {wgs(63, 00, 12, 00), {6989134.048, 348083.148}},
	    {wgs(63, 00, 19, 30), {6993565.630, 727798.671}},
	    {wgs(65, 00, 13, 30), {7209293.753, 429270.201}},
	    {wgs(65, 00, 21, 45), {7225449.115, 817833.405}},
	    {wgs(67, 00, 16, 30), {7432168.174, 565398.458}},
	    {wgs(67, 00, 24, 00), {7459745.672, 891298.142}},
	    {wgs(69, 00, 21, 00), {7666089.698, 739639.195}},
	};

	for (const auto p : points) {
	    const auto a = t(p.a);
	    orchis::assert_eq(format(a), format(p.ref));
	}
    }
}
