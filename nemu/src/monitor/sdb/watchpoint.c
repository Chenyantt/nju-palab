/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[32];
  uint32_t old_val;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

word_t expr(char *e, bool *success);

void new_wp(char *e, uint32_t val)
{
  if (free_ == NULL)
    assert(0);
  WP *wp = free_;
  free_ = free_->next;
  strcpy(wp->expr, e);
  wp->old_val = val;
  wp->next = head;
  head = wp;
  printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
}

void free_WP(int no)
{
  if (head != NULL && head->NO == no)
  {
    memset(head->expr, 0, sizeof head->expr);
    WP *wp = head;
    head = head->next;
    wp->next = free_;
    free_ = wp;
  }
  else
  {
    WP *pre_WP = head;
    while (pre_WP != NULL && pre_WP->next != NULL)
    {
      if (pre_WP->next->NO != no)
        pre_WP = pre_WP->next;
      else
      {
        WP *wp = pre_WP->next;
        memset(wp->expr, 0, sizeof wp->expr);
        pre_WP->next = wp->next;
        wp->next = free_;
        free_ = wp;
        break;
      }
    }
  }
}

void watch_display()
{
  WP *wp = head;
  puts("Num    What");
  while (wp != NULL)
  {
    printf("%d      %s\n", wp->NO, wp->expr);
    wp = wp->next;
  }
}

bool scan_watchpoints()
{
  WP *wp = head;
  bool flag = false;
  while (wp != NULL)
  {
    bool success = true;
    uint32_t new_val = expr(wp->expr, &success);
    assert(success == true);
    if (new_val != wp->old_val)
    {
      Log("Watchpoint %d: %s changed\nOld value = %u\nNew value = %u\n",
          wp->NO, wp->expr, wp->old_val, new_val);
      wp->old_val = new_val;
      flag = true;
    }
    wp = wp->next;
  }
  return flag;
}