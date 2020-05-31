#include <orchis.h>

#include <cluster.h>

namespace {

    template <class T>
    void join(std::ostream& os, const std::vector<T> vals)
    {
	const char* delim = "";
	for (const auto& val: vals) {
	    os << delim << val;
	    delim = ", ";
	}
    }

    template <class T>
    std::string clusters(Cluster<T>& c,
			 const std::vector<T> vals)
    {
	std::ostringstream oss;
	for (const auto& val: vals) {
	    const auto ov = c.add(val);
	    if (ov.empty()) continue;

	    join(oss << '[', ov);
	    oss << ']';
	}
	const auto ov = c.end();
	if (!ov.empty()) {
	    join(oss << '[', ov);
	    oss << ']';
	}
	return oss.str();
    }
}

namespace cluster {

    using orchis::assert_eq;

    template <class N>
    bool lt2(const N& a, const N& b) { return b-a < 2; }

    void empty(orchis::TC)
    {
	Cluster<int> c(lt2<int>);
	assert_eq(clusters(c, {}), "");
    }

    void single(orchis::TC)
    {
	Cluster<int> c(lt2<int>);
	assert_eq(clusters(c, {42}), "[42]");
    }

    void close(orchis::TC)
    {
	Cluster<int> c(lt2<int>);
	assert_eq(clusters(c, {0, 1, 2, 3, 4}),
		  "[0, 1, 2, 3, 4]");
    }

    void distant(orchis::TC)
    {
	Cluster<int> c(lt2<int>);
	assert_eq(clusters(c, {0, 5, 10, 15, 20, 25}),
		  "[0][5][10][15][20][25]");
    }

    void two(orchis::TC)
    {
	Cluster<int> c(lt2<int>);
	assert_eq(clusters(c, {0, 0, 3, 3, 3}),
		  "[0, 0][3, 3, 3]");
    }
}
