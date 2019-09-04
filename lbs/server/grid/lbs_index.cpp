#include "server/grid/lbs_index.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/log.h"

#include "server/grid/lbs_hashtable.h"
#include "server/grid/lbs_grid.h"
#include "server/grid/lbs_distance.h"

#define LBS_LON_MIN 116
#define LBS_LON_MAX 117
#define LBS_LAT_MIN 39
#define LBS_LAT_MAX 41

#define LBS_ROW_NUM 200
#define LBS_COL_NUM 100

void lbs_nnheap_other_cell_insert(lbs_nnheap_t* lbs_nnheap,
                                  lbs_bitmap_t* lbs_bitmap,
                                  double lon,
                                  double lat);
static lbs_grid_t lbs_grid;

// 初始化网格索引
int lbs_grid_index_init() {
  lbs_grid_init(&lbs_grid,
                LBS_LON_MIN,
                LBS_LON_MAX,
                LBS_LAT_MIN,
                LBS_LAT_MAX,
                LBS_ROW_NUM,
                LBS_COL_NUM);
  return 0;
}

// 更新接口[出租车位置更新]
int lbs_grid_index_update(double lon,double lat,uint64_t timestamp,uint32_t id) {
  lbs_grid_update(&lbs_grid,lon,lat,id);
  return 0;
}

// 范围查询接口[查询某一范围内的所有出租车信息]
int lbs_grid_index_range_query(double lon1,double lon2,double lat1,double lat2,lbs_res_node_t* out) {
  int row1 = lbs_grid_cell_row(&lbs_grid,lat1);
  int row2 = lbs_grid_cell_row(&lbs_grid,lat2);
  int col1 = lbs_grid_cell_col(&lbs_grid,lon1);
  int col2 = lbs_grid_cell_col(&lbs_grid,lon2);
  int i = 0;
  int j = 0;
  for(i = row1; i <= row2; i++)
    for(j = col1; j <= col2; j++){
      int cell_id = lbs_grid_cell_id(&lbs_grid,i,j);          //求得包含在（row1,col1）,(row2,col2)中所有的cell_id链
      lbs_cell_t* cell = lbs_grid_cell(&lbs_grid, cell_id);
      lbs_mov_node_t* head = &cell->dammy_node;               //不同结构体指针直接赋值？
      lbs_mov_node_t* cur = (lbs_mov_node_t*)head->queue.next;
      while (cur != head) {
        if(cur->lon > lon1 && cur->lon < lon2 && cur->lat > lat1 && cur->lat < lat2){       //遍历cell_id链中的所有节点，将在范围内的节点插入到res_node中
          lbs_res_node_t* res_node = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
          res_node->lon = cur->lon;
          res_node->lat = cur->lat;
          res_node->id = cur->id;
          lbs_queue_insert_head(&(out->queue), &(res_node->queue));
        }
        cur = (lbs_mov_node_t*)cur->queue.next;
      }
  }
  // TODO: by student
  return 0;
}

// NN查询接口[查询离lon,lat最近的出租车]
int lbs_grid_index_nn_query(double lon, double lat, lbs_res_node_t* out) {
  int cell_id = lbs_grid_cell_id(&lbs_grid, lbs_grid_cell_row(&lbs_grid, lat), lbs_grid_cell_col(&lbs_grid, lon));
  lbs_nnheap_t lbs_nnheap;
  uint32_t bits_num = lbs_grid.row_num * lbs_grid.col_num;

  lbs_bitmap_t lbs_bitmap;
  lbs_nnheap_init(&lbs_nnheap);
  lbs_bitmap_init(&lbs_bitmap, bits_num);

  lbs_nnheap_insert(&lbs_nnheap, &(lbs_grid.cell + cell_id)->dammy_node, cell_id, 1, 0);
  lbs_bitmap_setbit(&lbs_bitmap, cell_id);

  lbs_heapnode_t *top_node = lbs_nnheap_top(&lbs_nnheap);
  int is_grid = top_node->is_grid;
  lbs_nnheap_pop(&lbs_nnheap);

  while (is_grid == 1)
  {
    lbs_mov_node_t *head = &(lbs_grid.cell + cell_id)->dammy_node;
    lbs_mov_node_t *cur_node = (lbs_mov_node_t*)head->queue.next;
    while (cur_node != head)
    {
      lbs_nnheap_insert(&lbs_nnheap, cur_node, cell_id, 0, lbs_distance(cur_node->lon, cur_node->lat, lon, lat));
      cur_node = (lbs_mov_node_t*)cur_node->queue.next;
    }
    lbs_nnheap_other_cell_insert(&lbs_nnheap, &lbs_bitmap, lon, lat);

    top_node = lbs_nnheap_top(&lbs_nnheap);
    is_grid = top_node->is_grid;
    cell_id = top_node->cell_id;
    lbs_nnheap_pop(&lbs_nnheap);
  }

  lbs_res_node_t *res = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
  lbs_queue_insert_head(&(out->queue), &(res->queue));
  res->lon = top_node->node->lon;
  res->lat = top_node->node->lat;
  return 0;
}

void lbs_nnheap_other_cell_insert(lbs_nnheap_t* lbs_nnheap, lbs_bitmap_t* lbs_bitmap, double lon, double lat) {
  int row = lbs_grid_cell_row(&lbs_grid, lat);
  int col = lbs_grid_cell_col(&lbs_grid, lon);

  int cell_id[] = { lbs_grid_cell_id(&lbs_grid, row + 1, col - 1),
                    lbs_grid_cell_id(&lbs_grid, row + 1, col),
                    lbs_grid_cell_id(&lbs_grid, row + 1, col + 1),
                    lbs_grid_cell_id(&lbs_grid, row, col - 1),
                    lbs_grid_cell_id(&lbs_grid, row, col + 1),
                    lbs_grid_cell_id(&lbs_grid, row - 1, col - 1),
                    lbs_grid_cell_id(&lbs_grid, row - 1, col),
                    lbs_grid_cell_id(&lbs_grid, row - 1, col + 1) };
  
  double clon[] = { lbs_grid.lon_min + col * lbs_grid.width,
                  lon,
                  lbs_grid.lon_min + (col + 1) * lbs_grid.width,
                  lbs_grid.lon_min + col * lbs_grid.width,
                  lbs_grid.lon_min + (col + 1) * lbs_grid.width,
                  lbs_grid.lon_min + col * lbs_grid.width,
                  lon,
                  lbs_grid.lon_min + (col + 1) * lbs_grid.width };
  
  double clat[] = { lbs_grid.lat_min + (row + 1) * lbs_grid.height,
                  lbs_grid.lat_min + (row + 1) * lbs_grid.height,
                  lbs_grid.lat_min + (row + 1) * lbs_grid.height,
                  lat,
                  lat,
                  lbs_grid.lat_min + row * lbs_grid.height,
                  lbs_grid.lat_min + row * lbs_grid.height,
                  lbs_grid.lat_min + row * lbs_grid.height };
  
  double distance[] = { lbs_distance(lon, lat, clon[0], clat[0]),
                        lbs_distance(lon, lat, clon[1], clat[1]),
                        lbs_distance(lon, lat, clon[2], clat[2]),
                        lbs_distance(lon, lat, clon[3], clat[3]),
                        lbs_distance(lon, lat, clon[4], clat[4]),
                        lbs_distance(lon, lat, clon[5], clat[5]),
                        lbs_distance(lon, lat, clon[6], clat[6]),
                        lbs_distance(lon, lat, clon[7], clat[7]) };
  int i = 0;
  for(i = 0; i < 8; i ++)
  if(lbs_bitmap_isset(lbs_bitmap, cell_id[i]) == 0)
  {
      lbs_nnheap_insert(lbs_nnheap, &(lbs_grid.cell+cell_id[i])->dammy_node, cell_id[i], 1, distance[i]);
      lbs_bitmap_setbit(lbs_bitmap, cell_id[i]);
  }
}
