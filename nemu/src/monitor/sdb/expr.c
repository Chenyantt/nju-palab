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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>

word_t vaddr_read(vaddr_t addr, int len);

enum
{
  TK_NOTYPE = 256,
  TK_EQ,

  /* TODO: Add more token types */
  TK_NE,
  TK_DEREF,
  TK_AND,
  TK_REG,
  TK_HEX,
  TK_DEC,
};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces
    {"\\+", '+'},      // plus
    {"==", TK_EQ},     // equal
    {"!=", TK_NE},
    {"-", '-'},
    {"\\*", '*'},
    {"/", '/'},
    {"\\(", '('},
    {"\\)", ')'},
    {"&&", TK_AND},
    {"\\$\\$?[a-z0-9]+", TK_REG},
    {"0x[0-9a-f]+", TK_HEX},
    {"[0-9]+", TK_DEC},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};
/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        case TK_REG:
        case TK_HEX:
        case TK_DEC:
          if (substr_len >= 32)
            assert(0);
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
        default:
          tokens[nr_token++].type = rules[i].token_type;
        }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int check_parentheses(int start, int end)
{
  int bracket_stk_top = -1;
  int first = 0;
  int bracket_l2r = -1;
  for (int i = start; i <= end; ++i)
  {
    if (tokens[i].type == '(')
      bracket_stk_top++;
    else if (tokens[i].type == ')')
    {
      if (bracket_stk_top == -1)
        return 0;
      else
      {
        if (bracket_stk_top == 0 && first == 0)
          first = 1, bracket_l2r = i;
        bracket_stk_top--;
      }
    }
  }
  if (bracket_stk_top != -1)
    return 0;
  if (tokens[start].type == '(' && tokens[end].type == ')' && bracket_l2r == end)
    return 1;
  else
    return 2;
}

int op_prio_tab[400] = {0};
char op_associate_tab[400] = {0};

uint32_t eval_expr(int start, int end, bool *success)
{
  if (start > end)
  {
    *success = false;
    return 0;
  }
  else if (start == end)
  {
    if (tokens[start].type == TK_REG)
    {
      return isa_reg_str2val(tokens[start].str, success);
    }
    else if (tokens[start].type == TK_DEC || tokens[start].type == TK_HEX)
    {
      return (uint32_t)strtoul(tokens[start].str, NULL, 0);
    }
    else
    {
      *success = false;
      return 0;
    }
  }
  else
  {
    int res = check_parentheses(start, end);
    if (res == 0)
    {
      *success = false;
      return 0;
    }
    else if (res == 1)
    {
      return eval_expr(start + 1, end - 1, success);
    }
    else
    {
      int max_op_priority = 0;
      int max_op_idx = -1;
      int is_in_bracket = 0;
      //&&:5 //== !=:4 //+-:3 */:2 *:1
      for (int i = start; i <= end; ++i)
      {
        switch (tokens[i].type)
        {
        case '(':
          ++is_in_bracket;
          break;
        case ')':
          --is_in_bracket;
          break;
        default:
          if (is_in_bracket)
            continue;
          if (op_prio_tab[tokens[i].type] > max_op_priority)
            max_op_idx = i, max_op_priority = op_prio_tab[tokens[i].type];
          else if (op_prio_tab[tokens[i].type] == max_op_priority && op_associate_tab[tokens[i].type] != 'r')
            max_op_idx = i;
          break;
        }
      }
      if (max_op_idx == -1)
      {
        *success = false;
        return 0;
      }
      switch (tokens[max_op_idx].type)
      {
      case '+':
        return eval_expr(start, max_op_idx - 1, success) + eval_expr(max_op_idx + 1, end, success);
        break;
      case '-':
        return eval_expr(start, max_op_idx - 1, success) - eval_expr(max_op_idx + 1, end, success);
        break;
      case '*':
        return eval_expr(start, max_op_idx - 1, success) * eval_expr(max_op_idx + 1, end, success);
        break;
      case '/':
        uint32_t div = eval_expr(max_op_idx + 1, end, success);
        if (div == 0)
        {
          *success = false;
          return 0;
        }
        else
          return eval_expr(start, max_op_idx - 1, success) / div;
        break;
      case TK_EQ:
        return eval_expr(start, max_op_idx - 1, success) == eval_expr(max_op_idx + 1, end, success);
        break;
      case TK_NE:
        return eval_expr(start, max_op_idx - 1, success) != eval_expr(max_op_idx + 1, end, success);
        break;
      case TK_AND:
        return eval_expr(start, max_op_idx - 1, success) && eval_expr(max_op_idx + 1, end, success);
        break;
      case TK_DEREF:
        assert(max_op_idx == start);
        vaddr_t addr = eval_expr(max_op_idx + 1, end, success);
        return vaddr_read(addr, 4);
        break;
      default:
        break;
      }
    }
  }
  return 0;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for (int i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == '*')
    {
      if (i > 0 && (tokens[i - 1].type == ')' || tokens[i - 1].type == TK_REG || tokens[i - 1].type == TK_HEX || tokens[i - 1].type == TK_DEC))
        continue;
      else
        tokens[i].type = TK_DEREF;
    }
  }

  op_prio_tab[TK_DEREF] = 1;
  op_prio_tab['*'] = op_prio_tab['/'] = 2;
  op_prio_tab['+'] = op_prio_tab['-'] = 3;
  op_prio_tab[TK_EQ] = op_prio_tab[TK_NE] = 4;
  op_prio_tab[TK_AND] = 5;

  op_associate_tab[TK_DEREF] = 'r';

  return eval_expr(0, nr_token - 1, success);
}
