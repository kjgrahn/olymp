/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 or version 3 of the
 *    License.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * (Actually, you received no such copy.)
 *
 */
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>

#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "jfif.h"
#include "tiff/tiff.h"
#include "exif.h"
#include "filename.h"


namespace {

    struct IOError {};

    /**
     * Minimal wrapper for the opened-for-reading fd used here.
     */
    class Fd {
    public:
	explicit Fd(const std::string& path)
	    : fd {open(path.c_str(), O_RDONLY)}
	{
	    if (fd==-1) throw IOError {};
	}
	~Fd() { close(fd); }

	ssize_t read(void *buf, size_t count) const
	{
	    return ::read(fd, buf, count);
	}

    private:
	const int fd;
    };

    struct NoApp1 {};

    /**
     * The JFIF APP1 segment found in open file 'fd'. May throw.
     */
    jfif::Segment app1_of(const Fd& fd)
    {
	jfif::Decoder decoder;
	uint8_t buf[8*1024];

	auto is_app1 = [] (const jfif::Segment& seg) {
			   return seg.marker == jfif::marker::APP1;
		       };

	while (1) {
	    auto res = fd.read(buf, sizeof buf);
	    if (res==-1) throw IOError {};
	    if (res==0) break;

	    decoder.feed(buf, buf+res);
	    auto it = std::find_if(begin(decoder.v), end(decoder.v), is_app1);
	    if (it!=end(decoder.v)) return *it;
	}
	decoder.end();
	auto it = std::find_if(begin(decoder.v), end(decoder.v), is_app1);
	if (it!=end(decoder.v)) return *it;
	throw NoApp1 {};
    }

    /**
     * The JPG image file name formed by a certain date and serial
     * number.
     */
    std::string filename(const exif::DateTimeOriginal& ts,
			 const Serial& nnnn)
    {
	std::ostringstream oss;
	oss << ts.date() << '_' << nnnn << ".jpg";
	return oss.str();
    }

    /**
     * Render the output for one image.
     */
    void render(std::ostream& os,
		const exif::DateTimeOriginal& ts,
		const Serial& nnnn)
    {
	os << '\n'
	   << filename(ts, nnnn) << '\n'
	   << ts.date() << ' ' << ts.hhmm() << '\n';
    }

    /**
     * A bit like 'mv -i'.
     */
    bool mv_i(const std::string& from,
	      const exif::DateTimeOriginal& ts,
	      const Serial& nnnn)
    {
	/* Use link(2) instead of rename(2) since it's important to
	 * fail rather than destroy existing files.
	 */
	const auto to = neighbour(from, filename(ts, nnnn));
	if (to==from) return true;
	int err = link(from.c_str(), to.c_str());
	if (err) return false;

	err = unlink(from.c_str());
	return !err;
    }

    /**
     * Like olympv() but for a single file, and with a boolean result.
     */
    bool olymp(const std::string& file,
	       std::ostream& out,
	       std::ostream& err,
	       const bool rename)
    {
	auto errfile = [&file, &err] () -> std::ostream& {
			   err << file << ": error: ";
			   return err;
		       };

	const Serial nnnn = serial(file);
	if (!nnnn.valid()) {
	    errfile() << "no serial number in file name\n";
	    return false;
	}

	try {
	    const Fd fd {file};
	    const auto app1 = app1_of(fd);
	    const tiff::File tiff {app1.v};

	    const exif::DateTimeOriginal ts {tiff};
	    if (!ts.valid()) {
		errfile() << "no valid timestamp in EXIF data\n";
		return false;
	    }

	    render(out, ts, nnnn);

	    if (rename && !mv_i(file, ts, nnnn)) {
		errfile() << "cannot rename: " << std::strerror(errno) << '\n';
		return false;
	    }

	    return true;
	}
	catch (const jfif::Decoder::Error&) {
	    errfile() << "cannot decode as JPEG\n";
	}
	catch (const IOError&) {
	    errfile() << std::strerror(errno) << '\n';
	}
	catch (const NoApp1&) {
	    errfile() << "no EXIF data in file\n";
	}
	catch (const tiff::Error&) {
	    errfile() << "corrupt EXIF data structure\n";
	}

	return false;
    }

    /**
     * Investigate 'files', a sequence of file names, and print a
     * better name, and date/time stamp, to 'out'.
     *
     * If 'rename' is set, also try to rename them accordingly.
     * Whines to 'err' if something goes wrong, and also returns a
     * non-zero exit code.
     */
    int olympv(const std::vector<std::string>& files,
	       std::ostream& out,
	       std::ostream& err,
	       const bool rename)
    {
	int rc = 0;
	for (const auto& file: files) {
	    if (!olymp(file, out, err, rename)) rc = 1;
	}
	return rc;
    }
}

int main(int argc, char ** argv)
{
    const std::string prog = argv[0] ? argv[0] : "olymp";
    const std::string usage = std::string("usage: ")
	+ prog + " [-e] file ...\n"
	"       "
	+ prog + " --help\n"
	"       "
	+ prog + " --version";
    const char optstring[] = "e";
    const struct option long_options[] = {
	{"help", 0, 0, 'H'},
	{"version", 0, 0, 'V'},
	{0, 0, 0, 0}
    };

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    bool rename = false;

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring,
			    &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'e':
	    rename = true;
	    break;
	case 'H':
	    std::cout << usage << '\n';
	    return 0;
	    break;
	case 'V':
	    std::cout << "olymp 3.2\n"
		      << "Copyright (c) 2002--2012, 2014, 2019 Jörgen Grahn\n";
	    return 0;
	    break;
	case ':':
	case '?':
	default:
	    std::cerr << usage << '\n';
	    return 1;
	    break;
	}
    }

    return olympv({argv+optind, argv+argc},
		  std::cout, std::cerr, rename);
}
