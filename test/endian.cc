#include <array>
#include <orchis.h>

#include <tiff/endian.h>


namespace endianness {

    void big(orchis::TC)
    {
	const tiff::Motorola be;

	std::array<uint8_t, 4> v { 0x11, 0x22, 0x33, 0x44 };
	const uint8_t* a = v.data();
	const auto e = a + v.size();
	orchis::assert_eq(be.eat32(a), 0x11223344);
	orchis::assert_eq(a, e);

	const uint8_t* b = v.data();
	orchis::assert_eq(be.eat16(b), 0x1122);
	orchis::assert_eq(be.eat16(b), 0x3344);
	orchis::assert_eq(b, e);
    }

    void little(orchis::TC)
    {
	const tiff::Intel le;

	std::array<uint8_t, 4> v { 0x11, 0x22, 0x33, 0x44 };
	const uint8_t* a = v.data();
	const auto e = a + v.size();
	orchis::assert_eq(le.eat32(a), 0x44332211);
	orchis::assert_eq(a, e);

	const uint8_t* b = v.data();
	orchis::assert_eq(le.eat16(b), 0x2211);
	orchis::assert_eq(le.eat16(b), 0x4433);
	orchis::assert_eq(b, e);
    }
}
