/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_H
#define OLYMP_TIFF_H

#include <cstdint>
#include <vector>
#include <array>
#include <memory>

namespace tiff {

    using std::size_t;
    class File;

    class Offset {
    public:
	explicit Offset(const uint8_t*);
    };

    struct Blob {
	template <class It>
	Blob(It a, It b);
	Blob(Offset, std::size_t);
	Blob();

	size_t size() const;
	uint8_t operator[] (size_t n) const;
    };

    /**
     * A TIFF IFD appearing at a certain offset in a File.  An IFD is
     * a sequence of 1..255 fields and a next IFD offset.
     */
    class Ifd {
    public:
	Ifd() = default;
	Ifd(const File&, Offset);

	Blob find(unsigned tag, unsigned type, unsigned count) const;

    private:
	const uint8_t* begin() const;
	const uint8_t* end() const;
	unsigned get32(const uint8_t*) const;
    };

    /**
     * TIFF fields, aka "tags", as they appear in various tables in
     * the TIFF and Exif specs.  The list is incomplete.
     */
    namespace field {

	template <unsigned Tag, unsigned Len>
	class Ascii {
	    std::string val;
	};

	template <unsigned Tag, unsigned Len = 1>
	class Short {
	    std::array<unsigned, Len> val;
	};

	template <unsigned Tag, unsigned Len = 1>
	class Long {
	    std::array<unsigned, Len> val;
	};

	template <unsigned Tag, unsigned Len = 1>
	class Rational {
	    std::array<std::pair<unsigned, unsigned>, Len> val;
	};
    }

    /**
     * A TIFF file according to TIFF revision 6.0 (Adobe 1992).
     *
     * Here it's defined as the content of a JFIF APP1 segment, after
     * the Exif marker.  And what we're interested in is TIFF fields
     * in the Exif and GPS IFDs, which can be found via IFD 0.
     */
    class File {
    public:
	explicit File(const std::vector<uint8_t>& app1);
	Ifd ifd0;
	Ifd exif;
	Ifd gps;

	template <class F>
	std::unique_ptr<F> find(const Ifd& ifd) const;

    private:
	const Blob content;
	const bool bigendian;
    };

    template <class F>
    std::unique_ptr<F> File::find(const Ifd& ifd) const
    {
	Blob b = ifd.find(F::tag, F::type, F::len);
    }
}

#endif
