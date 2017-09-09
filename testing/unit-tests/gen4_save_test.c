/*
 * Copyright (c) 2017 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "c_test_common.h"
#include "test-utils.h"

#include <pksav/config.h>
#include <pksav/gen4/save.h>

#include <stdio.h>
#include <string.h>

// TODO: replace when size is moved to header
#define GEN4_SAVE_SIZE 0x80000

static void pksav_buffer_is_gen4_save_test(
    const char* subdir,
    const char* save_name,
    pksav_gen4_game_t game
) {
    TEST_ASSERT_NOT_NULL(subdir);
    TEST_ASSERT_NOT_NULL(save_name);

    static char filepath[256];
    static uint8_t save_buffer[GEN4_SAVE_SIZE];
    pksav_error_t error = PKSAV_ERROR_NONE;

    char* pksav_test_saves = getenv("PKSAV_TEST_SAVES");
    if(!pksav_test_saves) {
        TEST_FAIL_MESSAGE("Failed to get test save directory.");
    }

    snprintf(
        filepath, sizeof(filepath),
        "%s%s%s%s%s",
        pksav_test_saves, FS_SEPARATOR, subdir, FS_SEPARATOR, save_name
    );

    if(read_file_into_buffer(filepath, save_buffer, GEN4_SAVE_SIZE)) {
        TEST_FAIL_MESSAGE("Failed to read save into buffer.");
    }

    bool is_buffer_gen4_save = false;
    error = pksav_buffer_is_gen4_save(
                save_buffer,
                GEN4_SAVE_SIZE,
                game,
                &is_buffer_gen4_save
            );
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);
    TEST_ASSERT_TRUE(is_buffer_gen4_save);
}

static void pksav_file_is_gen4_save_test(
    const char* subdir,
    const char* save_name,
    pksav_gen4_game_t game
) {
    TEST_ASSERT_NOT_NULL(subdir);
    TEST_ASSERT_NOT_NULL(save_name);

    static char filepath[256];
    pksav_error_t error = PKSAV_ERROR_NONE;

    char* pksav_test_saves = getenv("PKSAV_TEST_SAVES");
    if(!pksav_test_saves) {
        TEST_FAIL_MESSAGE("Failed to get test save directory.");
    }

    snprintf(
        filepath, sizeof(filepath),
        "%s%s%s%s%s",
        pksav_test_saves, FS_SEPARATOR, subdir, FS_SEPARATOR, save_name
    );

    bool is_file_gen4_save = false;
    error = pksav_file_is_gen4_save(
                filepath,
                game,
                &is_file_gen4_save
            );
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);
    TEST_ASSERT_TRUE(is_file_gen4_save);
}

static void gen4_save_load_and_save_match_test(
    const char* subdir,
    const char* save_name,
    pksav_gen4_game_t game
) {
    TEST_ASSERT_NOT_NULL(subdir);
    TEST_ASSERT_NOT_NULL(save_name);

    static char original_filepath[256];
    static char tmp_save_filepath[256];
    pksav_gen4_save_t gen4_save;
    pksav_error_t error = PKSAV_ERROR_NONE;

    char* pksav_test_saves = getenv("PKSAV_TEST_SAVES");
    if(!pksav_test_saves) {
        TEST_FAIL_MESSAGE("Failed to get test save directory.");
    }

    snprintf(
        original_filepath, sizeof(original_filepath),
        "%s%s%s%s%s",
        pksav_test_saves, FS_SEPARATOR, subdir, FS_SEPARATOR, save_name
    );
    snprintf(
        tmp_save_filepath, sizeof(tmp_save_filepath),
        "%s%spksav_%d_%s",
        get_tmp_dir(), FS_SEPARATOR, get_pid(), save_name
    );

    error = pksav_gen4_save_load(
                original_filepath,
                &gen4_save
            );
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(game, gen4_save.gen4_game);

    error = pksav_gen4_save_save(
                tmp_save_filepath,
                &gen4_save
            );
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);

    pksav_gen4_save_t tmp_save;
    error = pksav_gen4_save_load(
                tmp_save_filepath,
                &tmp_save
            );
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);
    TEST_ASSERT_EQUAL(gen4_save.gen4_game, tmp_save.gen4_game);

    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.pokemon_party, tmp_save.pokemon_party, sizeof(pksav_gen4_pokemon_party_t))
    );
    if(gen4_save.gen4_game == PKSAV_GEN4_HGSS) {
        TEST_ASSERT_EQUAL(0,
            memcmp(gen4_save.pokemon_pc, tmp_save.pokemon_pc, sizeof(gen4_save.pokemon_pc->hgss))
        );
    } else {
        TEST_ASSERT_EQUAL(0,
            memcmp(gen4_save.pokemon_pc, tmp_save.pokemon_pc, sizeof(gen4_save.pokemon_pc->dppt))
        );
    }
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.item_bag, tmp_save.item_bag, sizeof(pksav_gen4_item_bag_t))
    );
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.trainer_name, tmp_save.trainer_name, 16)
    );
    TEST_ASSERT_EQUAL(*gen4_save.trainer_gender, *tmp_save.trainer_gender);
    TEST_ASSERT_EQUAL(*gen4_save.money, *tmp_save.money);
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.rival_name, tmp_save.rival_name, 16)
    );
    TEST_ASSERT_EQUAL(*gen4_save.adventure_started_time, *tmp_save.adventure_started_time);
    TEST_ASSERT_EQUAL(*gen4_save.pokemon_league_champ_time, *tmp_save.pokemon_league_champ_time);
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.total_playtime, tmp_save.total_playtime, sizeof(pksav_date_t))
    );
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.player_coordinates, tmp_save.player_coordinates, sizeof(pksav_coordinates_t))
    );
    if(gen4_save.gen4_game == PKSAV_GEN4_HGSS) {
        TEST_ASSERT_EQUAL(0,
            memcmp(gen4_save.hgss_follower_coordinates, tmp_save.hgss_follower_coordinates, sizeof(pksav_coordinates_t))
        );
    }
    TEST_ASSERT_EQUAL(0,
        memcmp(gen4_save.trainer_card_signature, tmp_save.trainer_card_signature, sizeof(pksav_trainer_card_signature_t))
    );
    TEST_ASSERT_EQUAL(*gen4_save.sinnoh_johto_badges, *tmp_save.sinnoh_johto_badges);
    if(gen4_save.gen4_game == PKSAV_GEN4_HGSS) {
        TEST_ASSERT_EQUAL(*gen4_save.hgss_kanto_badges, *tmp_save.hgss_kanto_badges);
    }

    error = pksav_gen4_save_free(&tmp_save);
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);

    error = pksav_gen4_save_free(&gen4_save);
    TEST_ASSERT_EQUAL(PKSAV_ERROR_NONE, error);

    if(delete_file(tmp_save_filepath)) {
        TEST_FAIL_MESSAGE("Failed to clean up temp file.");
    }
}

static void pksav_buffer_is_diamond_save_test() {
    pksav_buffer_is_gen4_save_test("diamond_pearl", "pokemon_diamond.sav", PKSAV_GEN4_DP);
}

static void pksav_file_is_diamond_save_test() {
    pksav_file_is_gen4_save_test("diamond_pearl", "pokemon_diamond.sav", PKSAV_GEN4_DP);
}

static void diamond_save_load_and_save_match_test() {
    gen4_save_load_and_save_match_test("diamond_pearl", "pokemon_diamond.sav", PKSAV_GEN4_DP);
}

static void pksav_buffer_is_platinum_save_test() {
    pksav_buffer_is_gen4_save_test("platinum", "pokemon_platinum.sav", PKSAV_GEN4_PLATINUM);
}

static void pksav_file_is_platinum_save_test() {
    pksav_file_is_gen4_save_test("platinum", "pokemon_platinum.sav", PKSAV_GEN4_PLATINUM);
}

static void platinum_save_load_and_save_match_test() {
    gen4_save_load_and_save_match_test("platinum", "pokemon_platinum.sav", PKSAV_GEN4_PLATINUM);
}

static void pksav_buffer_is_soulsilver_save_test() {
    pksav_buffer_is_gen4_save_test("heartgold_soulsilver", "pokemon_soulsilver.sav", PKSAV_GEN4_HGSS);
}

static void pksav_file_is_soulsilver_save_test() {
    pksav_file_is_gen4_save_test("heartgold_soulsilver", "pokemon_soulsilver.sav", PKSAV_GEN4_HGSS);
}

static void soulsilver_save_load_and_save_match_test() {
    gen4_save_load_and_save_match_test("heartgold_soulsilver", "pokemon_soulsilver.sav", PKSAV_GEN4_HGSS);
}

PKSAV_TEST_MAIN(
    PKSAV_TEST(pksav_buffer_is_diamond_save_test)
    PKSAV_TEST(pksav_file_is_diamond_save_test)
    PKSAV_TEST(diamond_save_load_and_save_match_test)
    PKSAV_TEST(pksav_buffer_is_platinum_save_test)
    PKSAV_TEST(pksav_file_is_platinum_save_test)
    PKSAV_TEST(platinum_save_load_and_save_match_test)
    PKSAV_TEST(pksav_buffer_is_soulsilver_save_test)
    PKSAV_TEST(pksav_file_is_soulsilver_save_test)
    PKSAV_TEST(soulsilver_save_load_and_save_match_test)
)