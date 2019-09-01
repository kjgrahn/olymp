/*
 * Copyright (c) 2012 Jörgen Grahn.
 * All rights reserved.
 *
 */
#ifndef UDPTOOLS_HEXREAD_H
#define UDPTOOLS_HEXREAD_H
#include <stdlib.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

size_t hexread(uint8_t* buf, const char** begin, const char* end);

#ifdef __cplusplus
}
#endif
#endif
