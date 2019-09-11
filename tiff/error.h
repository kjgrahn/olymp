/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_ERROR_H
#define OLYMP_TIFF_ERROR_H

namespace tiff {

    class Error {};
    class Segfault: public Error {};
}

#endif
