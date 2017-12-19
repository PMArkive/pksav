/*!
 * @file    pksav/nds/text.h
 * @ingroup PKSav
 * @brief   Conversions between C strings and the string formats used in Generation IV-V games.
 *
 * Copyright (c) 2016 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#ifndef PKSAV_NDS_TEXT_H
#define PKSAV_NDS_TEXT_H

#include <pksav/config.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Convert a string from the given generation's format to a multi-byte C string
/*!
 * In Generation IV games, strings are stored with a proprietary character
 * map, and in Generation V games, strings are stored in Unicode. In both cases,
 * characters are 2 bytes wide.
 *
 * Both buffers passed into the function must be at least the length passed
 * in the num_chars parameter, or it will result in undefined behavior.
 *
 * \param gen4 true if the text is from Generation IV, false if from Generation V
 * \param input_buffer Generation IV-V string
 * \param output_text output buffer in which to place converted text
 * \param num_chars the number of characters to convert
 */
PKSAV_API void pksav_text_from_nds(
    bool gen4,
    const uint16_t* input_buffer,
    char* output_text,
    size_t num_chars
);

//! Convert a string from the given generation's format to a wide-character C string
/*!
 * In Generation IV games, strings are stored with a proprietary character
 * map, and in Generation V games, strings are stored in Unicode. In both cases,
 * characters are 2 bytes wide.
 *
 * Both buffers passed into the function must be at least the length passed
 * in the num_chars parameter, or it will result in undefined behavior.
 *
 * \param gen4 true if the text is from Generation IV, false if from Generation V
 * \param input_buffer Generation IV-V string
 * \param output_text output buffer in which to place converted text
 * \param num_chars the number of characters to convert
 */
PKSAV_API void pksav_widetext_from_nds(
    bool gen4,
    const uint16_t* input_buffer,
    wchar_t* output_text,
    size_t num_chars
);

//! Convert a multi-byte C string to a string in the given game's format
/*!
 * In Generation IV games, strings are stored with a proprietary character
 * map, and in Generation V games, strings are stored in Unicode. In both cases,
 * characters are 2 bytes wide.
 *
 * Both buffers passed into the function must be at least the length passed
 * in the num_chars parameter, or it will result in undefined behavior.
 *
 * \param gen4 true if the text is for Generation IV, false if for Generation V
 * \param input_text C string to convert
 * \param output_buffer output buffer in which to place converted text
 * \param num_chars the number of characters to convert
 */
PKSAV_API void pksav_text_to_nds(
    bool gen4,
    const char* input_text,
    uint16_t* output_buffer,
    size_t num_chars
);

//! Convert a wide-character C string to a string in the given game's format
/*!
 * In Generation IV games, strings are stored with a proprietary character
 * map, and in Generation V games, strings are stored in Unicode. In both cases,
 * characters are 2 bytes wide.
 *
 * Both buffers passed into the function must be at least the length passed
 * in the num_chars parameter, or it will result in undefined behavior.
 *
 * \param gen4 true if the text is for Generation IV, false if for Generation V
 * \param input_text C string to convert
 * \param output_buffer output buffer in which to place converted text
 * \param num_chars the number of characters to convert
 */
PKSAV_API void pksav_widetext_to_nds(
    bool gen4,
    const wchar_t* input_text,
    uint16_t* output_buffer,
    size_t num_chars
);

#ifdef __cplusplus
}
#endif

#endif /* PKSAV_NDS_TEXT_H */