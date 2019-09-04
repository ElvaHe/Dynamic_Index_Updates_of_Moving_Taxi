#include <stdio.h>
#include <assert.h>

#include "server/grid/lbs_hashtable.h"

int main(){
  lbs_hashtable_t hashtable;
  int ret = lbs_hashtable_init(&hashtable);
  assert(ret == 0);               //测试lbs_hashtable_init()函数的正确性
  printf("ret = %d\n",ret);

  lbs_mov_node_t node;
  node.id = 11;
  lbs_hashtable_set(&hashtable, 11, &node, 12);
  lbs_hashnode_t* f = lbs_hashtable_get(&hashtable, 11);
  assert(f->mov_node == &node);   //测试lbs_hashtable_set(),lbs_hashtable_get()函数的正确性
  f = lbs_hashtable_get(&hashtable, 12);
  assert(f == NULL);
  return 0;
  }
