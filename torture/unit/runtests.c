/* -*- mode: C -*- Time-stamp: "08/03/17 14:20:55 jemarch"
 *
 *       File:         runtests.c
 *       Date:         Sat Feb 23 21:40:43 2008
 *
 *       GNU PDF Library - Unit testing driver
 *
 */

#include <check.h>

extern Suite *tsuite_alloc (void);
extern Suite *tsuite_stm (void);
extern Suite *tsuite_list (void);

int
main (int argc, char **argv)
{
  int failures;
  SRunner *sr;
  Suite *s;

  s = tsuite_alloc ();

  sr = srunner_create (s);
  srunner_add_suite (sr, tsuite_stm ());
  srunner_add_suite (sr, tsuite_list ());

  srunner_set_log (sr, "ut.log");
  srunner_run_all (sr, CK_VERBOSE);
  failures = srunner_ntests_failed (sr);
  srunner_free (sr);
  
  return (failures == 0) ? 0 : 1;
}

/* End of runtests.c */
