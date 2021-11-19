#include "../module.h"

#include <stdio.h>
#include <string.h>

#include <iostream>

using namespace std;

static char buf[4096];
char* ptr = NULL;

int encodeRequest(char* data, int maxLen) {
  //只做一次初始化
  if (ptr == NULL) {
    sprintf(buf, "hello tcp!\n");
    ptr = buf;
  }

  int len = strlen(ptr);

  if (len > maxLen) return -1;

  memcpy(data, ptr, len);

  return len;
}

int decodeResponse(char* data, int len) { return 0; }