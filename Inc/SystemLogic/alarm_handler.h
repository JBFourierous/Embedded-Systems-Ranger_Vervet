#ifndef ALARM_HANDLER_H_
#define ALARM_HANDLER_H_

#include "common.h"
#include "motor_driver.h"
#include "speaker_alarm.h"
#include "led_matrix.h"
#include "microphone.h"
#include "GSM_board.h"
#include "camera.h"

void handle_alarm(char alarm_type);

#endif /* ALARM_HANDLER_H_ */
