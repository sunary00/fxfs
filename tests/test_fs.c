
#include <check.h>
#include "fs.h"


START_TEST ( test_init_super_block )
{
    initialize_superblock();
    fxfs_context* ctx = fxfs_get_context();

    ck_assert(NULL != ctx->superblock);
} END_TEST


Suite* 
fs_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("fxfs");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_init_super_block);
    suite_add_tcase(s, tc_core);

    // Init context
    fxfs_init_context();

    return s;
}


 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = fs_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
