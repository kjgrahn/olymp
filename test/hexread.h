/* -*- c++ -*-
 *
 * Copyright (c) 2012, 2019 Jörgen Grahn.
 * All rights reserved.
 *
 */
#ifndef OLYMP_HEXREAD_H
#define OLYMP_HEXREAD_H

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>

size_t hexread(uint8_t* buf, const char** begin, const char* end);

std::vector<uint8_t> hexread(const char** begin, const char* end);
std::vector<uint8_t> hexread(const std::string& s);

#endif
