#include <orchis.h>

#include <filename.h>

namespace {
    bool operator== (const Serial&a, int b) { return a == Serial(b); }
    bool operator== (int a, const Serial& b) { return b==a; }
}

namespace filename {

    using orchis::TC;
    using orchis::assert_eq;

    void olympus(TC)
    {
	assert_eq(serial("pa051234.jpg"), 1234);
	assert_eq(serial("PA051234.JPG"), 1234);
    }

    void dsc(TC)
    {
	assert_eq(serial("dsc01234.jpg"), 1234);
	assert_eq(serial("DSC01234.JPG"), 1234);
    }

    void dscn(TC)
    {
	assert_eq(serial("dscn1234.jpg"), 1234);
	assert_eq(serial("DSCN1234.JPG"), 1234);
    }

    void imag(TC)
    {
	assert_eq(serial("imag1234.jpg"), 1234);
	assert_eq(serial("IMAG1234.JPG"), 1234);
    }

    void self(TC)
    {
	assert_eq(serial("1990-01-02_1234.jpg"), 1234);
    }

    void path(TC)
    {
	assert_eq(1234, serial("pa051234.jpg"));
	assert_eq(1234, serial("foo/pa051234.jpg"));
	assert_eq(1234, serial("foo/bar/pa051234.jpg"));
	assert_eq(1234, serial("./pa051234.jpg"));
	assert_eq(1234, serial(".//pa051234.jpg"));
	assert_eq(1234, serial("/pa051234.jpg"));
    }
}
