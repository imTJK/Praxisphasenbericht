// Calculate total calculated time
for(uint8_t i = 0; i <= 4; i++) {
    t += delta_t[i];
}
// Max deviation of 3 milliseconds
if(std::max(t, total_feed_time_f_s) - std::min(t, total_feed_time_f_s) >= 0.003) {
    _status.modulError= _sf_stepper_error::SF_STEPPER_ERR_INV_RAMP_CALC;
    return _status.modulError;
}

// Calculate "traveled" distance for each part of the profile
// (serves no "practical" purpose apart from checking the calculation for errors)
dist[0] = (delta_t[0] * v1_fs_s) / 2;
dist[1] = (delta_t[1] * (_vmax_fs_s - v1_fs_s)) / 2;
dist[2] =  delta_t[2] * (_vmax_fs_s - v1_fs_s);
dist[3] = (delta_t[3] * (_vmax_fs_s - v1_fs_s)) / 2;
dist[4] = (delta_t[4] * v1_fs_s) 								/ 2;
dist[5] = (delta_t[1] + delta_t[2] + delta_t[3]) * v1_fs_s;

// Calculate total calculated distance traveled
for(uint8_t i = 0; i <= 5; i++) {
    _target_pos_fs += dist[i];// * _um_per_step;
}

// Max deviation of one fullstep (~109um)
if(std::max(_target_pos_fs, dist_set_fs) - std::min(_target_pos_fs, dist_set_fs) >= 1) {
    _status.modulError = _sf_stepper_error::SF_STEPPER_ERR_INV_RAMP_CALC;
    return _status.modulError;
}