#include <orchis.h>
#include "hexread.h"

#include <gps.h>

namespace data {
    const auto exif = hexread(
	"45 78 69 66 00 00" // Exif
	"4949 2a00 0800 0000" // TIFF header
	"0100"              // IFD 0 with one tag
	"2588 0400 01000000 1a00 0000" // GPS IFD pointer
	"0000 0000"         // no next IFD
	""
	"0500"
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
}
