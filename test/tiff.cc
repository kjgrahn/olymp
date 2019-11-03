#include <vector>
#include <orchis.h>
#include "hexread.h"

#include <tiff/tiff.h>

namespace {

    using orchis::assert_eq;
    using orchis::assert_true;
    using orchis::assert_false;

    std::vector<uint8_t> h(const std::string& s) { return hexread(s); }
}

namespace tiff {

    namespace intel {

	const auto data = h(
	    "45 78 69 66 00 00"   // Exif
	    "4949 2a00 0800 0000" // TIFF header
	    "1b00"                        // IFD 0 with 27 tags
	    ""
	    "0100 0100 00000000 ffffffff" // Byte[0]
	    "0200 0100 01000000 01ffffff" // Byte[1]
	    "0300 0100 04000000 01020304" // Byte[4]
	    "0400 0100 05000000 52010000" // Byte[5]
	    ""
	    "0101 0200 00000000 ffffffff" // Ascii[0]
	    "0201 0200 01000000 00ffffff" // Ascii[1] ""
	    "0301 0200 04000000 666f6f00" // Ascii[4] "foo"
	    "0401 0200 06000000 58010000" // Ascii[6] "fobar"
	    ""
	    "0102 0300 00000000 ffffffff" // Short[0]
	    "0202 0300 01000000 1147ffff" // Short[1]
	    "0302 0300 02000000 666f6f00" // Short[2]
	    "0402 0300 05000000 5e010000" // Short[5]
	    ""
	    "0103 0400 00000000 ffffffff" // Long[0]
	    "0203 0400 01000000 abadcafe" // Long[1]
	    "0303 0400 03000000 68010000" // Long[3]
	    ""
	    "0104 0500 00000000 ffffffff" // Rational[0]
	    "0204 0500 01000000 74010000" // Rational[1]
	    "0304 0500 03000000 74010000" // Rational[3]
	    ""
	    "0105 0600 00000000 ffffffff" // SByte[0]
	    "0205 0600 01000000 01ffffff" // SByte[1]
	    "0305 0600 04000000 01020304" // SByte[4]
	    "0405 0600 05000000 52010000" // SByte[5]
	    ""
	    "0106 0700 00000000 ffffffff" // Undefined[0]
	    "0206 0700 01000000 01ffffff" // Undefined[1]
	    "0306 0700 04000000 01020304" // Undefined[4]
	    "0406 0700 05000000 52010000" // Undefined[5]
	    ""
	    "0107 0d00 01000000 ffffffff" // unknown type
	    "00000000"                    // no next IFD
	    ""
	    "deadf00d69 ff"               // 0x0152
	    "666f626172 00"               // 0x0158
	    "1147 1247 1347 1447 1547"    // 0x015e
	    "deadf00d feedc0ed babeabad"  // 0x0168
	    "0100 0000 0100 0000"         // 0x0174
	    "0100 0000 0200 0000"
	    "0100 0000 0300 0000"
	    ""                            // 0x018c
	    "");
    }

    using namespace tiff::type;

    namespace failure {

	void not_found(const std::vector<uint8_t>& data)
	{
	    const File f {data};
	    assert_eq(f.ifd0.find<Ascii>(0x100), "");
	}

	void type(const std::vector<uint8_t>& data)
	{
	    const File f {data};
	    assert_eq(f.ifd0.find<Ascii>(0x002), "");
	    assert_eq(f.ifd0.find<Ascii>(0x405), "");
	}
    }

    void bytev(const std::vector<uint8_t>& data)
    {
	using v = Byte::array_type;
	const File f {data};
	assert_true(f.ifd0.find<Byte>(0x001) == v{});
	assert_true(f.ifd0.find<Byte>(0x002) == v{1});
	assert_true(f.ifd0.find<Byte>(0x003) == v{1, 2, 3, 4});
	assert_true(f.ifd0.find<Byte>(0x004) == v{0xde, 0xad, 0xf0,
						  0x0d, 0x69});
    }

    void ascii(const std::vector<uint8_t>& data)
    {
	const File f {data};
	assert_eq(f.ifd0.find<Ascii>(0x101), "");
	assert_eq(f.ifd0.find<Ascii>(0x102), "");
	assert_eq(f.ifd0.find<Ascii>(0x103), "foo");
	assert_eq(f.ifd0.find<Ascii>(0x104), "fobar");
    }

    void shortv(const std::vector<uint8_t>& data)
    {
	using v = Short::array_type;
	const File f {data};
	assert_true(f.ifd0.find<Short>(0x201) == v{});
	assert_true(f.ifd0.find<Short>(0x202) == v{0x4711});
	assert_true(f.ifd0.find<Short>(0x203) == v{0x6f66, 0x6f});
	assert_true(f.ifd0.find<Short>(0x204) == v{0x4711, 0x4712, 0x4713,
						   0x4714, 0x4715});
    }

    void longv(const std::vector<uint8_t>& data)
    {
	using v = Long::array_type;
	const File f {data};
	assert_true(f.ifd0.find<Long>(0x301) == v{});
	assert_true(f.ifd0.find<Long>(0x302) == v{0xfecaadab});
	assert_true(f.ifd0.find<Long>(0x303) == v{0x0df0adde,
						  0xedc0edfe,
						  0xadabbeba});
    }

    void ratv(const std::vector<uint8_t>& data)
    {
	using v = Rational::array_type;
	const File f {data};
	assert_true(f.ifd0.find<Rational>(0x401) == v{});
	assert_true(f.ifd0.find<Rational>(0x402) == v{{1, 1}});
	assert_true(f.ifd0.find<Rational>(0x403) == v{{1, 1},
						      {1, 2},
						      {1, 3}});
    }

    void undefinedv(const std::vector<uint8_t>& data)
    {
	using v = Undefined::array_type;
	const File f {data};
	assert_true(f.ifd0.find<Undefined>(0x601) == v{});
	assert_true(f.ifd0.find<Undefined>(0x602) == v{1});
	assert_true(f.ifd0.find<Undefined>(0x603) == v{1, 2, 3, 4});
	assert_true(f.ifd0.find<Undefined>(0x604) == v{0xde, 0xad, 0xf0,
						       0x0d, 0x69});
    }

    namespace optional {

	template <class OT>
	void assert_empty(const OT& val)
	{
	    orchis::assert_false(val.has_value());
	}

	void byte_array(const std::vector<uint8_t>& data)
	{
	    const File f {data};
	    // no such tag
	    assert_empty(find<Byte, 1>(f.ifd0, 0x100));
	    // wrong type (short)
	    assert_empty(find<Byte, 1>(f.ifd0, 0x201));
	    // wrong number of elements
	    assert_empty(find<Byte, 1>(f.ifd0, 0x001));
	    assert_empty(find<Byte, 1>(f.ifd0, 0x003));
	    assert_empty(find<Byte, 1>(f.ifd0, 0x004));

	    auto b1 = find<Byte, 1>(f.ifd0, 0x002);
	    orchis::assert_true(b1.has_value());
	    orchis::assert_true(*b1 == std::array<uint8_t, 1>{1});

	    auto b5 = find<Byte, 5>(f.ifd0, 0x004);
	    orchis::assert_true(b5.has_value());
	    orchis::assert_true(*b5 == std::array<uint8_t, 5>{0xde, 0xad, 0xf0,
							      0x0d, 0x69});
	}

	void one_long(const std::vector<uint8_t>& data)
	{
	    const File f {data};
	    // no such tag
	    assert_empty(find<Long>(f.ifd0, 0x100));
	    // wrong type (short)
	    assert_empty(find<Long>(f.ifd0, 0x201));
	    // wrong number of elements
	    assert_empty(find<Long>(f.ifd0, 0x301));
	    assert_empty(find<Long>(f.ifd0, 0x303));

	    auto L1 = find<Long>(f.ifd0, 0x302);
	    orchis::assert_true(L1.has_value());
	    orchis::assert_eq(*L1, 0xfecaadab);
	}
    }

    namespace intel {

	using orchis::TC;

	namespace failure {
	    void not_found(TC)	{ tiff::failure::not_found(data); }
	    void type(TC)	{ tiff::failure::type(data); }
	}
	void bytev(TC)		{ tiff::bytev(data); }
	void ascii(TC)		{ tiff::ascii(data); }
	void shortv(TC)		{ tiff::shortv(data); }
	void longv(TC)		{ tiff::longv(data); }
	void ratv(TC)		{ tiff::ratv(data); }
	void undefinedv(TC)	{ tiff::undefinedv(data); }
	namespace optional	{
	    void byte_array(TC) { tiff::optional::byte_array(data); }
	    void one_long(TC)	{ tiff::optional::one_long(data); }
	}
    }

    namespace motorola {

	const auto data = h(
	    "45 78 69 66 00 00"   // Exif
	    "4d4d 002a 0000 0008" // TIFF header
	    "001b"                        // IFD 0 with 27 tags
	    ""
	    "0001 0001 00000000 ffffffff" // Byte[0]
	    "0002 0001 00000001 01ffffff" // Byte[1]
	    "0003 0001 00000004 01020304" // Byte[4]
	    "0004 0001 00000005 00000152" // Byte[5]
	    ""
	    "0101 0002 00000000 ffffffff" // Ascii[0]
	    "0102 0002 00000001 00ffffff" // Ascii[1] ""
	    "0103 0002 00000004 666f6f00" // Ascii[4] "foo"
	    "0104 0002 00000006 00000158" // Ascii[6] "fobar"
	    ""
	    "0201 0003 00000000 ffffffff" // Short[0]
	    "0202 0003 00000001 4711ffff" // Short[1]
	    "0203 0003 00000002 6f66006f" // Short[2]
	    "0204 0003 00000005 0000015e" // Short[5]
	    ""
	    "0301 0004 00000000 ffffffff" // Long[0]
	    "0302 0004 00000001 fecaadab" // Long[1]
	    "0303 0004 00000003 00000168" // Long[3]
	    ""
	    "0401 0005 00000000 ffffffff" // Rational[0]
	    "0402 0005 00000001 00000174" // Rational[1]
	    "0403 0005 00000003 00000174" // Rational[3]
	    ""
	    "0501 0006 00000000 ffffffff" // SByte[0]
	    "0502 0006 00000001 01ffffff" // SByte[1]
	    "0503 0006 00000004 01020304" // SByte[4]
	    "0504 0006 00000005 00000152" // SByte[5]
	    ""
	    "0601 0007 00000000 ffffffff" // Undefined[0]
	    "0602 0007 00000001 01ffffff" // Undefined[1]
	    "0603 0007 00000004 01020304" // Undefined[4]
	    "0604 0007 00000005 00000152" // Undefined[5]
	    ""
	    "0701 000d 00000001 ffffffff" // unknown type
	    "00000000"                    // no next IFD
	    ""
	    "deadf00d69 ff"               // 0x0152
	    "666f626172 00"               // 0x0158
	    "4711 4712 4713 4714 4715"    // 0x015e
	    "0df0adde edc0edfe adabbeba"  // 0x0168
	    "0000 0001 0000 0001"         // 0x0174
	    "0000 0001 0000 0002"
	    "0000 0001 0000 0003"
	    ""                            // 0x018c
	    "");

	using orchis::TC;

	namespace failure {
	    void not_found(TC)	{ tiff::failure::not_found(data); }
	    void type(TC)	{ tiff::failure::type(data); }
	}
	void bytev(TC)		{ tiff::bytev(data); }
	void ascii(TC)		{ tiff::ascii(data); }
	void shortv(TC)		{ tiff::shortv(data); }
	void longv(TC)		{ tiff::longv(data); }
	void ratv(TC)		{ tiff::ratv(data); }
	void undefinedv(TC)	{ tiff::undefinedv(data); }
	namespace optional	{
	    void byte_array(TC) { tiff::optional::byte_array(data); }
	    void one_long(TC)	{ tiff::optional::one_long(data); }
	}
    }

    namespace broken {

	using orchis::TC;
	const std::string exif {"45 78 69 66 00 00"};

	void assert_empty(const char* s)
	{
	    const File f {h(exif + s)};
	    orchis::assert_true(f.ifd0.empty());
	    orchis::assert_true(f.exif.empty());
	    orchis::assert_true(f.gps.empty());
	}

	template <class Err>
	void assert_throws(const char* s)
	{
	    try {
		const File f {h(exif + s)};
	    }
	    catch (const Err&) {
		return;
	    }
	    catch (...) {
		throw orchis::Failure {"threw the wrong exception"};
	    }
	    throw orchis::Failure {"should have thrown"};
	}

	void empty(TC)
	{
	    assert_empty("4d4d 002a 00000008 0000 00000000");
	    assert_empty("4949 2a00 08000000 0000 00000000");
	}

	void segfault(TC)
	{
	    assert_throws<Segfault>("4949 2a00");
	    assert_throws<Segfault>("4949 2a");
	    assert_throws<Segfault>("4949");
	    assert_throws<Segfault>("49");
	    assert_throws<Segfault>("");

	    assert_throws<Segfault>("4d4d 002a");
	    assert_throws<Segfault>("4d4d 00");
	    assert_throws<Segfault>("4d4d");
	    assert_throws<Segfault>("4d");
	    assert_throws<Segfault>("");

	    assert_throws<Segfault>("4949 2a00 09000000 0000 00000000");
	    assert_throws<Segfault>("4949 2a00 08000000 0000 000000");
	    assert_throws<Segfault>("4949 2a00 08000000 0100 00000000");

	    assert_throws<Segfault>("4d4d 002a 00000009 0000 00000000");
	    assert_throws<Segfault>("4d4d 002a 00000008 0000 000000");
	    assert_throws<Segfault>("4d4d 002a 00000008 0001 00000000");
	}

	void error(TC)
	{
	    assert_throws<Error>("4949 2b00 08000000 0000 00000000");
	    assert_throws<Error>("4949 002a 08000000 0000 00000000");
	    assert_throws<Error>("4900 2a00 08000000 0000 00000000");
	    assert_throws<Error>("0049 2a00 08000000 0000 00000000");
	    assert_throws<Error>("0000 2a00 08000000 0000 00000000");

	    assert_throws<Error>("4d4d 002b 00000008 0000 00000000");
	    assert_throws<Error>("4d4d 2a00 00000008 0000 00000000");
	    assert_throws<Error>("4d00 002a 00000008 0000 00000000");
	    assert_throws<Error>("004d 002a 00000008 0000 00000000");
	    assert_throws<Error>("0000 002a 00000008 0000 00000000");
	}
    }
}
