/*!
 * @file    pksav/gen1/save.h
 * @ingroup PKSav
 * @brief   Functions for interacting with Generation I save files.
 *
 * Copyright (c) 2016 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#ifndef PKSAV_GEN1_SAVE_H
#define PKSAV_GEN1_SAVE_H

#include <pksav/config.h>
#include <pksav/error.h>

#include <pksav/gen1/items.h>
#include <pksav/gen1/pokemon.h>
#include <pksav/gen1/text.h>

#include <pksav/math/base256.h>
#include <pksav/math/bcd.h>
#include <pksav/math/endian.h>

#include <stdbool.h>
#include <stdint.h>

/*
 * Structures exclusive to Generation I games
 */
#pragma pack(push,1)

typedef struct {
    uint16_t hours;
    uint8_t minutes;
    uint8_t seconds;
} pksav_gen1_time_t;

#pragma pack(pop)

/*
 * Savefile size and locations of fields within file
 */

#define PKSAV_GEN1_SAVE_SIZE 0x8000

typedef enum {
    PKSAV_GEN1_PLAYER_NAME             = 0x2598,
    PKSAV_GEN1_POKEDEX_OWNED           = 0x25A3,
    PKSAV_GEN1_POKEDEX_SEEN            = 0x25B6,
    PKSAV_GEN1_ITEM_BAG                = 0x25C9,
    PKSAV_GEN1_MONEY                   = 0x25F3,
    PKSAV_GEN1_RIVAL_NAME              = 0x25F6,
    PKSAV_GEN1_OPTIONS                 = 0x2601,
    PKSAV_GEN1_BADGES                  = 0x2602,
    PKSAV_GEN1_PLAYER_ID               = 0x2605,
    PKSAV_GEN1_PIKACHU_FRIENDSHIP      = 0x271C,
    PKSAV_GEN1_ITEM_PC                 = 0x27E6,
    PKSAV_GEN1_CURRENT_POKEMON_BOX_NUM = 0x284C,
    PKSAV_GEN1_CASINO_COINS            = 0x2850,
    PKSAV_GEN1_TIME_PLAYED             = 0x2CED,
    PKSAV_GEN1_POKEMON_PARTY           = 0x2F2C,
    PKSAV_GEN1_CURRENT_POKEMON_BOX     = 0x30C0,
    PKSAV_GEN1_CHECKSUM                = 0x3523,
    PKSAV_GEN1_POKEMON_PC_FIRST_HALF   = 0x4000,
    PKSAV_GEN1_POKEMON_PC_SECOND_HALF  = 0x6000
} pksav_gen1_save_offset_t;

/*
 * Badge positions in bitmap
 */
typedef enum {
    PKSAV_GEN1_EARTH_BADGE = 0,
    PKSAV_GEN1_VOLCANO_BADGE,
    PKSAV_GEN1_MARSH_BADGE,
    PKSAV_GEN1_SOUL_BADGE,
    PKSAV_GEN1_RAINBOW_BADGE,
    PKSAV_GEN1_THUNDER_BADGE,
    PKSAV_GEN1_CASCADE_BADGE,
    PKSAV_GEN1_BOULDER_BADGE
} pksav_gen1_badge_t;

/*
 * Savefile structure to be used by user
 */
typedef struct {
    pksav_gen1_pokemon_party_t* pokemon_party;
    pksav_gen1_pokemon_box_t** pokemon_boxes;

    pksav_gen1_item_bag_t* item_bag;
    pksav_gen1_item_pc_t* item_pc;

    pksav_gen1_time_t* time_played;
    uint16_t* trainer_id;

    // Stored in BCD
    uint8_t* money;
    uint8_t* casino_coins;

    // Stored as strings
    uint8_t* trainer_name;
    uint8_t* rival_name;

    // Stored as a bitmap
    uint8_t* badges;

    uint8_t* pikachu_friendship;

    bool yellow;
    uint8_t* raw;
} pksav_gen1_save_t;

#ifdef __cplusplus
extern "C" {
#endif

PKSAV_API bool pksav_file_is_gen1_save(
    const char* filepath
);

PKSAV_API pksav_error_t pksav_gen1_save_load(
    const char* filepath,
    pksav_gen1_save_t* gen1_save
);

PKSAV_API pksav_error_t pksav_gen1_save_save(
    const char* filepath,
    pksav_gen1_save_t* gen1_save
);

static PKSAV_INLINE void pksav_gen1_save_free(
    pksav_gen1_save_t* gen1_save
) {
    free(gen1_save->pokemon_boxes);
    free(gen1_save->raw);
}

static PKSAV_INLINE uint32_t pksav_gen1_save_get_money(
    pksav_gen1_save_t* gen1_save
) {
    return (uint32_t)(pksav_from_bcd(gen1_save->money, 3));
}

static PKSAV_INLINE uint16_t pksav_gen1_save_get_casino_coins(
    pksav_gen1_save_t* gen1_save
) {
    return (uint16_t)(pksav_from_bcd(gen1_save->casino_coins, 2));
}

static PKSAV_INLINE bool pksav_gen1_save_has_badge(
    pksav_gen1_save_t* gen1_save,
    pksav_gen1_badge_t badge
) {
    return ((*gen1_save->badges) & (1 << badge));
}

static PKSAV_INLINE uint16_t pksav_gen1_save_get_trainer_id(
    pksav_gen1_save_t* gen1_save
) {
    return pksav_bigendian16(*gen1_save->trainer_id);
}

static PKSAV_INLINE void pksav_gen1_save_get_trainer_name_as_text(
    pksav_gen1_save_t* gen1_save,
    char* output_text
) {
    pksav_text_from_gen1(gen1_save->trainer_name, output_text, 7);
}

static PKSAV_INLINE void pksav_gen1_save_get_trainer_name_as_widetext(
    pksav_gen1_save_t* gen1_save,
    wchar_t* output_text
) {
    pksav_widetext_from_gen1(gen1_save->trainer_name, output_text, 7);
}

static PKSAV_INLINE void pksav_gen1_save_get_rival_name_as_text(
    pksav_gen1_save_t* gen1_save,
    char* output_text
) {
    pksav_text_from_gen1(gen1_save->rival_name, output_text, 7);
}

static PKSAV_INLINE void pksav_gen1_save_get_rival_name_as_widetext(
    pksav_gen1_save_t* gen1_save,
    wchar_t* output_text
) {
    pksav_widetext_from_gen1(gen1_save->rival_name, output_text, 7);
}

PKSAV_API pksav_error_t pksav_gen1_save_set_money(
    pksav_gen1_save_t* gen1_save,
    uint32_t money
);

PKSAV_API pksav_error_t pksav_gen1_save_set_casino_coins(
    pksav_gen1_save_t* gen1_save,
    uint16_t casino_coins
);

static PKSAV_INLINE void pksav_gen1_save_set_has_badge(
    pksav_gen1_save_t* gen1_save,
    pksav_gen1_badge_t badge,
    bool has_badge
) {
    if(has_badge) {
        (*gen1_save->badges) |= (1 << badge);
    } else {
        (*gen1_save->badges) &= (uint8_t)(1 << badge);
    }
}

static PKSAV_INLINE void pksav_gen1_save_set_trainer_id(
    pksav_gen1_save_t* gen1_save,
    uint16_t trainer_id
) {
    *gen1_save->trainer_id = pksav_bigendian16(trainer_id);
}

PKSAV_API pksav_error_t pksav_gen1_save_set_trainer_name_from_text(
    pksav_gen1_save_t* gen1_save,
    const char* trainer_name
);

PKSAV_API pksav_error_t pksav_gen1_save_set_trainer_name_from_widetext(
    pksav_gen1_save_t* gen1_save,
    const wchar_t* trainer_name
);

PKSAV_API pksav_error_t pksav_gen1_save_set_rival_name_from_text(
    pksav_gen1_save_t* gen1_save,
    const char* rival_name
);

PKSAV_API pksav_error_t pksav_gen1_save_set_rival_name_from_widetext(
    pksav_gen1_save_t* gen1_save,
    const wchar_t* rival_name
);

#ifdef __cplusplus
}
#endif

#endif /* PKSAV_GEN1_SAVE_H */
