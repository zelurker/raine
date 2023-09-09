
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __YMF278B_H__
#define __YMF278B_H__

#define MAX_YMF278B	(2)

#define YMF278B_STD_CLOCK (33868800)                  /* standard clock for OPL4 */

struct YMF278B_interface {
	int num;        								/* Number of chips */
  int clock[MAX_YMF278B];
	int region[MAX_YMF278B];						/* memory region of sample ROMs */
	int mixing_level[MAX_YMF278B];					/* volume */
	void (*irq_callback[MAX_YMF278B])(int state);	/* irq callback */
};

int  YMF278B_sh_start( const struct YMF278B_interface *msound );
void YMF278B_sh_stop( void );

READ8_HANDLER( YMF278B_status_port_0_r );
READ8_HANDLER( YMF278B_data_port_0_r );
WRITE8_HANDLER( YMF278B_control_port_0_A_w );
WRITE8_HANDLER( YMF278B_data_port_0_A_w );
WRITE8_HANDLER( YMF278B_control_port_0_B_w );
WRITE8_HANDLER( YMF278B_data_port_0_B_w );
WRITE8_HANDLER( YMF278B_control_port_0_C_w );
WRITE8_HANDLER( YMF278B_data_port_0_C_w );

READ8_HANDLER( YMF278B_status_port_1_r );
READ8_HANDLER( YMF278B_data_port_1_r );
WRITE8_HANDLER( YMF278B_control_port_1_A_w );
WRITE8_HANDLER( YMF278B_data_port_1_A_w );
WRITE8_HANDLER( YMF278B_control_port_1_B_w );
WRITE8_HANDLER( YMF278B_data_port_1_B_w );
WRITE8_HANDLER( YMF278B_control_port_1_C_w );
WRITE8_HANDLER( YMF278B_data_port_1_C_w );

UINT8 ymf278b_r(int num, UINT32 offset);
void ymf278b_w(int num, UINT32 offset, UINT8 data);

#endif

#ifdef __cplusplus
}
#endif
