#ifndef __POST_PROCESS__
#define __POST_PROCESS__



void threshold(int threshold_value, void *gray_img, void *thres_img,
	       int block_size = 7, double std_desv = 5);



#endif
