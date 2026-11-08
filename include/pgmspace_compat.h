#ifndef PGMSPACE_COMPAT_H
#define PGMSPACE_COMPAT_H

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#endif
