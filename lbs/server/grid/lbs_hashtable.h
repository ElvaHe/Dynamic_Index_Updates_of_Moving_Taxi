#ifndef SERVER_GRID_LBS_HASHTABLE_H_
#define SERVER_GRID_LBS_HASHTABLE_H_

#include <pthread.h>

#include "server/grid/lbs_defs.h" //hashtable的作用是给定一辆车的编号id，可以方便的查看到该车在grid中对应的cell_id

typedef struct lbs_hashnode_s{    //hashnode为hashtable中任意一个节点，由hash_node和queue构成的二维数组来决定其位置
  //int cell_id;  
  lbs_queue_t queue;              //链表（hashtable行）
  lbs_mov_node_t *mov_node;     //节点（指向grid中一辆车所对应的节点move_node）
  int cell_id;
}lbs_hashnode_t;
typedef struct lbs_hashtable_s{
  pthread_mutex_t mutex;
  int capacity;                 //定义capacoty个头结点（queue位于链首头结点）
  lbs_hashnode_t *hash_node;    //一维数组（hashtable列）
}lbs_hashtable_t;

int lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable);
int lbs_hashtable_set(lbs_hashtable_t* lbs_hashtable,uint32_t id,lbs_mov_node_t* lbs_mov_node,uint32_t cell_id);
lbs_hashnode_t* lbs_hashtable_get(lbs_hashtable_t* lbs_hashtable,uint32_t id);
#endif
