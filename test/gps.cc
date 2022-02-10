#include <orchis.h>
#include "hexread.h"

#include <gps.h>
#include <wgs84.h>

namespace data {
    const auto exif = hexread(
	"45 78 69 66 00 00" // Exif
	"4949 2a00 0800 0000" // TIFF header
	"0100"              // IFD 0 with one tag       08
	"2588 0400 01000000 1a00 0000" // GPS IFD pointer
	"0000 0000"         // no next IFD
	""
	"0500"              //                          1a
	"0100 0200 02000000 4e00 0000" // GPSLatitudeRef N
	"0200 0500 03000000 6400 0000" // GPSLatitude
	"0300 0200 02000000 4500 0000" // GPSLongitudeRef E
	"0400 0500 03000000 7c00 0000" // GPSLongitude
	"1200 0200 07000000 5c00 0000" // GPSMapDatum
	"0000 0000"                    // no next IFD
	"5747 532d 3834 0000"          // WGS-84        5c
	"3a00 0000 0100 0000"          // 58 (lat)      64
	"0c00 0000 0100 0000"          // 12
	"c002 0000 6400 0000"          // 7.04
	"0d00 0000 0100 0000"          // 13 (long)     7c
	"1c00 0000 0100 0000"          // 28
	"6216 0000 6400 0000"          // 57.30
	"");

    const auto p101608 = hexread(
	"4578 6966 0000"
	"4949 2a00 0800 0000"
	"0100"
	"2588 0400 01000000 1a00 0000"
	"0000 0000"
	""
	"0900"
	"0000 0100 04000000 0202 0000"
	"0100 0200 02000000 4e00 0000"
	"0200 0500 03000000 5001 0000"
	"0300 0200 02000000 4500 0000"
	"0400 0500 03000000 6801 0000"
	"0500 0100 01000000 0000 0000"
	"0600 0500 01000000 8001 0000"
	"0700 0500 03000000 9401 0000"
	"1d00 0200 0b000000 8801 0000"
	"0000 0000"
	"5a00 0000 4000 0400 0100 0000 0000 0000"
	"5000 0400 0100 0000 0100 0000 0001 0300"
	"0100 0000 0000 0000 0000 0000 0000 0000"
	"0000 0000 0200 0100 0200 0400 0000 5239"
	"3800 0200 0700 0400 0000 3031 3030 0000"
	"0000 0900 0000 0100 0400 0000 0202 0000"
	"0100 0200 0200 0000 4e00 0000 0200 0500"
	"0300 0000 2804 0000 0300 0200 0200 0000"
	"4500 0000 0400 0500 0300 0000 4004 0000"
	"0500 0100 0100 0000 0000 0000 0600 0500"
	"0100 0000 5804 0000 0700 0500 0300 0000"
	"6c04 0000 1d00 0200 0b00 0000 6004 0000"
	"0000 0000 3900 0000 0100 0000 2800 0000"
	"0100 0000 1e00 0000 0100 0000 0c00 0000"
	"0100 0000 0100 0000 0100 0000 1000 0000"
	"0100 0000 8f00 0000 0100 0000 3230 3230"
	"3a30 363a 3232 0000 0800 0000 0100 0000"
	"0f00 0000 0100 0000 2c00 0000 0100 0000"
	"");

    const auto p2482 = hexread(
	"4578 6966 0000"
	"4949 2a00 0800 0000"
	"0100"
	"2588 0400 01000000 1a00 0000"
	"0000 0000"
	""
	"1400"
	"0000 0100 04000000 0203 0000"
	"0100 0200 02000000 4e00 0000"
	"0200 0500 03000000 1c01 0000"
	"0300 0200 02000000 4500 0000"
	"0400 0500 03000000 3401 0000"
	"0500 0100 01000000 0000 0000"
	"0600 0500 01000000 4c01 0000"
	"0700 0500 03000000 5401 0000"
	"0900 0200 02000000 4100 0000"
	"0a00 0200 02000000 3300 0000"
	"0b00 0500 01000000 6c01 0000"
	"0c00 0200 02000000 4b00 0000"
	"0d00 0500 01000000 7401 0000"
	"0e00 0200 02000000 5400 0000"
	"0f00 0500 01000000 7c01 0000"
	"1000 0200 02000000 4d00 0000"
	"1100 0500 01000000 8401 0000"
	"1200 0200 07000000 8c01 0000"
	"1d00 0200 0b000000 9401 0000"
	"1e00 0300 01000000 0000 0000"
	"0000 0000"
	"0100 0000 0000 0000 0000 0000 4100 0000"
	"0100 0000 2700 0000 0100 0000 223b 0000"
	"e803 0000 0e00 0000 0100 0000 2000 0000"
	"0100 0000 e924 0000 e803 0000 df23 0000"
	"0a00 0000 0400 0000 0100 0000 2100 0000"
	"0100 0000 384a 0000 e803 0000 942a 0000"
	"1027 0000 ac02 0000 e803 0000 dd3a 0000"
	"6400 0000 b04f 0000 6400 0000 5747 532d"
	"3834 0000 3230 3138 3a30 373a 3234 0000"
	"");

    const auto p2120 = hexread(
	"4578 6966 0000"
	"4949 2a00 0800 0000"
	"0100"
	"2588 0400 01000000 1a00 0000"
	"0000 0000"
	""
	"0b00"
	"0000 0100 04000000 0203 0000"
	"0100 0200 02000000 4e00 0000"
	"0200 0500 03000000 c600 0000"
	"0300 0200 02000000 4500 0000"
	"0400 0500 03000000 de00 0000"
	"0500 0100 01000000 0000 0000"
	"0600 0500 01000000 f600 0000"
	"0700 0500 03000000 fe00 0000"
	"0800 0200 04000000 3133 2000"
	"1200 0200 06000000 1601 0000"
	"1d00 0200 0b000000 1c01 0000"
	"0000 0000"
	"0000 3133 2000 1200 0200 0600 0000 6604"
	"0000 1d00 0200 0b00 0000 6c04 0000 0000"
	"0000 3f00 0000 0100 0000 3b00 0000 0100"
	"0000 2292 0000 e803 0000 1300 0000 0100"
	"0000 2900 0000 0100 0000 d66b 0000 e803"
	"0000 3836 0000 6400 0000 0e00 0000 0100"
	"0000 2700 0000 0100 0000 5898 0000 e803"
	"0000 5747 5338 3400 3230 3232 3a30 323a"
	"3039 0000 0600 0301 0300 0100 0000 0600"
	"");
}

namespace gps {

    using orchis::assert_eq;
    using orchis::assert_true;
    using orchis::assert_false;

    const tiff::File file{data::exif};

    void lat_ref(orchis::TC)
    {
	assert_eq(LatitudeRef{file}.val, "N");
    }

    void long_ref(orchis::TC)
    {
	assert_eq(LongitudeRef{file}.val, "E");
    }

    void map_datum(orchis::TC)
    {
	assert_eq(MapDatum{file}.val, "WGS-84");
    }

    template <class C>
    void assert_triplet(const C& coord,
			unsigned a, unsigned b,
			unsigned c, unsigned d,
			unsigned e, unsigned f)
    {
	assert_true(coord.val.has_value());
	const auto& v = *coord.val;
	assert_eq(v.size(), 3);
	assert_eq(v[0].first, a); assert_eq(v[0].second, b);
	assert_eq(v[1].first, c); assert_eq(v[1].second, d);
	assert_eq(v[2].first, e); assert_eq(v[2].second, f);
    }

    template <class C>
    void assert_rational(const C& val,
			 unsigned a, unsigned b)
    {
	assert_true(val.val.has_value());
	const auto& v = *val.val;
	assert_eq(v.first, a);
	assert_eq(v.second, b);
    }

    void latitude(orchis::TC)
    {
	assert_triplet(Latitude{file},
		       58, 1,
		       12, 1,
		       704, 100);
    }

    void longitude(orchis::TC)
    {
	assert_triplet(Longitude{file},
		       13, 1,
		       28, 1,
		       5730, 100);
    }

    void missing(orchis::TC)
    {
	assert_false(TimeStamp{file}.val.has_value());
	assert_eq(DateStamp{file}.val, "");
	assert_false(DOP{file}.val.has_value());
	assert_false(HPositioningError{file}.val.has_value());
    }

    namespace example {

	void p101608(orchis::TC)
	{
	    const tiff::File file {data::p101608};

	    assert_eq(MapDatum{file}.val,     "");
	    assert_eq(LatitudeRef{file}.val,  "N");
	    assert_eq(LongitudeRef{file}.val, "E");
	    assert_triplet(Latitude{file},  57, 1, 40, 1, 30, 1);
	    assert_triplet(Longitude{file}, 12, 1,  1, 1, 16, 1);
	    assert_triplet(TimeStamp{file},  8, 1, 15, 1, 44, 1);
	    assert_eq(DateStamp{file}.val, "2020:06:22");
	}

	void p2482(orchis::TC)
	{
	    const tiff::File file {data::p2482};

	    assert_eq(MapDatum{file}.val,     "WGS-84");
	    assert_eq(LatitudeRef{file}.val,  "N");
	    assert_eq(LongitudeRef{file}.val, "E");
	    assert_triplet(Latitude{file},  65, 1, 39, 1, 15138, 1000);
	    assert_triplet(Longitude{file}, 14, 1, 32, 1,  9449, 1000);
	    assert_triplet(TimeStamp{file},  4, 1, 33, 1, 19000, 1000);
	    assert_eq(DateStamp{file}.val, "2018:07:24");
	    assert_rational(DOP{file}, 10900, 10000);
	}

	void p2120(orchis::TC)
	{
	    const tiff::File file {data::p2120};

	    assert_eq(MapDatum{file}.val,     "WGS84");
	    assert_eq(LatitudeRef{file}.val,  "N");
	    assert_eq(LongitudeRef{file}.val, "E");
	    assert_triplet(Latitude{file},  63, 1, 59, 1, 37410, 1000);
	    assert_triplet(Longitude{file}, 19, 1, 41, 1, 27606, 1000);
	    assert_triplet(TimeStamp{file}, 14, 1, 39, 1, 39000, 1000);
	    assert_eq(DateStamp{file}.val, "2022:02:09");
	}
    }
}

namespace wgs84 {

    using orchis::assert_eq;
    using orchis::assert_true;
    using orchis::assert_false;

    std::vector<uint8_t> overwrite(std::vector<uint8_t> v,
				   unsigned offset,
				   const char* change)
    {
	auto v2 = hexread(change);
	std::copy(begin(v2), end(v2), begin(v) + 6 + offset);
	return v;
    }

    void simple(orchis::TC)
    {
	const tiff::File file{data::exif};
	const Coordinate c{file};
	assert_true(c.valid());
	std::ostringstream oss;
	oss << c;
	assert_eq(oss.str(), "58.2020 13.4826");
    }

    void decimals(orchis::TC)
    {
	auto exif = overwrite(data::exif, 0x7c,
			      "aa0e 0200 1027 0000" // 13.4826
			      "0000 0000 0100 0000" // 0
			      "0000 0000 6400 0000" // 0
			      "");
	const tiff::File file{exif};
	const Coordinate c{file};
	assert_true(c.valid());
	std::ostringstream oss;
	oss << c;
	assert_eq(oss.str(), "58.2020 13.4826");
    }

    void africa(orchis::TC)
    {
	auto exif = overwrite(data::exif, 0x1c,
			      "0100 0200 02000000 53");
	const tiff::File file{exif};
	const Coordinate c{file};
	assert_true(c.valid());
	std::ostringstream oss;
	oss << c;
	assert_eq(oss.str(), "-58.2020 13.4826");
    }

    namespace bad {

	void datum(orchis::TC)
	{
	    auto exif = overwrite(data::exif, 0x5c,
				  "666f 6f00");
	    const tiff::File file{exif};
	    assert_eq(gps::MapDatum{file}.val, "foo");
	    const Coordinate c{file};
	    assert_false(c.valid());
	}

	void north(orchis::TC)
	{
	    auto exif = overwrite(data::exif, 0x1c,
				  "0100 0200 02000000 4e4500");
	    const tiff::File file{exif};
	    assert_eq(gps::LatitudeRef{file}.val, "NE");
	    const Coordinate c{file};
	    assert_false(c.valid());
	}

	void div_by_zero(orchis::TC)
	{
	    auto exif = overwrite(data::exif, 0x64,
				  "3a00 0000 0000 0000");
	    const tiff::File file{exif};
	    const Coordinate c{file};
	    assert_false(c.valid());
	}
    }
}
