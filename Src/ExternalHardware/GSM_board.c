/*
 * GSM_board.c
 *
 *  Created on: 17 lug 2019
 *      Author: JBFourierous
 */

#include "GSM_board.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

// GSM global variables declaration
uint8_t GSM_response[100];							// buffer for GSM module response
uint8_t GSM_new_response_size = 0;						// size of the expected response of the module
bool GSM_cfun = true;								// flag indicating the working mode of the module

// ----------------------------------------------------- INNER FUNCTIONS ------------------------------------------------------------- //

/** @brief Sends an AT command to the GSM board
 *  @param  identifier of the command
 *  @return none
 */
static void send_AT_command(uint8_t command) {
	switch(command) {
		case AT:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", sizeof("AT\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case CREG:
			GSM_new_response_size = CREG_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CREG?\r\n", sizeof("AT+CREG?\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, CREG_LEN);
			break;
		case CSQ:
			GSM_new_response_size = CSQ_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CSQ\r\n", sizeof("AT+CSQ\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, CSQ_LEN);
			break;
		case CBC:
			GSM_new_response_size = CBC_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CBC\r\n", sizeof("AT+CBC\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, CBC_LEN);
			break;
		case CFUN0:
			if(GSM_cfun) {
				GSM_new_response_size = CFUN0_LEN;
				HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CFUN=0\r\n", sizeof("AT+CFUN=0\r\n"), HAL_MAX_DELAY);
				HAL_UART_Receive_IT(&huart1, GSM_response, CFUN0_LEN);
			} else {
				GSM_new_response_size = OK_LEN;
				HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CFUN=0\r\n", sizeof("AT+CFUN=0\r\n"), HAL_MAX_DELAY);
				HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			}
			break;
		case CFUN1:
			if(GSM_cfun) {
				GSM_new_response_size = OK_LEN;
				HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CFUN=1\r\n", sizeof("AT+CFUN=1\r\n"), HAL_MAX_DELAY);
				HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			} else {
				GSM_new_response_size = CFUN1_LEN;
				HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CFUN=1\r\n", sizeof("AT+CFUN=1\r\n"), HAL_MAX_DELAY);
				HAL_UART_Receive_IT(&huart1, GSM_response, CFUN1_LEN);
			}
			break;
		case CONFSAPBR:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",
					sizeof("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case CONFAPN:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+SAPBR=3,1,\"APN\",\"my_apn\"\r\n",
					sizeof("AT+SAPBR=3,1,\"APN\",\"my_apn\"\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case OPENSAPBR:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+SAPBR=1,1\r\n", sizeof("AT+SAPBR=1,1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPINIT:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPINIT\r\n", sizeof("AT+HTTPINIT\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPPARA_CID:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPPARA=\"CID\",1\r\n", sizeof("AT+HTTPPARA=\"CID\",1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPPARA_URL:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPPARA=\"URL\",\"http://my_server\"\r\n",
					sizeof("AT+HTTPPARA=\"URL\",\"http://my_server\\"\r\n"), HAL_MAX_DELAY);
			// qua va il nostro ricevente
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPPARA_CONTENT:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPPARA=\"CONTENT\",\"text/plain\"\r\n",
					sizeof("AT+HTTPPARA=\"CONTENT\",\"text/plain\"\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPDATA:
			GSM_new_response_size = HTTPDATA_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPDATA=38400,60000\r\n", sizeof("AT+HTTPDATA=38400,60000\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, HTTPDATA_LEN);
			break;
		case HTTPACTION:
			GSM_new_response_size = HTTPACTION_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPACTION=1\r\n", sizeof("AT+HTTPACTION=1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case HTTPTERM:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+HTTPTERM\r\n", sizeof("AT+HTTPTERM\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case CLOSESAPBR:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+SAPBR=0,1\r\n", sizeof("AT+SAPBR=0,1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case DATA:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			// send camera image to the GSM board
			camera_capture_image();
			break;
		case CMGF:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMGF=1\r\n", sizeof("AT+CMGF=1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		case CMGS:
			GSM_new_response_size = CMGS_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMGS=\"+39receiver\"\r\n", sizeof("AT+CMGS=\"+39receiver\"\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, CMGS_LEN);
			break;
		case SMS:
			GSM_new_response_size = SMS_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"Ranger Vervet System emergency: terrestrial threat\x1A",
					sizeof("Ranger Vervet System emergency: terrestrial threat\x1A"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, SMS_LEN);
			break;
		case CPIN:
			GSM_new_response_size = CPIN_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CPIN=my_pin\r\n", sizeof("AT+CPIN=my_pin\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, CPIN_LEN);
			break;
		case CSCS:
			GSM_new_response_size = OK_LEN;
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CSCS=\"GSM\"\r\n", sizeof("AT+CSCS=\"GSM\"\r\n"), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart1, GSM_response, OK_LEN);
			break;
		default:
			break;
	}
}

/** @brief Checks the GSM board response to a command
 *  @param  req_id identifier of the sent command. Depending on it a certain response is expected
 *  @return true if the response received is the expected one, false otherwise
 */
static bool parse_AT_response(uint8_t req_id) {
	char* token;
	const char colon[2] = ":";
	const char comma[2] = ",";
	bool res = false;
	switch(req_id) {
		// The expected response is in the form \r\n+CSQ: xx,yy\r\nOK, where xx is an indication of the signal strength in dB
		case CSQ:
			token = strstr(GSM_response, colon);
			if(token == NULL)
				res = false;
			else if(token[2] == '1' || token[2] == '2' || token[2] == '3')
				res = true;
			else
				res = false;
			break;
		// The expected response is in the form \r\n+CBC: xx,yy,zz\r\nOK, where xx is an indication of the battery level
		case CBC:
			token = strstr(GSM_response, comma);
			if(token == NULL)
				res = false;
			else if(isdigit(token[1]) && token[1] > '3')
				res = true;
			else
				res = false;
			break;
		case CFUN0:
			if(GSM_cfun) {
				if(strncmp(GSM_response, "\r\n+CPIN: NOT READY\r\n\r\nOK\r\n", CFUN0_LEN) == 0)
					res = true;
				else
					res = false;
			} else {
				if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
					res = true;
				else
					res = false;
			}
			GSM_cfun = false;
			break;
		case CFUN1:
			if(GSM_cfun) {
				if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
					res = true;
				else
					res = false;
			} else {
				if(strncmp(GSM_response, "\r\n+CPIN: SIM PIN\r\n\r\nOK\r\n", CFUN1_LEN) == 0)
					res = true;
				else
					res = false;
			}
			GSM_cfun = true;
			break;
		case CONFSAPBR:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CONFAPN:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case AT:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CREG:
			if(strncmp(GSM_response, "\r\n+CREG: 0,1\r\n\r\nOK\r\n", CREG_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case OPENSAPBR:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPINIT:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPPARA_CID:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPPARA_URL:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPPARA_CONTENT:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPDATA:
			if(strncmp(GSM_response, "\r\nDOWNLOAD\r\n", HTTPDATA_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case DATA:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPACTION:
			if(strncmp(GSM_response, "\r\nOK\r\n\r\n+HTTPACTION:1,200", HTTPACTION_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case HTTPTERM:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CLOSESAPBR:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CMGF:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CMGS:
			if(strncmp(GSM_response, "\r\n> ", CMGS_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case SMS:
			if(strstr(GSM_response, "OK") != NULL)
				res = true;
			else
				res = false;
			break;
		case CPIN:
			if(strncmp(GSM_response, "\r\nOK\r\n\r\n+CPIN: READY\r\n\r\nCall Ready\r\n\r\nSMS Ready\r\n", CPIN_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		case CSCS:
			if(strncmp(GSM_response, "\r\nOK\r\n", OK_LEN) == 0)
				res = true;
			else
				res = false;
			break;
		default:
			res = false;
	}
	GSM_new_response = false;
	return res;
}

/** @brief Sends an AT command and waits for the GSM module response, verifyng if correct
 *  @param  req_id identifier of the sent command
 *  @return true if the response received is the expected one, false otherwise
 */
static bool send_and_wait_AT(uint8_t command) {
	// Tries 5 times to send a command. Waits for UART interrupt to notify the GSM module response
	bool res = false;
	uint32_t n = UINT32_MAX;
	uint8_t i = 5;
	do {
		send_AT_command(command);
		while(!GSM_new_response && n)
			n--;
		res = parse_AT_response(command);
	} while(!res && i--);
	return res;
}

// --------------------------------------------------------- END INNER FUNCTIONS ------------------------------------------------------- //


// ----------------------------------------------------- INTERFACE FUNCTIONS ----------------------------------------------------------- //

/** @brief Verifies if the battery power level is below a critical threshold
 *  @param  none
 *  @return true is the power level is ok, false otherwise
 */
static bool GSM_power_status() {
	// Sends the AT command AT+CBC to get the battery charge level
	return send_and_wait_AT(CBC);
}

/** @brief Verifies if the module is correctly registered to network and if the signal power level is over a critical threshold
 *  @param  none
 *  @return true is the board is connected and the signal level is , false otherwise
 */
static bool GSM_signal_status() {
	// Sends the AT command AT+CSQ which gets as response a value in dB for the signal strength
	return send_and_wait_AT(CSQ);
}

/** @brief Soft reset of the module
 *  @param  none
 *  @return true is the reset is successful, false otherwise
 */
static bool GSM_soft_reset() {
	// Turn on and off aero mode to reset the device
	bool res = false;
	res = GSM_mode(false);
	if(res) {
		res = GSM_mode(true);
		if(res) {
			res = send_and_wait_AT(CPIN);
		}
	}
	return res;
}

/** @brief Verifies if the module is in good power and signal status to properly work
 *  @param  none
 *  @return true if the GSM board is correctly working, false otherwise
 */
bool GSM_check() {
	return (GSM_power_status() && GSM_signal_status());
}

/** @brief Initializes the GSM board connecting to the network
 *  @param  none
 *  @return True if GSM module is correectly initialized, false otherwise
 */
bool GSM_init() {
	bool res = false;
	// Establish communication with GSM board using AT command
	res = send_and_wait_AT(AT);
	if(res) {
		// GSM software reset to recover from a possible inconsistent state
		res = GSM_soft_reset();
		if(res) {
			// Check network registration status with AT+CREG
			res = send_and_wait_AT(CREG);
		}
	}
	GSM_cfun = true;
	return res;
}

/** @brief Sends HTTP POST command to a remote server
 *  @param  none
 *  @return true if command is correctly executed, false otherwise
 */
bool GSM_send_HTTP() {
	bool res = true;
	// open bearer configured during initialization
	if(send_and_wait_AT(OPENSAPBR)) {
		// initiate HTTP operation
		if(send_and_wait_AT(HTTPINIT)) {
			// define HTTP, parameters, such as URL and content type
			if(send_and_wait_AT(HTTPPARA_CID)) {
				if(send_and_wait_AT(HTTPPARA_URL)) {
					if(send_and_wait_AT(HTTPPARA_CONTENT)) {
						// define message dimension and send data to GSM module
						if(send_and_wait_AT(HTTPDATA)) {
							// send data and wait for GSM module reception ok
							if(send_and_wait_AT(DATA)) {
								// send HTTP post command
								if(send_and_wait_AT(HTTPACTION)) {
									// transfer successful, close HTTP session and bearer for GPRS connection
									if(send_and_wait_AT(HTTPTERM)) {
										if(send_and_wait_AT(CLOSESAPBR)) {
											res = true;
										} else
											res = false;
									} else
										res = false;
								} else
									res = false;
							} else
								res = false;
						} else
							res = false;
					} else
						res = false;
				} else
					res = false;
			} else
				res = false;
		} else
			res = false;
	} else
		res = false;
	// If any error occurred try to close GPRS connection before return
	if(!res)
		send_and_wait_AT(CLOSESAPBR);
	return res;
}

/** @brief Sets the GSM board in sleep mode or active mode
 *  @param  mode true if active mode, false otherwise
 *  @return true if command is correctly executed, false otherwise
 */
bool GSM_mode(bool mode) {
	// Sends the AT command AT+CFUN=x, where x=1 sets full functionality and x=0 minimum functionality for power saving (aero mode)
	bool res = false;
	if(mode) {
		res = send_and_wait_AT(CFUN1);
	} else {
		res = send_and_wait_AT(CFUN0);
	}
	return res;
}

/** @brief Initializes GPRS parameters for Internet connection
 *  @param  none
 *  @return true if GPRS is correctlty initialized, false otherwise
 */
bool GSM_GPRS_init() {
	bool res = false;
	// Initializes the bearer to connect via GPRS
	res = send_and_wait_AT(CONFSAPBR);
	if(res) {
		// Sets the APN of the mobile operator
		res = send_and_wait_AT(CONFAPN);
	}
	return res;
}

/** @brief Sends SMS to a receiver terminal
 *  @param  none
 *  @return true if SMS is correctly sent, false otherwise
 */
bool GSM_send_SMS() {
	bool res = false;
	// Set text mode for message
	res = send_and_wait_AT(CMGF);
	if(res) {
		// Set character encoding
		res = send_and_wait_AT(CSCS);
		if(res) {
			// Set receiver number and send message
			res = send_and_wait_AT(CMGS);
			if(res) {
				// Send SMS data
				res = send_and_wait_AT(SMS);
			}
		}
	}
	return res;
}

// -------------------------------------------------------- END INTERFACE FUNCTIONS ---------------------------------------------------- //
