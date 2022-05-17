#ifndef GBFILTER_DEF_H_
#define GBFILTER_DEF_H_

// GB Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int GB_FILTER_R_ADDR = 0x00000000;
const int GB_FILTER_RESULT_ADDR = 0x00000004;
 
// GB mask
const int GBmask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
#endif
