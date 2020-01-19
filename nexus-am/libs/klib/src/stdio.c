#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  int i,len;
  char out[200];
  va_start(args,fmt);
  vsprintf(out,fmt,args);
  va_end(args);
  len = strlen(out);
  for(i = 0;i < len;i ++)
  {
    _putc(out[i]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char c;
  int flag;
  int field_width;
  int len;
  int num;
  int i,j;
  char *dst = out;
  const char *temp_s;
  char string_number[100];


  for(; *fmt ; fmt++){
      if(*fmt != '%'){
        //locate %
        *dst++ = *fmt;
        continue;
      }
      fmt++;
      flag = 0;
      while(*fmt == ' ' || *fmt == '0'){
      	if(*fmt == ' ')  flag |= 8;
      	else if(*fmt == '0') flag |= 1;
      	fmt++;
      }
      field_width = 0;
      if(*fmt >= '0' && *fmt <= '9')
      {
	      while(*fmt >= '0' && *fmt <= '9'){
		      field_width = field_width*10 + *fmt -'0';
		      fmt++;
	      }
      }else if(*fmt == '*'){
	      fmt++;
	      field_width = va_arg(ap,int);
      }
      switch(*fmt){
        case 's':
	      temp_s = va_arg(ap,char *);
	      len = strlen(temp_s);
	      for(i = 0;i < len;i ++){
		      *dst++ = *temp_s++;
	      }
	      continue;
	      case 'd': break;
        default: break;
      }
      j = 0;
      num = va_arg(ap,int);
      if(num == 0){
	      string_number[j++] = '0';
      }else{
    	  if(num < 0){
    	      *dst++ = '-';
    	      num = -num;
    	  }
    	  while(num){
    	      string_number[j++] = num%10 + '0';
    	      num /= 10;
    	  }
      }
      if(j < field_width){
	      num = field_width - j;
	      c = flag & 1 ? '0' : ' ';
	      while(num--){
		      *dst++ = c;
	      }
      }
      while(j--){
	      *dst++ = string_number[j];
      }
  }
  *dst = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int val;
  va_start(args,fmt);
  val = vsprintf(out,fmt,args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
