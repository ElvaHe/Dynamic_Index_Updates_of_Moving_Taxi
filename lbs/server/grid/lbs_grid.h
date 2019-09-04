#ifndef SERVER_GRID_LBS_GRID_H_
#define SERVER_GRID_LBS_GRID_H_

#include <pthread.h>

#include "server/grid/lbs_defs.h"
#include "server/grid/lbs_hashtable.h"

typedef struct lbs_cell_s{
  lbs_mov_node_t dammy_node;      //每个cell包含一个链表中的头结点，用于建立链表结构（由cell和queue可以确定任何一个grid中的任何一个节点lbs_cell）
  pthread_mutex_t mutex;          //锁
}lbs_cell_t;

typedef struct lbs_grid_s{        //grid,cell,mov_node,queue，在初始化过程中要特别留意结构体的嵌套，以免遗漏变量
  double lon_min;
  double lon_max;
  double lat_min;
  double lat_max;
  double height;
  double width;
  int row_num;
  int col_num;
  lbs_hashtable_t hashtable;      //每辆车id的位置信息mov_node,在grid和hashtable中可以一一对应
  lbs_cell_t *cell;               //建立一维数组，表示grid划分成的每个小格cell（每个小格cell本身是一个链表结构）
}lbs_grid_t;

int lbs_grid_init(lbs_grid_t* lbs_grid,double lon_min,double lon_max,double lat_min,double lat_max,int row_num,int col_num);
int lbs_grid_update(lbs_grid_t* lbs_grid,double lon,double lat, uint32_t id);  //暂时不涉及uint64_t timestamp
int lbs_grid_cell_row(lbs_grid_t* lbs_grid,double lat);
int lbs_grid_cell_col(lbs_grid_t* lbs_grid,double lon);
int lbs_grid_cell_id(lbs_grid_t* lbs_grid,int cell_row,int cell_col);       //grid中col,row,cell_id都是从0开始
//计算row和col
void lbs_grid_cell_row_col(lbs_grid_t* lbs_grid,int cell_id,int *cell_row,int *cell_col);
lbs_cell_t* lbs_grid_cell(lbs_grid_t* lbs_grid,int cell_id);

#endif
