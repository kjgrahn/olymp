#include <array>
#include <orchis.h>

#include <tiff/endian.h>


namespace endianness {

    void big(orchis::TC)
    {
	std::array<uint8_t, 4> v { 0x11, 0x22, 0x33, 0x44 };
	auto a = begin(v);
	orchis::assert_eq(be::eat32(a), 0x11223344);
	orchis::assert_eq(a, end(v));

	auto b = begin(v);
	orchis::assert_eq(be::eat16(b), 0x1122);
	orchis::assert_eq(be::eat16(b), 0x3344);
	orchis::assert_eq(b, end(v));
    }

    void little(orchis::TC)
    {
	std::array<uint8_t, 4> v { 0x11, 0x22, 0x33, 0x44 };
	auto a = begin(v);
	orchis::assert_eq(le::eat32(a), 0x44332211);
	orchis::assert_eq(a, end(v));

	auto b = begin(v);
	orchis::assert_eq(le::eat16(b), 0x2211);
	orchis::assert_eq(le::eat16(b), 0x4433);
	orchis::assert_eq(b, end(v));
    }
}
