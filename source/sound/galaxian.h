
int galaxian_sh_start();
void galaxian_sh_stop(void);
void galaxian_sh_update();
WRITE_HANDLER( galaxian_shoot_enable_w );
WRITE_HANDLER( galaxian_noise_enable_w );
WRITE_HANDLER( galaxian_vol_w );
WRITE_HANDLER( galaxian_pitch_w );
WRITE_HANDLER( galaxian_background_enable_w );
WRITE_HANDLER( galaxian_lfo_freq_w );
void galaxian_noise_timer_cb(int param);
void galaxian_lfo_timer_cb(int param);

