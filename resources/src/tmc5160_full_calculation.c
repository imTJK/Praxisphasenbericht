// @see \docs\Modules\TMC5160_Calculations.xlsx
sf_stepper_error_t SF_Stepper_c::calculateRampParameters() {
	// Software Error Handling
	if ((float)(_total_feed_time_ms) < MIN_FEED_DURATION) {
		_status.modulError =_sf_stepper_error::SF_STEPPER_ERR_LOW_FEED_TIME;
		return _status.modulError;
	}
	else if(_vel_um_s < MIN_FEED_SPEED) {
		_status.modulError = _sf_stepper_error::SF_STEPPER_ERR_LOW_FEED_SPEED;
		return _status.modulError;
	}

	float delta_t[5];
	float t=0;
	float dist[6];

	//reset Target Position
	_target_pos_fs = 0;

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

	//if(_total_feed_dis_um == 0) { _total_feed_dis_um = _vel_um_s * _total_feed_time_s; }
	float dist_set_fs =  _total_feed_dis_um/ _um_per_step;

	// Calculate time spent on each part of the profile
	delta_t[0] = t_feed_min_s;       // A1 -> konst.
	delta_t[4] = t_feed_min_s;       // D1 -> konst.
	delta_t[1] = _acc_frac * (total_feed_time_f_s - delta_t[0] - delta_t[4]);  // AMAX
	delta_t[3] = _dec_frac * (total_feed_time_f_s - delta_t[0] - delta_t[4]);   // DMAX
	delta_t[2] = total_feed_time_f_s - (delta_t[0] + delta_t[1] + delta_t[3] + delta_t[4]); // Cruise

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

	// Calculate the relevant values for the ramp profile
	_a1_fs_s2   = v1_fs_s / delta_t[0];
	_amax_fs_s2 = (_vmax_fs_s - v1_fs_s) / delta_t[1];
	_d1_fs_s2   = - (v1_fs_s / delta_t[4]);
	_dmax_fs_s2 = (v1_fs_s - _vmax_fs_s) / delta_t[3];

	_status.modulError = _sf_stepper_error::SF_STEPPER_ERR_NONE;
	return _status.modulError;
}
