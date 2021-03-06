#ifndef _DIMMER_H
#define _DIMMER_H

extern void dimmer_init();
extern void set_dimmer(uint8_t channel, uint8_t bright);

extern uint8_t get_channel_status();

extern uint8_t get_channel_val(uint8_t chan);

extern uint8_t dim_vals_8bit[4];

#endif // _CAN_HANDLER

