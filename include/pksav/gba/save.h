/*
 * Copyright (c) 2016-2018 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#ifndef PKSAV_GBA_SAVE_H
#define PKSAV_GBA_SAVE_H

#include <pksav/config.h>
#include <pksav/error.h>

#include <pksav/common/trainer_id.h>

#include <pksav/gba/items.h>
#include <pksav/gba/pokedex.h>
#include <pksav/gba/pokemon.h>
#include <pksav/gba/save_type.h>
#include <pksav/gba/time.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PKSAV_GBA_SAVE_SIZE 0x10000

#define PKSAV_GBA_NUM_POKEMON_BOXES 14

#define PKSAV_GBA_TRAINER_NAME_LENGTH 7

struct pksav_gba_options
{
    uint8_t* button_mode_ptr;

    uint8_t* text_options_ptr;

    uint8_t* sound_battle_options_ptr;
};

struct pksav_gba_pokemon_storage
{
    struct pksav_gba_pokemon_party* party_ptr;

    struct pksav_gba_pokemon_pc* pc_ptr;
};

struct pksav_gba_item_storage
{
    union pksav_gba_item_bag* bag_ptr;

    struct pksav_gba_item_pc* pc_ptr;
};

struct pksav_gba_trainer_info
{
    union pksav_trainer_id* id_ptr;

    /*!
     * @brief A pointer to the trainer's name.
     *
     * This value should be accessed with ::pksav_text_from_gba with a num_chars
     * value of 7.
     *
     * This value should be set with ::pksav_text_to_gba with a num_chars
     * value of 7.
     */
    uint8_t* name_ptr;

    uint8_t* gender_ptr;

    uint32_t* money_ptr;
};

struct pksav_gba_misc_fields
{
    uint8_t* rival_name_ptr;

    uint16_t* casino_coins_ptr;
};

struct pksav_gba_save
{
    enum pksav_gba_save_type save_type;

    struct pksav_gba_time* time_played_ptr;

    struct pksav_gba_options options;

    struct pksav_gba_item_storage item_storage;

    struct pksav_gba_pokemon_storage pokemon_storage;

    struct pksav_gba_pokedex pokedex;

    struct pksav_gba_trainer_info trainer_info;

    struct pksav_gba_misc_fields misc_fields;

    void* internal_ptr;
};

#ifdef __cplusplus
extern "C" {
#endif

PKSAV_API enum pksav_error pksav_gba_get_buffer_save_type(
    const uint8_t* buffer,
    size_t buffer_len,
    enum pksav_gba_save_type* save_type_out
);

PKSAV_API enum pksav_error pksav_gba_get_file_save_type(
    const char* filepath,
    enum pksav_gba_save_type* save_type_out
);

PKSAV_API enum pksav_error pksav_gba_load_save_from_buffer(
    uint8_t* buffer,
    size_t buffer_len,
    struct pksav_gba_save* gba_save_out
);

PKSAV_API enum pksav_error pksav_gba_load_save_from_file(
    const char* filepath,
    struct pksav_gba_save* gba_save_out
);

PKSAV_API enum pksav_error pksav_gba_save_save(
    const char* filepath,
    struct pksav_gba_save* gba_save_ptr
);

PKSAV_API enum pksav_error pksav_gba_free_save(
    struct pksav_gba_save* gba_save_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PKSAV_GBA_SAVE_H */
