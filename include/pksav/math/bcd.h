/*
 * Copyright (c) 2016 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#ifndef PKSAV_MATH_BCD_H
#define PKSAV_MATH_BCD_H

#include <pksav/config.h>

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

PKSAV_API size_t pksav_from_bcd(
    const uint8_t* buffer,
    size_t num_bytes
);

PKSAV_API void pksav_to_bcd(
    size_t num,
    uint8_t* buffer_out
);

#ifdef __cplusplus
}
#endif

#endif
