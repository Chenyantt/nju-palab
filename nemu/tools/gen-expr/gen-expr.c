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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n){
  return (uint32_t)(rand()%n);
}

int buffer_idx = 0;
char op[4] = {'+','-','*','/'};

int gen_rand_num(){
  uint32_t rand_num = (uint32_t)(rand()%10);
  char tmp_buf[40]={0};
  sprintf(tmp_buf,"%u",rand_num);
  if(strlen(tmp_buf) + buffer_idx >= 65535) return -1;
  strcpy(buf+buffer_idx,tmp_buf);
  buffer_idx += strlen(tmp_buf);
  return 0;
}

static int gen_expr(){
  if(buffer_idx >= 65535) return -1;
  switch (choose(3))
  {
  case 0:
    return gen_rand_num();
    break;
  case 1:
    buf[buffer_idx++] = '(';
    if(gen_expr() == -1) return -1;
    if(buffer_idx >= 65535) return -1;
    buf[buffer_idx++]=')';return 0;
    break;
  case 2:
    if(gen_expr() == -1) return -1;
    if(buffer_idx>=65535) return -1;
    buf[buffer_idx++]=op[choose(4)];
    if(buffer_idx>=65535) return -1;
    if(gen_expr()==-1) return -1;
    return 0;
    break;
  default:
    break;
  }
  return 0;
}


static void gen_rand_expr() {
  buffer_idx=0;
  while(gen_expr()==-1);
  buf[buffer_idx]=0;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    //strcpy(buf, "1/0");

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

   // int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    int ret = system("gcc -w /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    if(ret == -1) continue;;

    printf("%u %s\n", result, buf);
  }
  return 0;
}
