// Software Error Handling
if ((float)(_total_feed_time_ms) < MIN_FEED_DURATION) {
	_status.modulError =_sf_stepper_error::SF_STEPPER_ERR_LOW_FEED_TIME;
	return _status.modulError;
}
else if(_vel_um_s < MIN_FEED_SPEED) {
	_status.modulError = _sf_stepper_error::SF_STEPPER_ERR_LOW_FEED_SPEED;
	return _status.modulError;
}			