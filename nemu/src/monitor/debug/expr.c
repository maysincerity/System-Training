#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define EVALUATE_ERR 0x0fffffff
enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NEQ, TK_AND, TK_OR, TK_NOT,
  TK_REG,
  TK_HEX, TK_DEC,
  TK_DEREFERENCE, TK_NEGTIVE

};

static struct rule {
  char *regex;
  int token_type;
  int priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE, 0},    // spaces
  {"\\+", '+', 4},         // plus
  {"\\-", '-', 4},
  {"\\*", '*', 3},
  {"/", '/', 3},
  {"\\(", '(', 1},
  {"\\)", ')', 1},
  {"==", TK_EQ, 7},         // equal
  {"!=", TK_NEQ, 7},       //not equal
  {"&&", TK_AND, 11},       //and
  {"\\|\\|", TK_OR, 12},        //or
  {"!", TK_NOT, 2},        //not
  {"\\$[a-zA-Z]+", TK_REG, 0},   //reg
  {"0[Xx][0-9a-fA-F]+", TK_HEX, 0},
  {"[0-9]+", TK_DEC, 0}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  int priority;
} Token;

Token tokens[1000];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_REG:{
            tokens[nr_token].type = TK_REG;
            strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token += 1;
            break;
          };
          case TK_HEX:{
            tokens[nr_token].type = TK_HEX;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token += 1;
            break;
          };
          case TK_DEC:{
            tokens[nr_token].type = TK_DEC;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token += 1;
            break;
          };
          case TK_NOTYPE: break;
          default: {
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            nr_token += 1;
          };
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  //check *, -
  for(i=0; i<nr_token; i++){
    if(tokens[i].type == '*' && (i == 0 || !(tokens[i-1].type == ')' || tokens[i-1].type == TK_REG || tokens[i-1].type == TK_HEX || tokens[i-1].type == TK_DEC))){
      tokens[i].type = TK_DEREFERENCE;
      // printf("dereference\n");
      tokens[i].priority = 2;
    }
    if(tokens[i].type == '-' && (i == 0 || !(tokens[i-1].type == ')' || tokens[i-1].type == TK_REG || tokens[i-1].type == TK_HEX || tokens[i-1].type == TK_DEC))){
      tokens[i].type = TK_NEGTIVE;
      tokens[i].priority = 2;
    }
  }

  int check_parentheses = 0;
  for(i = 0; i<nr_token; i++){
    if(tokens[i].type == '('){
      check_parentheses++;
    }else if(tokens[i].type == ')'){
      check_parentheses--;
    }
    if(check_parentheses<0){
      return false;
    }
  }
  if(check_parentheses != 0 || tokens[0].type == ')' || tokens[nr_token-1].type == '('){
    return false;
  }

  return true;
}

static bool check_parentheses_status(int start_pos, int end_pos){
  int i = 0;
  int check_parentheses = 0;
  if(!(tokens[start_pos].type == '(' && tokens[end_pos].type == ')')){
    return false;
  }
  for(i = start_pos; i <= end_pos; i++){
    if(tokens[i].type == '('){
      check_parentheses++;
    }else if(tokens[i].type == ')'){
      check_parentheses--;
    }
    if(check_parentheses == 0 && i != end_pos){
      return false;
    }
  }
  return true;
}

static int find_main_operator(int p, int q){
  int i = 0;
  int priority = -1;
  int parentheses_flag = 0;
  int main_operator_pos = 0;

  for(i=p; i<q; i++){
    //imm num
    if(tokens[i].type == TK_HEX || tokens[i].type == TK_DEC){
    }else if(tokens[i].type == '('){
      parentheses_flag++;
    }else if(tokens[i].type == ')'){
      parentheses_flag--;
    }else{
      if(tokens[i].priority >= priority && parentheses_flag == 0){
        priority = tokens[i].priority;
        main_operator_pos = i;
      }
    }
  }

  return main_operator_pos;
}

uint32_t get_reg_value(char *reg_name){
  int i=0;
  for(char *p = reg_name; *p!='\0'; ++p){
    *p = tolower(*p);
  }
  for(i=R_EAX; i<=R_EDI; i++){
    if(strcmp(reg_name, regsl[i]) == 0){
      return reg_l(i);
    }
  }
  for(i=R_AX; i<=R_DI; i++){
    if(strcmp(reg_name, regsw[i]) == 0){
      return reg_w(i);
    }
  }  
  for(i=R_AL; i<=R_BH; i++){
    if(strcmp(reg_name, regsb[i]) == 0){
      return reg_b(i);
    }
  }
  if(strcmp(reg_name, "eip") == 0){
    return cpu.eip;
  }else{
    Assert(0, "Please enter valid reg name\n");
  }
  return 0;
}

uint32_t eval(int p, int q){
  if(p>q){
    printf("Bad expression.\n");
    return EVALUATE_ERR;
  }
  else if(p == q){
    //single token
    assert(tokens[p].type == TK_HEX || tokens[p].type == TK_DEC || tokens[p].type == TK_REG);
    uint32_t num;
    switch(tokens[p].type){
      case TK_HEX:{
        sscanf(tokens[p].str, "%x", &num);
        break;
      };
      case TK_DEC:{
        num = (uint32_t)atoi(tokens[p].str);
        break;
      };
      case TK_REG:{
        num = get_reg_value(tokens[p].str);
        break;
      }
      default:
        Assert(0, "Element is not a number.\n");
    }
    return num;
  }
  else if(check_parentheses_status(p, q)){
    return eval(p+1, q-1);
  }
  else{
    int main_operator_pos = find_main_operator(p, q);
    //handle dereference and negtive num
    if(p == main_operator_pos && (tokens[main_operator_pos].type == TK_DEREFERENCE || tokens[main_operator_pos].type == TK_NEGTIVE)){
      uint32_t val = eval(p+1, q);
      switch(tokens[main_operator_pos].type){
        case TK_DEREFERENCE:
          return vaddr_read(val, 4);
        case TK_NEGTIVE:
          return -val;
        default: break;
      }
    }

    uint32_t val1 = eval(p, main_operator_pos-1);
    uint32_t val2 = eval(main_operator_pos+1, q);
    if(val1 == EVALUATE_ERR || val2 == EVALUATE_ERR){
      return EVALUATE_ERR;
    }
    switch(tokens[main_operator_pos].type){
      case '+':return val1+val2;
      case '-':return val1-val2;
      case '*':return val1*val2;
      case '/':{
        if(val2 == 0){
          return EVALUATE_ERR;
        }else{
          return val1/val2;
        }
      };
      case TK_EQ:return val1==val2;
      case TK_NEQ:return val1!=val2;
      case TK_AND:return val1&&val2;
      case TK_OR:return val1||val2;
      default:return EVALUATE_ERR;
    }

  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    // printf("make token false\n");
    return 0;
  }
  // printf("make token success\n");
  /* TODO: Insert codes to evaluate the expression. */
  uint32_t result = eval(0, nr_token-1);
  if(result != EVALUATE_ERR){
    *success = true;
    // printf("eval true\n");
  }else{
    *success = false;
    // printf("eval false\n");
  }

  if(*success){
    return result;
  }else{
    printf("Evaluation failed\n");
  }

  return 0;
}
