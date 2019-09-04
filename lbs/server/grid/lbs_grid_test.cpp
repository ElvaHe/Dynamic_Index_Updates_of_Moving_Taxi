#include <stdio.h>
#include <assert.h>

 #include "server/grid/lbs_hashtable.h"

int main(){
  lbs_grid_t lbs_grid;
  int ret = lbs_grid_init(&lbs_grid,116,117,39,41,0,0,200,100);   //init的验证
  assert(ret==0);
  printf("ret = %d\n",ret);

  int ret= lbs_grid_cell_id(lbs_grid,115.5,int 40);
// assert(ret==X) X为期待的值
  printf("ret = %d\n",ret);

//其他函数验证类似

  lbs_harshnode_t *f= lbs_hashtable_get(hashtable,12);
  assert(f==NULL);
  int ret2=lbs_grid_update(lbs_grid,116.5,40 , 12);               //update情况一的验证
  lbs_harshnode_t *f= lbs_hashtable_get(hashtable,12);
  assert(f->cell_id==lbs_grid_cell_id(lbs_grid,lbs_grid_cell_col(lbs_grid,40),lbs_grid_cell_col(lbs_grid,116.5)));
  assert(f->mov_node->lon==116.5&&f->mov_node->col==40);
  assert(ret2==0);
  //还可以添加mov_node是否插入到cell_id链表中的判定
  int ret3=lbs_grid_update(lbs_grid,116.3,40.5 , 12);             //update情况二，三的验证（由输入的lon,col的值即cell_id与cell_id_old是否相同来决定）
  assert(f->cell_id==lbs_grid_cell_id(lbs_grid,lbs_grid_cell_col(lbs_grid,40.5),lbs_grid_cell_col(lbs_grid,116.3)));
  assert(f->mov_node->lon==116.3&&f->mov_node->col==40.5);
  assert(ret3==0);

  return 0;
}




