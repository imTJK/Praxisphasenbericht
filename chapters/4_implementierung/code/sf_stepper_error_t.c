// SF-Stepper Software Error enum
typedef enum _sf_stepper_error
{
	SF_STEPPER_ERR_NONE,	       // Init Error, invalid Device ID
	SF_STEPPER_ERR_COM,	       // SPI Communication Error
	SF_STEPPER_ERR_INV_RAMP_CALC,  // Invalid Ramp-Profile Calculation
	SF_STEPPER_ERR_LOW_FEED_TIME,  // Invalid feed time
	SF_STEPPER_ERR_LOW_FEED_SPEED, // Invalid feed speed
} sf_stepper_error_t;