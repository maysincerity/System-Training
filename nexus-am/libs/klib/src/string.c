#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char *p = s;
  while(*p != '\0'){
  	p++;
  }

  return (size_t)(p - s);
}

char *strcpy(char* dst,const char* src) {
  char *sd = dst;
  const char *ss = src;
  while((*sd++ = *ss++) != '\0');

  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  char *sd = dst;
  const char *ss = src;
  while(n>0){
  	n--;
  	if((*sd++ = *ss++) == '\0'){
  		memset(dst, '\0', n);
  		break;
  	}
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  char *sd = dst;
  while(*sd != '\0'){
  	sd++;
  }
  strcpy(sd, src);

  return dst;    
}

int strcmp(const char* s1, const char* s2) {
  unsigned char uc1, uc2;
  while(*s1 != '\0' && *s1 == *s2){
  	s1++;
  	s2++;
  }
  uc1 = (*(unsigned char *) s1);
  uc2 = (*(unsigned char *) s2);

  return ((uc1<uc2)?-1:(uc1>uc2));
}

int strncmp(const char* s1, const char* s2, size_t n) {
  unsigned char uc1, uc2;
  if(n == 0){
  	return 0;
  }
  while(n-- > 0 && *s1 == *s2){
  	if(n == 0 || *s1 == '\0'){
  		return 0;
  	}
  	s1++;
  	s2++;
  }
  uc1 = (*(unsigned char *) s1);
  uc2 = (*(unsigned char *) s2);

  return ((uc1<uc2)?-1:(uc1>uc2));
}

void* memset(void* v,int c,size_t n) {
  unsigned char *us = v;
  unsigned char uc = c;
  while(n-- != 0){
  	*us++ = uc;
  }

  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *dst = out;
  const char *src = in;
  while(n-- != 0){
  	*dst++ = *src++;
  }

  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  const unsigned char *us1 = (const unsigned char *) s1;
  const unsigned char *us2 = (const unsigned char *) s2;
  while(n-- != 0){
  	if(*us1 != *us2){
  		return (*us1 < *us2) ? -1 : +1;
  	}
  	us1++;
  	us2++;
  }
  return 0;
}

#endif
