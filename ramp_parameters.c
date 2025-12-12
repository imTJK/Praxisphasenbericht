// Calculate time spent on each part of the profile
delta_t[0] = t_feed_min_s;       // A1 -> konst.
delta_t[4] = t_feed_min_s;       // D1 -> konst.
delta_t[1] = _acc_frac * (total_feed_time_f_s - delta_t[0] - delta_t[4]);  // AMAX
delta_t[3] = _dec_frac * (total_feed_time_f_s - delta_t[0] - delta_t[4]);   // DMAX
delta_t[2] = total_feed_time_f_s - (delta_t[0] + delta_t[1] + delta_t[3] + delta_t[4]); // Cruise

// Calculate the relevant values for the ramp profile
_a1_fs_s2   = v1_fs_s / delta_t[0];
_amax_fs_s2 = (_vmax_fs_s - v1_fs_s) / delta_t[1];
_d1_fs_s2   = - (v1_fs_s / delta_t[4]);
_dmax_fs_s2 = (v1_fs_s - _vmax_fs_s) / delta_t[3];