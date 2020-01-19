#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint32_t value;
  char expr[256];

} WP;

WP* new_wp();
void free_wp(int number);
void check_wp();
void print_wp();

#endif
