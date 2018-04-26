/*
 * Copyright (c) 2016-2018 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "gen1/save_internal.h"
#include "util/fs.h"

#include <pksav/gen1/save.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum pksav_error pksav_gen1_get_buffer_save_type(
    const uint8_t* buffer,
    size_t buffer_len,
    enum pksav_gen1_save_type* save_type_out
)
{
    if(!buffer || !save_type_out)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    enum pksav_error error = PKSAV_ERROR_NONE;

    *save_type_out = PKSAV_GEN1_SAVE_TYPE_NONE;

    if(buffer_len >= PKSAV_GEN1_SAVE_SIZE)
    {
        uint8_t buffer_checksum = buffer[PKSAV_GEN1_CHECKSUM];
        uint8_t calculated_checksum = pksav_gen1_get_save_checksum(buffer);
        if(buffer_checksum == calculated_checksum)
        {
            /*
             * Check if this save is for the Yellow version. The only way to
             * check this is to check the Pikachu Friendship field, which
             * isn't used in Red/Blue. This is usually fine but will fail if
             * the trainer's Pikachu despises the trainer enough to have a
             * friendship value of 0, which is unlikely but technically
             * possible.
             */
            if(buffer[PKSAV_GEN1_PIKACHU_FRIENDSHIP] > 0)
            {
                *save_type_out = PKSAV_GEN1_SAVE_TYPE_YELLOW;
            }
            else
            {
                *save_type_out = PKSAV_GEN1_SAVE_TYPE_RED_BLUE;
            }
        }
    }

    return error;
}

enum pksav_error pksav_gen1_get_file_save_type(
    const char* filepath,
    enum pksav_gen1_save_type* save_type_out
)
{
    if(!filepath || !save_type_out)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    enum pksav_error error = PKSAV_ERROR_NONE;

    uint8_t* file_buffer = NULL;
    size_t buffer_len = 0;
    error = pksav_fs_read_file_to_buffer(
                filepath,
                &file_buffer,
                &buffer_len
            );
    if(!error)
    {
        assert(file_buffer != NULL);

        enum pksav_gen1_save_type save_type = PKSAV_GEN1_SAVE_TYPE_NONE;
        error = pksav_gen1_get_buffer_save_type(
                    file_buffer,
                    buffer_len,
                    &save_type
                );
        free(file_buffer);

        // Only return a result upon success.
        if(!error)
        {
            *save_type_out = save_type;
        }
    }

    return error;
}

static void _pksav_gen1_set_save_pointers(
    struct pksav_gen1_save* gen1_save_ptr,
    uint8_t* file_buffer
)
{
    assert(gen1_save_ptr != NULL);
    assert(file_buffer != NULL);

    // Item storage
    struct pksav_gen1_item_storage* item_storage_ptr = &gen1_save_ptr->item_storage;

    item_storage_ptr->item_bag_ptr = (struct pksav_gen1_item_bag*)(
                                         &file_buffer[PKSAV_GEN1_ITEM_BAG]
                                     );
    item_storage_ptr->item_pc_ptr = (struct pksav_gen1_item_pc*)(
                                        &file_buffer[PKSAV_GEN1_ITEM_PC]
                                    );

    // Pokémon storage
    struct pksav_gen1_pokemon_storage* pokemon_storage_ptr = &gen1_save_ptr->pokemon_storage;

    pokemon_storage_ptr->party_ptr = (struct pksav_gen1_pokemon_party*)(
                                         &file_buffer[PKSAV_GEN1_POKEMON_PARTY]
                                     );

    for(size_t box_index = 0; box_index < 6; ++box_index)
    {
        size_t offset = PKSAV_GEN1_POKEMON_PC_FIRST_HALF +
                        (sizeof(struct pksav_gen1_pokemon_box) * box_index);

        pokemon_storage_ptr->box_ptrs[box_index] = (struct pksav_gen1_pokemon_box*)(
                                                               &file_buffer[offset]
                                                           );
    }
    for(size_t box_index = 6; box_index < 12; ++box_index)
    {
        size_t offset = PKSAV_GEN1_POKEMON_PC_SECOND_HALF +
                        (sizeof(struct pksav_gen1_pokemon_box) * (box_index - 6));

        pokemon_storage_ptr->box_ptrs[box_index] = (struct pksav_gen1_pokemon_box*)(
                                                               &file_buffer[offset]
                                                           );
    }

    pokemon_storage_ptr->current_box_num_ptr = &file_buffer[PKSAV_GEN1_CURRENT_BOX_NUM];
    pokemon_storage_ptr->current_box_ptr = (struct pksav_gen1_pokemon_box*)(
                                                       &file_buffer[PKSAV_GEN1_CURRENT_BOX]
                                                   );

    // Pokédex lists
    struct pksav_gen1_pokedex_lists* pokedex_lists_ptr = &gen1_save_ptr->pokedex_lists;

    pokedex_lists_ptr->seen_ptr  = &file_buffer[PKSAV_GEN1_POKEDEX_SEEN];
    pokedex_lists_ptr->owned_ptr = &file_buffer[PKSAV_GEN1_POKEDEX_OWNED];

    // Trainer info
    struct pksav_gen1_trainer_info* trainer_info_ptr = &gen1_save_ptr->trainer_info;

    trainer_info_ptr->id_ptr     = (uint16_t*)&file_buffer[PKSAV_GEN1_PLAYER_ID];
    trainer_info_ptr->name_ptr   = &file_buffer[PKSAV_GEN1_PLAYER_NAME];
    trainer_info_ptr->money_ptr  = &file_buffer[PKSAV_GEN1_MONEY];
    trainer_info_ptr->badges_ptr = &file_buffer[PKSAV_GEN1_BADGES];

    // Misc
    struct pksav_gen1_misc_fields* misc_fields_ptr = &gen1_save_ptr->misc_fields;

    misc_fields_ptr->rival_name_ptr   = &file_buffer[PKSAV_GEN1_RIVAL_NAME];
    misc_fields_ptr->casino_coins_ptr = &file_buffer[PKSAV_GEN1_CASINO_COINS];

    if(gen1_save_ptr->save_type == PKSAV_GEN1_SAVE_TYPE_YELLOW)
    {
        misc_fields_ptr->pikachu_friendship_ptr = &file_buffer[PKSAV_GEN1_PIKACHU_FRIENDSHIP];
    }
    else
    {
        misc_fields_ptr->pikachu_friendship_ptr = NULL;
    }

    gen1_save_ptr->time_played_ptr = (struct pksav_gen1_time*)(
                                         &file_buffer[PKSAV_GEN1_TIME_PLAYED]
                                     );
    gen1_save_ptr->options_ptr = &file_buffer[PKSAV_GEN1_OPTIONS];

    // Internal
    gen1_save_ptr->internal_ptr = calloc(sizeof(struct pksav_gen1_save_internal), 1);
    struct pksav_gen1_save_internal* internal_ptr = gen1_save_ptr->internal_ptr;
    internal_ptr->raw_save_ptr = file_buffer;
    internal_ptr->checksum_ptr = &file_buffer[PKSAV_GEN1_CHECKSUM];
}

static enum pksav_error _pksav_gen1_load_save_from_buffer(
    uint8_t* buffer,
    size_t buffer_len,
    bool is_buffer_ours,
    struct pksav_gen1_save* gen1_save_out
)
{
    assert(gen1_save_out != NULL);
    assert(buffer != NULL);

    enum pksav_error error = PKSAV_ERROR_NONE;

    enum pksav_gen1_save_type save_type = PKSAV_GEN1_SAVE_TYPE_NONE;
    error = pksav_gen1_get_buffer_save_type(
                buffer,
                buffer_len,
                &save_type
            );
    if(!error && (save_type != PKSAV_GEN1_SAVE_TYPE_NONE))
    {
        if(save_type != PKSAV_GEN1_SAVE_TYPE_NONE)
        {
            gen1_save_out->save_type = save_type;
            _pksav_gen1_set_save_pointers(
                gen1_save_out,
                buffer
            );

            // Internal
            struct pksav_gen1_save_internal* internal_ptr = gen1_save_out->internal_ptr;
            internal_ptr->is_buffer_ours = is_buffer_ours;
        }
        else
        {
            error = PKSAV_ERROR_INVALID_SAVE;
        }
    }

    return error;
}

enum pksav_error pksav_gen1_load_save_from_buffer(
    uint8_t* buffer,
    size_t buffer_len,
    struct pksav_gen1_save* gen1_save_out
)
{
    if(!buffer || !gen1_save_out)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    return _pksav_gen1_load_save_from_buffer(
               buffer,
               buffer_len,
               false, // is_buffer_ours
               gen1_save_out
           );
}

enum pksav_error pksav_gen1_load_save_from_file(
    const char* filepath,
    struct pksav_gen1_save* gen1_save_out
)
{
    if(!filepath || !gen1_save_out)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    enum pksav_error error = PKSAV_ERROR_NONE;

    uint8_t* buffer = NULL;
    size_t buffer_len = 0;
    error = pksav_fs_read_file_to_buffer(
                filepath,
                &buffer,
                &buffer_len
            );

    if(!error)
    {
        error = _pksav_gen1_load_save_from_buffer(
                    buffer,
                    buffer_len,
                    true, // is_buffer_ours
                    gen1_save_out
                );
        if(error)
        {
            // We made this buffer, so it's on us to free it if there's
            // an error.
            free(buffer);
        }
    }

    return error;
}

enum pksav_error pksav_gen1_save_save(
    const char* filepath,
    struct pksav_gen1_save* gen1_save_ptr
)
{
    if(!filepath || !gen1_save_ptr)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    enum pksav_error error = PKSAV_ERROR_NONE;

    struct pksav_gen1_save_internal* internal_ptr = gen1_save_ptr->internal_ptr;

    *internal_ptr->checksum_ptr = pksav_gen1_get_save_checksum(
                                       internal_ptr->raw_save_ptr
                                  );

    error = pksav_fs_write_buffer_to_file(
                filepath,
                internal_ptr->raw_save_ptr,
                PKSAV_GEN1_SAVE_SIZE
            );

    return error;
}

enum pksav_error pksav_gen1_free_save(
    struct pksav_gen1_save* gen1_save_ptr
)
{
    if(!gen1_save_ptr)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }

    struct pksav_gen1_save_internal* internal_ptr = gen1_save_ptr->internal_ptr;
    if(internal_ptr->is_buffer_ours)
    {
        free(internal_ptr->raw_save_ptr);
    }
    free(internal_ptr);

    // Everything else is a pointer or an enum with a default value of 0,
    // so this one memset should be fine.
    memset(
        gen1_save_ptr,
        0,
        sizeof(*gen1_save_ptr)
    );

    return PKSAV_ERROR_NONE;
}

enum pksav_error pksav_gen1_pokemon_storage_set_current_box(
    struct pksav_gen1_pokemon_storage* gen1_pokemon_storage_ptr,
    uint8_t new_current_box_num
)
{
    if(!gen1_pokemon_storage_ptr)
    {
        return PKSAV_ERROR_NULL_POINTER;
    }
    if(new_current_box_num >= PKSAV_GEN1_NUM_POKEMON_BOXES)
    {
        return PKSAV_ERROR_PARAM_OUT_OF_RANGE;
    }

    uint8_t* current_box_num_ptr = gen1_pokemon_storage_ptr->current_box_num_ptr;
    struct pksav_gen1_pokemon_box* current_box_ptr = gen1_pokemon_storage_ptr->current_box_ptr;
    struct pksav_gen1_pokemon_box** box_ptrs = gen1_pokemon_storage_ptr->box_ptrs;

    uint8_t current_box_num = *current_box_num_ptr
                            & PKSAV_GEN1_CURRENT_POKEMON_BOX_NUM_MASK;

    *box_ptrs[current_box_num] = *current_box_ptr;

    *current_box_num_ptr &= ~PKSAV_GEN1_CURRENT_POKEMON_BOX_NUM_MASK;
    *current_box_num_ptr |= new_current_box_num;

    *current_box_ptr = *box_ptrs[new_current_box_num];

    return PKSAV_ERROR_NONE;
}
