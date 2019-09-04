#include "server/grid/lbs_defs.h"
#include "server/grid/lbs_nn_heap.h"

#include<stdlib.h>

#define CAPACITY 100

int lbs_nnheap_init(lbs_nnheap_t* lbs_nnheap){
  lbs_nnheap->capacity=CAPACITY;
  lbs_nnheap->size=0;
  lbs_nnheap->heap_nodes= (lbs_heapnode_t*)malloc(lbs_nnheap->capacity*sizeof(lbs_heapnode_t));
  return 0;
}

int lbs_nnheap_destroy(lbs_nnheap_t* lbs_nnheap){
  free(lbs_nnheap->heap_nodes);
  return 0;
}

int lbs_nnheap_insert(lbs_nnheap_t* lbs_nnheap, lbs_mov_node_t* lbs_mov_node, int cell_id, uint8_t is_grid, double distance){     //插入元素到堆的最后一个节点，并“筛选”排序
  if(lbs_nnheap->size >= lbs_nnheap->capacity)
  {
    lbs_nnheap->capacity = lbs_nnheap->capacity * 2;
    lbs_nnheap->heap_nodes = (lbs_heapnode_t*)realloc(lbs_nnheap->heap_nodes,lbs_nnheap->capacity);
  }
  // 修改 by largestone
  lbs_nnheap->size++;     
  lbs_heapnode_t* insert = lbs_nnheap->heap_nodes + lbs_nnheap->size - 1;
  insert->distance       = distance;
  insert->is_grid        = is_grid;
  insert->cell_id        = cell_id;
  insert->node           = lbs_mov_node;

  int insert_id = lbs_nnheap->size;
  while (insert_id > 1)
  {
    int parent_id = insert_id / 2;
    if(lbs_nnheap->heap_nodes[insert_id - 1].distance < lbs_nnheap->heap_nodes[parent_id - 1].distance)
    {
      switch_node(lbs_nnheap->heap_nodes + insert_id - 1,
                  lbs_nnheap->heap_nodes + parent_id - 1);
      insert_id=parent_id;
    }
    else 
      break;
  }

  return 0;
}

void switch_node(lbs_heapnode_t* node1,lbs_heapnode_t* node2){
  lbs_heapnode_t tmp =*node1;
  *node1=*node2;
  *node2=tmp;
}

lbs_heapnode_t* lbs_nnheap_top(lbs_nnheap_t* lbs_nnheap){     
  if(lbs_nnheap->size > 0 )
    return lbs_nnheap->heap_nodes;
  else
    return NULL;
  
}

void lbs_nnheap_pop(lbs_nnheap_t* lbs_nnheap){  //删除堆顶，并对剩余元素重新建堆
   lbs_nnheap->heap_nodes[0] = lbs_nnheap->heap_nodes[lbs_nnheap->size - 1];
   lbs_nnheap->size--;

   int parent_id = 1;
  
   while (parent_id <= lbs_nnheap->size / 2)
   {
     int lchild_id = 2 * parent_id;
     int rchild_id = 2 * parent_id + 1;
     int min = lchild_id;                   //即使只有一个孩子，也可以将其赋给min

     if (rchild_id <= lbs_nnheap->size)
     {
        if(lbs_nnheap->heap_nodes[lchild_id -1].distance > lbs_nnheap->heap_nodes[rchild_id - 1].distance)
          min = rchild_id;
     }

     if(lbs_nnheap->heap_nodes[parent_id -1].distance > lbs_nnheap->heap_nodes[min -1].distance){
        switch_node(lbs_nnheap->heap_nodes + parent_id -1, lbs_nnheap->heap_nodes + min -1);
        parent_id = min;
      }
      else
        break;
  }
}
