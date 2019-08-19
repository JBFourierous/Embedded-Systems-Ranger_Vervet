/*
 * GSM_board.h
 *
 *  Created on: 17 lug 2019
 *      Author: JBFourierous
 */

#ifndef GSM_BOARD_H_
#define GSM_BOARD_H_

#include "common.h"
#include "stdbool.h"
#include "camera.h"
#include "usart.h"

// Basic network commands
#define CSQ 0
#define CBC 1
#define CFUN1 2
#define CFUN0 3
#define AT 5
#define CREG 6
#define CPIN 22

// GPRS commands
#define CONFSAPBR 7
#define CONFAPN 8
#define OPENSAPBR 9
#define HTTPINIT 10
#define HTTPPARA_CID 11
#define HTTPPARA_URL 12
#define HTTPPARA_CONTENT 13
#define HTTPDATA 14
#define HTTPACTION 15
#define HTTPTERM 16
#define CLOSESAPBR 17
#define DATA 18

// SMS commands
#define CMGF 19
#define CMGS 20
#define SMS 21
#define CSCS 23

// Response messages length
#define OK_LEN 6
#define CREG_LEN 20
#define CBC_LEN 22
#define CSQ_LEN 20
#define HTTPDATA_LEN 12
#define HTTPACTION_LEN 26
#define CMGS_LEN 4
#define SMS_LEN 17
#define CPIN_LEN 49
#define CFUN0_LEN 26
//#define CFUN1_LEN 49
#define CFUN1_LEN 24

bool GSM_init();
bool GSM_GPRS_init();
bool GSM_check();
bool GSM_send_HTTP();
bool GSM_send_SMS();
bool GSM_mode(bool mode);

#endif /* GSM_BOARD_H_ */
