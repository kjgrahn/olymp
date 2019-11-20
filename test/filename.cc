#include <orchis.h>

#include <filename.h>

namespace filename {

    using orchis::TC;
    using orchis::assert_eq;

    const Serial s1234{1234};

    void olympus(TC)
    {
	assert_eq(serial("pa051234.jpg"), s1234);
	assert_eq(serial("PA051234.JPG"), s1234);
    }

    void dsc(TC)
    {
	assert_eq(serial("dsc01234.jpg"), s1234);
	assert_eq(serial("DSC01234.JPG"), s1234);
    }

    void dscn(TC)
    {
	assert_eq(serial("dscn1234.jpg"), s1234);
	assert_eq(serial("DSCN1234.JPG"), s1234);
    }

    void imag(TC)
    {
	assert_eq(serial("imag1234.jpg"), s1234);
	assert_eq(serial("IMAG1234.JPG"), s1234);
    }

    void self(TC)
    {
	assert_eq(serial("1990-01-02_1234.jpg"), s1234);
    }

    void freeform(TC)
    {
	assert_eq(serial("1234"),  s1234);
	assert_eq(serial("1234a"), s1234);
	assert_eq(serial("a1234"), s1234);

	assert_eq(serial("78a1234"), s1234);
	assert_eq(serial("7891234"), s1234);

	assert_eq(serial("78a12"), Serial{12});
    }

    void missing(TC)
    {
	assert_eq(serial("foo"), Serial{0});
	assert_eq(serial(""), Serial{0});
    }

    void path(TC)
    {
	assert_eq(s1234, serial("pa051234.jpg"));
	assert_eq(s1234, serial("foo/pa051234.jpg"));
	assert_eq(s1234, serial("foo/bar/pa051234.jpg"));
	assert_eq(s1234, serial("./pa051234.jpg"));
	assert_eq(s1234, serial(".//pa051234.jpg"));
	assert_eq(s1234, serial("/pa051234.jpg"));
    }
}
