#include"server/grid/lbs_defs.h"
#include"server/grid/lbs_grid.h"
#include"server/grid/lbs_hashtable.h"
#include<stdio.h>
#include<stdlib.h>

int lbs_grid_init(lbs_grid_t* lbs_grid,
                  double lon_min,
                  double lon_max,
                  double lat_min,
                  double lat_max,
                  int row_num,
                  int col_num){
  lbs_grid->lon_min=lon_min;
  lbs_grid->lon_max=lon_max;
  lbs_grid->lat_min=lat_min;
  lbs_grid->lat_max=lat_max;
  lbs_grid->row_num=row_num;
  lbs_grid->col_num=col_num;
  lbs_grid->height=(lat_max-lat_min)/row_num;       //多余？
  lbs_grid->width=(lon_max-lon_min)/col_num;
  lbs_hashtable_init(&(lbs_grid->hashtable));      //加入&的原因是，lbs_grid->hashtable类型是变量，而lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable) 中形参类型是指针，所以变量要取地址
  lbs_grid->cell=(lbs_cell_t*)malloc(row_num*col_num*sizeof(lbs_cell_t));
  int i;
  for(i=0;i<row_num*col_num;i++){                 //初始化每一个cell_id链
    lbs_cell_t *head= lbs_grid->cell+i;
    lbs_queue_init(&(head->dammy_node.queue));
    pthread_mutex_init(&head->mutex, NULL);
  }
  return 0;
}

int lbs_grid_update(lbs_grid_t* lbs_grid,double lon,double lat, uint32_t id){           //更新一辆车（id）的位置信息（Lon,lat）
  int cell_id=lbs_grid_cell_id(lbs_grid,lbs_grid_cell_row(lbs_grid,lat),lbs_grid_cell_col(lbs_grid,lon) );      //由lon,lat确定cell_id
  lbs_hashnode_t * hashnode = lbs_hashtable_get(&(lbs_grid->hashtable),id);               //在hashtable中根据id查找该车辆之前的位置信息mov_node

  if(hashnode == NULL)                                                                  //情况一：hashtable中不存在该车的位置信息
  {
    lbs_mov_node_t *lbs_mov_node =(lbs_mov_node_t*)malloc(sizeof(lbs_mov_node_t));      //新建一个mov_node用于保存该车的位置信息
    lbs_mov_node->lon=lon;
    lbs_mov_node->lat=lat;
    lbs_mov_node->id=id;
    
    lbs_cell_t *head=lbs_grid->cell+cell_id;                                          
    pthread_mutex_lock(&(head->mutex));
    lbs_queue_insert_head(&(head->dammy_node.queue),&(lbs_mov_node->queue));           //将该车的mov_node插入到cell_id对应的链表中
    pthread_mutex_unlock(&(head->mutex));
    lbs_hashtable_set(&(lbs_grid->hashtable),id,lbs_mov_node,cell_id);                  //将该车的mov_node插入到hashtable中
  }
  else
  {                                                                                     //情况二：hashtable中存在该车的位置信息，且cell_id未发生改变
    int cell_id_old = hashnode->cell_id;
    hashnode->mov_node->lon=lon;
    hashnode->mov_node->lat=lat;                                                        //这时只需更新mov_node中的lon,lat信息

    if(cell_id_old != cell_id  )                                                        //情况三：hashtable中存在该车的位置信息，且cell_id发生改变
    {
      hashnode->cell_id=cell_id;
      lbs_cell_t *head=lbs_grid->cell+cell_id_old;
      pthread_mutex_lock(&(head->mutex));
      lbs_queue_remove(&(hashnode->mov_node->queue));                                          //此时需要将原先cell_id_old链中的该节点删除
      pthread_mutex_unlock(&(head->mutex)); 

      head=lbs_grid->cell+cell_id;
      pthread_mutex_lock(&(head->mutex));
      lbs_queue_insert_head(&(head->dammy_node.queue),&(hashnode->mov_node->queue));   //并将该节点插入到cell_id链中
      pthread_mutex_unlock(&(head->mutex));
    }
    
    
  }
  return 0;
}


int lbs_grid_cell_row(lbs_grid_t* lbs_grid,double lat){                           //给定Lat确定行号cell_row
   return((lat-lbs_grid->lat_min)/lbs_grid->height);
}         

int lbs_grid_cell_col(lbs_grid_t* lbs_grid,double lon){                           //给定lon确定列号cell_col
   return((lon-lbs_grid->lon_min)/lbs_grid->width);
}

int lbs_grid_cell_id(lbs_grid_t* lbs_grid,int cell_row,int cell_col){             //由cell_row和cell_col确定cell_id
   return(lbs_grid->col_num*cell_row+cell_col);
}

void lbs_grid_cell_row_col(lbs_grid_t* lbs_grid,int cell_id,int *cell_row,int *cell_col){         //给定cell_id，确定其所在的行号，列号
   *cell_row=cell_id/lbs_grid->col_num;
   *cell_col=cell_id%lbs_grid->col_num;
}

lbs_cell_t* lbs_grid_cell(lbs_grid_t* lbs_grid,int cell_id){            //给定cell_id，返回其对应的cell链表中的头结点的指针
   lbs_cell_t *head=lbs_grid->cell+cell_id;
   return head;                                                   

}







