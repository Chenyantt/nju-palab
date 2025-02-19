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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
// word_t expr(char *e, bool *success);

// void check_eval_expr()
// {
//   char buf[65536] = {0};
//   FILE *fp = fopen("../nemu/tools/gen-expr/build/input", "r");
//   assert(fp != NULL);

//   uint32_t result;
//   while (fscanf(fp, "%u %s\n", &result, buf) != -1)
//   {
    
//     bool success = true;
//     uint32_t ret = expr(buf, &success);
//     if (ret == result){
//       //printf("True\n");
//     }
//     else{
//       printf("False ");
//       printf("result=%u my_result=%u expr=%s\n", result, ret, buf);
//     }
//     printf("result=%u my_result=%u expr=%s\n", result, ret, buf);
//     memset(buf, 0,sizeof buf);
//   }
//   fclose(fp);
// }

int main(int argc, char *argv[])
{
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
  //check_eval_expr();
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
