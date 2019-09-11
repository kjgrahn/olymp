#include <vector>
#include <orchis.h>
#include "hexread.h"

#include <tiff/tiff.h>


namespace {

    std::vector<uint8_t> h(const std::string& s)
    {
	std::vector<uint8_t> v(s.size()/2);
	const char* a = s.data();
	auto b = s.data() + s.size();
	auto n = hexread(v.data(), &a, b);
	orchis::assert_eq(a, b);
	v.resize(n);
	return v;
    }
}

namespace tiff {

    namespace simple {

	void bigendian(orchis::TC)
	{
	    const auto app1 = h(
		"45 78 69 66 00 00" // Exif
		"4d4d 2a 0000 0008" // TIFF header
		"0001"              // IFD 0 with one tag
		"8769 0004 00000001 0000 001a" // Exif IFD pointer
		"0000 0000"         // no next IFD
		""
		"0001"              // Exif IFD with one tag
		"9003 0002 00000014 0000 002c" // DateTimeOriginal
		"0000 0000"         // no next IFD
		"32 30 31 39 3a 30 39 3a 31 32 20 32 32 3a 33 30 3a 30 30 00");

	    File f {app1};
	}
    }
}
