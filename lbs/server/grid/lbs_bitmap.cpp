#include "server/grid/lbs_bitmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int set[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static int uset[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

/** 初始化Bitmap **/
int lbs_bitmap_init(lbs_bitmap_t* lbs_bitmap, uint32_t bits_num) {
  lbs_bitmap->bits_num = bits_num;
  lbs_bitmap->bits = (uint8_t*)malloc((bits_num /8 +1 ));
  memset(lbs_bitmap->bits, 0, bits_num / 8 + 1);
  return 0;
}

/** 销毁 **/
int lbs_bitmap_destroy(lbs_bitmap_t* lbs_bitmap) {
  return 0;
}

/** 设置Bit **/
int  lbs_bitmap_setbit(lbs_bitmap_t* lbs_bitmap, uint32_t pos) {
  if (pos >= lbs_bitmap->bits_num)
    return -1;
  lbs_bitmap->bits[pos / 8] |= set[pos % 8];
  return 0;
}

/**取消设置Bit **/
int lbs_bitmap_unsetbit(lbs_bitmap_t* lbs_bitmap, uint32_t pos) {
  if (pos >= lbs_bitmap->bits_num)
    return -1;
  lbs_bitmap->bits[pos / 8] &= uset[pos];
  return 0;
}

/** 判定是否设置Bit **/
int lbs_bitmap_isset(lbs_bitmap_t* lbs_bitmap, uint32_t pos) {
  if (pos >= lbs_bitmap->bits_num)
    return -1;
  if ((lbs_bitmap->bits[pos / 8] & set[pos % 8]) == 0)
    return 0;
  else
    return 1;
}
