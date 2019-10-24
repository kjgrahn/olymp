/* -*- c++ -*-
 *
 * Copyright (c) 2012, 2019 Jörgen Grahn
 * All rights reserved.
 */
#ifndef OLYMP_TRANSFORM_H
#define OLYMP_TRANSFORM_H

#include "wgs84.h"
#include "sweref99.h"

/**
 * Conversion WGS 84 --> SWEREF 99 TM.
 */
class Transform {
public:
    Transform();
    ~Transform();
    Transform(const Transform&) = delete;
    Transform& operator= (const Transform&) = delete;

    sweref99::Coordinate operator() (const wgs84::Coordinate& c) const;

    static const char* pj_release();

private:
    void* t;
};

#endif
