/*
 TIMER0 CAP0 -> Enable Timer2(H_SYNC_WAIT) -> call setupTimer2for h_pixel_clk
 TIMER0 CAP1 -> Enable Timer3(V_SYNC_WAIT) -> call setupTimer3for v_pixel_clk

 */
#include "types.h"


typedef struct
{
  uint8_t h_sync_wait_perc; //percentage of viewfield to wait after H_SYNC
  uint8_t v_sync_wait_perc; //percentage of viewfield to wait after V_SYNC

  uint8_t h_pixel_perc; //percentage of viewfield
  uint8_t v_pixel_perc; //percentage of viewfield
	
  uint8_t h_sync_wait_pre; //prescaler for waiting after H_SYNC received
  uint16_t h_sync_wait; //time for waiting after H_SYNC received
	
  uint8_t v_sync_wait_pre; //prescaler for waiting after V_SYNC received
  uint16_t v_sync_wait; //time for waiting after V_SYNC received
	
  uint16_t h_pixel_clk_pre; //prescaler for h pixel clock
  uint16_t h_pixel_end_val; //reset & stop counter here (h pixel counter)
	
  uint16_t v_pixel_clk_pre; //prescaler for v pixel clock
  uint16_t v_pixel_end_val; //reset counter here (v pixel counter)
	
  uint32_t h_line_time;	
  uint32_t h_line_time_avr;

  uint32_t h_line_cnt;
  uint32_t h_line_cnt_avr;
  
  uint32_t frame_time;
  uint32_t frame_time_avr;

  uint8_t frame_cnt;	//frame counter
  uint8_t frame_cnt_avr; //average frame counter
	
  uint8_t videobuffer[VIDEO_BUFFER_SIZE];
	
} Videoengine_t;


volatile Videoengine_t ve;

//function prototypes

void ve_init(void);

void ve_setpixel(uint16_t h_pos,uint16_t v_pos, uint8_t pixel);


void ve_set_new_resolution_v(uint16_t res_v);

void ve_set_new_resolution_h(uint16_t res_h);

void ve_set_new_sync_wait_h(uint16_t sync_h);

void ve_set_new_sync_wait_v(uint16_t sync_v);

void ve_set_pixel_perc_h(uint16_t pixel_h);

void ve_set_pixel_perc_v(uint16_t pixel_v);

void ve_calc_h_pixel_clk(void);

void ve_calc_v_pixel_clk(void);
