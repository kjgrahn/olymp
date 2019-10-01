#include <vector>
#include <orchis.h>
#include "hexread.h"

#include <tiff/tiff.h>

namespace {

    using orchis::assert_eq;
    using orchis::assert_true;
    using orchis::assert_false;

    std::vector<uint8_t> h(const std::string& s)
    {
	std::vector<uint8_t> v(s.size()/2);
	const char* a = s.data();
	auto b = s.data() + s.size();
	auto n = hexread(v.data(), &a, b);
	assert_eq(a, b);
	v.resize(n);
	return v;
    }
}

namespace tiff {

    namespace simple {

	namespace data {
	    const auto bigendian = h(
		"45 78 69 66 00 00" // Exif
		"4d4d 002a 0000 0008" // TIFF header
		"0001"              // IFD 0 with one tag
		"8769 0004 00000001 0000 001a" // Exif IFD pointer
		"0000 0000"         // no next IFD
		""
		"0001"              // Exif IFD with one tag
		"9003 0002 00000014 0000 002c" // DateTimeOriginal
		"0000 0000"         // no next IFD
		"32 30 31 39 3a 30 39 3a 31 32 20 32 32 3a 33 30 3a 30 30 00");

	    const auto lendian = h(
		"45 78 69 66 00 00" // Exif
		"4949 2a00 0800 0000" // TIFF header
		"0100"              // IFD 0 with one tag
		"6987 0400 01000000 1a00 0000" // Exif IFD pointer
		"0000 0000"         // no next IFD
		""
		"0100"              // Exif IFD with one tag
		"0390 0200 14000000 2c00 0000" // DateTimeOriginal
		"0000 0000"         // no next IFD
		"32 30 31 39 3a 30 39 3a 31 32 20 32 32 3a 33 30 3a 30 30 00");
	}

	void bigendian()
	{
	    File f {data::bigendian};
	}

	void lendian(orchis::TC)
	{
	    File f {data::lendian};

	    assert_false(f.ifd0.empty());
	    auto exif = f.ifd0.find<type::Long>(0x8769);
	    assert_eq(exif.size(), 1);
	    assert_eq(exif[0], 0x1a);

	    assert_false(f.exif.empty());
	    auto date = f.exif.find<type::Ascii>(0x9003);
	    assert_eq(date, "2019:09:12 22:30:00");
	}
    }

    namespace tags {

	using namespace tiff::type;

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
	    "0401 0200 06000000 58010000" // Ascii[5] "fobar"
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

	namespace failure {

	    void not_found(orchis::TC)
	    {
		const File f {data};
		assert_eq(f.ifd0.find<Ascii>(0x100), "");
	    }

	    void type(orchis::TC)
	    {
		const File f {data};
		assert_eq(f.ifd0.find<Ascii>(0x002), "");
		assert_eq(f.ifd0.find<Ascii>(0x405), "");
	    }
	}

	void bytev(orchis::TC)
	{
	    using v = Byte::array_type;
	    const File f {data};
	    assert_true(f.ifd0.find<Byte>(0x001) == v{});
	    assert_true(f.ifd0.find<Byte>(0x002) == v{1});
	    assert_true(f.ifd0.find<Byte>(0x003) == v{1, 2, 3, 4});
	    assert_true(f.ifd0.find<Byte>(0x004) == v{0xde, 0xad, 0xf0,
						      0x0d, 0x69});
	}

	void ascii(orchis::TC)
	{
	    const File f {data};
	    assert_eq(f.ifd0.find<Ascii>(0x101), "");
	    assert_eq(f.ifd0.find<Ascii>(0x102), "");
	    assert_eq(f.ifd0.find<Ascii>(0x103), "foo");
	    assert_eq(f.ifd0.find<Ascii>(0x104), "fobar");
	}

	void shortv(orchis::TC)
	{
	    using v = Short::array_type;
	    const File f {data};
	    assert_true(f.ifd0.find<Short>(0x201) == v{});
	    assert_true(f.ifd0.find<Short>(0x202) == v{0x4711});
	    assert_true(f.ifd0.find<Short>(0x203) == v{0x6f66, 0x6f});
	    assert_true(f.ifd0.find<Short>(0x204) == v{0x4711, 0x4712, 0x4713,
						       0x4714, 0x4715});
	}

	void longv(orchis::TC)
	{
	    using v = Long::array_type;
	    const File f {data};
	    assert_true(f.ifd0.find<Long>(0x301) == v{});
	    assert_true(f.ifd0.find<Long>(0x302) == v{0xfecaadab});
	    assert_true(f.ifd0.find<Long>(0x303) == v{0x0df0adde,
						      0xedc0edfe,
						      0xadabbeba});
	}

	void ratv(orchis::TC)
	{
	    using v = Rational::array_type;
	    const File f {data};
	    assert_true(f.ifd0.find<Rational>(0x401) == v{});
	    assert_true(f.ifd0.find<Rational>(0x402) == v{{1, 1}});
	    assert_true(f.ifd0.find<Rational>(0x403) == v{{1, 1},
							  {1, 2},
							  {1, 3}});
	}

	void undefinedv(orchis::TC)
	{
	    using v = Undefined::array_type;
	    const File f {data};
	    assert_true(f.ifd0.find<Undefined>(0x601) == v{});
	    assert_true(f.ifd0.find<Undefined>(0x602) == v{1});
	    assert_true(f.ifd0.find<Undefined>(0x603) == v{1, 2, 3, 4});
	    assert_true(f.ifd0.find<Undefined>(0x604) == v{0xde, 0xad, 0xf0,
							   0x0d, 0x69});
	}
    }
}
