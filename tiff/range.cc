/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "range.h"

using tiff::Range;

/**
 * The range covering 'v'.
 */
Range::Range(const std::vector<uint8_t>& v)
    : a {v.data()},
      b {a + v.size()}
{}

/**
 * The subrange of 'whole' that comes after the subrange 'pred'.
 */
Range::Range(const Range& whole, const Range& pred)
    : a {pred.b},
      b {whole.b}
{}
