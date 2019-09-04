#include "server/grid/lbs_hashtable.h"

#include <stdio.h>
#include <stdlib.h>

#define CAPACITY  1000000

int lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable){       
  lbs_hashtable->capacity = CAPACITY;
  lbs_hashtable->hash_node = (lbs_hashnode_t*)malloc(lbs_hashtable->capacity*sizeof(lbs_hashnode_t));             //初始化capacity个头结点（hashnode）
  int i;
  for(i = 0; i < lbs_hashtable->capacity; i ++)               //初始化每个头结点的属性
  {
    lbs_hashnode_t *head = lbs_hashtable->hash_node + i;  
    lbs_queue_init(&(head->queue));                           //不用初始化cell_id?mov_node?此时生成的是空节点（空链）
  } 
  pthread_mutex_init(&lbs_hashtable->mutex, NULL);
  return 0;
}

int lbs_hashtable_set(lbs_hashtable_t *lbs_hashtable, uint32_t id, lbs_mov_node_t *lbs_mov_node, uint32_t cell_id)
{                                                             //在hashtable中插入新的节点hashnode
  int pos = 0;
  pos = id % lbs_hashtable->capacity;                         //由id确定该节点在hashtable中的列（一维数组hash_node中的第pos个元素）

  lbs_hashnode_t* hash_node = (lbs_hashnode_t*)malloc(sizeof(lbs_hashnode_t));    //初始化新节点hashnode
  hash_node->cell_id = cell_id;
  hash_node->mov_node = lbs_mov_node;

  lbs_hashnode_t *head = lbs_hashtable->hash_node + pos;

  pthread_mutex_lock(&lbs_hashtable->mutex);
  lbs_queue_insert_head(&(head->queue), &(hash_node->queue));//将新节点直接插入到第pos列头结点之后
  pthread_mutex_unlock(&lbs_hashtable->mutex);

  return 0;
}
lbs_hashnode_t*lbs_hashtable_get(lbs_hashtable_t* lbs_hashtable,uint32_t id)
{                                                             //根据id查找其对应的hashnode在hashtable中的位置
  int pos = 0;
  pos = id % lbs_hashtable->capacity;                         //由id确定列
  lbs_hashnode_t *head = lbs_hashtable->hash_node + pos;      
  lbs_hashnode_t *cur_node = (lbs_hashnode_t *)head->queue.next;    //cur_node为链表中当前节点的下一个节点（hashnode）
  while(cur_node!=head)
  {
    if(cur_node->mov_node->id==id)
    {
      break;                                                  //通过遍历pos列对应链表中的各个节点，找出目的id所对应的节点hashnode
    }

    cur_node = (lbs_hashnode_t *)cur_node->queue.next;
  }                                                           

  if(cur_node == head)
    cur_node = NULL;                                          //若为找到。返回NULL

  return cur_node;
}
