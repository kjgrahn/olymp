#include <vector>
#include <orchis.h>
#include "hexread.h"

#include <tiff/tiff.h>
#include <exif.h>

namespace exif {

    using orchis::assert_eq;
    using orchis::assert_true;

    namespace data {
	const auto simple = hexread(
	    "45 78 69 66 00 00" // Exif
	    "4949 2a00 0800 0000" // TIFF header
	    "0100"              // IFD 0 with one tag
	    "6987 0400 01000000 1a00 0000" // Exif IFD pointer
	    "0000 0000"         // no next IFD
	    ""
	    "0100"              // Exif IFD with one tag
	    "0390 0200 14000000 2c00 0000" // DateTimeOriginal
	    "0000 0000"         // no next IFD
	    "32 30 31 39 3a 30 39 3a 31 32 20 32 32 3a 33 30 3a 35 39 00");
    }

    void datetime(orchis::TC)
    {
	tiff::File f {data::simple};
	const exif::DateTimeOriginal dt{f};

	assert_true(dt.valid());
	assert_eq(dt.date(), "2019-09-12");
	assert_eq(dt.hhmm(), "22:30");
	assert_eq(dt.hhmmss(), "22:30:59");
    }
}
