// Convert units from um/s to steps/s
auto total_feed_time_f_s = ((float) _total_feed_time_ms) / 1000;
float v1_fs_s  = (float) _v1_um_s  / _um_per_step;
float vel_fs_s = (float) _vel_um_s / _um_per_step;

// ms to s
float t_feed_min_s = (float) MIN_FEED_DURATION / 1000;

// Calculate VMAX
float _ad_coeff_1 = 2 * t_feed_min_s + (total_feed_time_f_s - 2 * t_feed_min_s) * (_acc_frac + _dec_frac) / 2;
float _ad_coeff_2 = (total_feed_time_f_s - 2 * t_feed_min_s) * (1 - _acc_frac/2 - _dec_frac/2);
_vmax_fs_s = (vel_fs_s * total_feed_time_f_s - v1_fs_s * _ad_coeff_1) / _ad_coeff_2;
