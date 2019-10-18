/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "jfif.h"

#include <algorithm>
#include <iterator>

using namespace jfif;

namespace {

    constexpr uint8_t ff = 0xff;
    constexpr uint8_t nil = 0;

    template<class T, class It>
    void append(std::vector<T>& v, It a, It b)
    {
	if (a!=b) v.insert(end(v), a, b);
    }

    bool standalone(unsigned ch)
    {
	return ch==0x01 || (0xd0 <= ch && ch <=0xd9);
    }
}

namespace jfif {

    /**
     * Helper for growing segments incrementally and pushing them
     * onto a vector of found segments.
     */
    struct Accumulator {
	explicit Accumulator(std::vector<Segment>& dst) : dst(dst) {}
	Accumulator(const Accumulator&) = delete;
	Accumulator& operator= (const Accumulator&) = delete;

	void emit(unsigned ch);
	void begin(unsigned ch);
	void msb(unsigned n);
	void lsb(unsigned n);
	const uint8_t* feed(const uint8_t *a, const uint8_t *b);

	uint8_t marker;
	unsigned missing = 0;
	std::vector<uint8_t> v;
	std::vector<Segment>& dst;
    };

    // Emit a standalone segment.
    void Accumulator::emit(unsigned ch)
    {
	dst.emplace_back(ch, std::vector<uint8_t>{});
    }

    // Begin a normal segment with marker 'ch'.
    void Accumulator::begin(unsigned ch)
    {
	marker = ch;
	missing = 0;
    }

    // Accept the 8 MSB of a segment length.
    void Accumulator::msb(unsigned n)
    {
	missing = n << 8;
    }

    // Accept the 8 LSB of a segment length.
    void Accumulator::lsb(unsigned n)
    {
	missing |= n;
	if (missing < 2) throw Decoder::IllegalLength {};
	missing -= 2;
	v.reserve(missing);
	v.resize(0);
	if (!missing) feed(nullptr, nullptr);
    }

    // Assuming we're building a segment, drain [a, b)
    // into it and return the remainder, if any.
    const uint8_t* Accumulator::feed(const uint8_t *a,
				     const uint8_t *b)
    {
	auto c = std::min(a+missing, b);
	append(v, a, c);
	missing -= c - a;
	if (!missing) {
	    dst.emplace_back(marker, v);
	}
	return c;
    }
}

Decoder::Decoder()
    : acc {std::make_unique<Accumulator>(v)},
      state {State::Start}
{}

Decoder::~Decoder() = default;

void Decoder::feed(const uint8_t *a, const uint8_t *b)
{
    using S = State;

    while (a!=b) {
	const auto ch = *a;

	switch (state) {
	case S::Start:
	    if (ch==ff) {
		state = S::FF;
	    }
	    else {
		throw FalseStart {};
	    }
	    a++;
	    break;

	case S::Entropy:
	    a = std::find(a, b, ff);
	    if (a!=b) {
		state = S::FF;
		a++;
	    }
	    break;

	case S::Segment:
	    a = acc->feed(a, b);
	    if (!acc->missing) state = S::Entropy;
	    break;

	case S::FF:
	    if (ch==nil) {
		if (v.empty()) throw FalseStart {};
		state = S::Entropy;
	    }
	    else if (ch==ff) {
		;
	    }
	    else if (standalone(ch)) {
		acc->emit(ch);
		if (ch==marker::EOI) {
		    state = S::Trailer;
		}
		else {
		    state = S::Entropy;
		}
	    }
	    else {
		acc->begin(ch);
		state = S::FFmm;
	    }
	    a++;
	    break;

	case S::FFmm:
	    acc->msb(ch);
	    state = S::FFmmnn;
	    a++;
	    break;

	case S::FFmmnn:
	    acc->lsb(ch);
	    if (!acc->missing) {
		state = S::Entropy;
	    }
	    else {
		state = S::Segment;
	    }
	    a++;
	    break;

	case S::Trailer:
	    a = b;
	    break;
	}
    }
}

std::vector<Segment>& Decoder::end()
{
    switch (state) {
    case State::Trailer:
    case State::Entropy:
	break;
    default:
	throw Trailer {};
    }

    return v;
}
