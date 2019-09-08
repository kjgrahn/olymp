#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "jfif.h"

namespace {

    // names for the most common markers
    static const std::unordered_map<unsigned, const char*> names =
    {
	{0xd8, "soi"},
	{0xc0, "sof0"},
	{0xc2, "sof2"},
	{0xc4, "dht"},
	{0xdb, "dqt"},
	{0xdd, "dri"},
	{0xda, "sos"},
	{0xd0, 0}, 	 {0xd1, 0},
	{0xd2, 0}, 	 {0xd3, 0},
	{0xd4, 0}, 	 {0xd5, 0},
	{0xd6, 0}, 	 {0xd7, 0},
	{0xe0, "app0"},  {0xe1, "app1"},
	{0xe2, "app2"},  {0xe3, "app3"},
	{0xe4, "app4"},  {0xe5, "app5"},
	{0xed, "app13"}, {0xee, "app14"},
	{0xfe, "com"},
	{0xd9, "eoi"},
    };

    std::ostream& describe(std::ostream& os,
			   const std::vector<jfif::Segment>& v)
    {
	for (const auto& seg : v) {
	    auto it = names.find(seg.marker);
	    if (it!=end(names)) {
		if (it->second) {
		    os << ' ' << it->second;
		}
	    }
	    else {
		char buf[6];
		std::snprintf(buf, sizeof buf, " ff%02x", seg.marker);
		os << buf;
	    }
	}
	return os;
    }

    void seg(std::ostream& os, const std::string& name)
    {
	jfif::Decoder decoder;
	const int fd = open(name.c_str(), O_RDONLY);
	if (fd==-1) {
	    os << name << ": cannot open: " << std::strerror(errno) << '\n';
	    return;
	}

	try {
	    while (1) {
		uint8_t buf[64*1024];
		auto res = read(fd, buf, sizeof buf);
		if (res==-1) {
		    os << name << ": error: " << std::strerror(errno) << '\n';
		}
		else if (res==0) {
		    break;
		}
		else {
		    decoder.feed(buf, buf+res);
		}
	    }

	    auto segments = decoder.end();
	    os << name << ":";
	    describe(os, segments) << '\n';
	}
	catch (jfif::Decoder::IllegalLength&) {
	    os << name << ": decode error: bad segment length\n";
	}
	catch (jfif::Decoder::Trailer&) {
	    os << name << ": decode error: trailing data\n";
	}
	catch (jfif::Decoder::Error&) {
	    os << name << ": decode error\n";
	}
	close(fd);
    }
}

int main(int argc, char** argv)
{
    const std::vector<std::string> args {&argv[1], &argv[argc]};
    for (auto name: args) {
	seg(std::cout, name);
    }
    return 0;
}
