/* 
 * SPLPv1.c
 * The file is part of practical task for System programming course. 
 * This file contains validation of SPLPv1 protocol. 
 */

/*
  Рудой Андрей Игоревич
  13 группа
*/



/*
---------------------------------------------------------------------------------------------------------------------------
# |      STATE      |         DESCRIPTION       |           ALLOWED MESSAGES            | NEW STATE | EXAMPLE
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
1 | INIT            | initial state             | A->B     CONNECT                      |     2     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
2 | CONNECTING      | client is waiting for con-| A<-B     CONNECT_OK                   |     3     |
  |                 | nection approval from srv |                                       |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
3 | CONNECTED       | Connection is established | A->B     GET_VER                      |     4     |                     
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          One of the following:        |     5     |                      
  |                 |                           |          - GET_DATA                   |           |                      
  |                 |                           |          - GET_FILE                   |           |                      
  |                 |                           |          - GET_COMMAND                |           |
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          GET_B64                      |     6     |                      
  |                 |                           |        ------------------------------------------------------------------
  |                 |                           |          DISCONNECT                   |     7     |                                 
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
4 | WAITING_VER     | Client is waiting for     | A<-B     VERSION ver                  |     3     | VERSION 2                     
  |                 | server to provide version |          Where ver is an integer (>0) |           |                      
  |                 | information               |          value. Only a single space   |           |                      
  |                 |                           |          is allowed in the message    |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
5 | WAITING_DATA    | Client is waiting for a   | A<-B     CMD data CMD                 |     3     | GET_DATA a GET_DATA 
  |                 | response from server      |                                       |           |                      
  |                 |                           |          CMD - command sent by the    |           |                      
  |                 |                           |           client in previous message  |           |                      
  |                 |                           |          data - string which contains |           |                      
  |                 |                           |           the following allowed cha-  |           |                      
  |                 |                           |           racters: small latin letter,|           |                     
  |                 |                           |           digits and '.'              |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
6 | WAITING_B64_DATA| Client is waiting for a   | A<-B     B64: data                    |     3     | B64: SGVsbG8=                    
  |                 | response from server.     |          where data is a base64 string|           |                      
  |                 |                           |          only 1 space is allowed      |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
7 | DISCONNECTING   | Client is waiting for     | A<-B     DISCONNECT_OK                |     1     |                      
  |                 | server to close the       |                                       |           |                      
  |                 | connection                |                                       |           |                      
---------------------------------------------------------------------------------------------------------------------------

IN CASE OF INVALID MESSAGE THE STATE SHOULD BE RESET TO 1 (INIT)

*/


#include "splpv1.h"

#include "string.h"

#include <malloc.h>

#include <stdbool.h>

bool* arr_cmd;
bool* arr_64;

void memory() {
	arr_cmd = (bool*)malloc(256 * sizeof(bool));
	arr_64 = (bool*)malloc(256 * sizeof(bool));
	for (int i = 97; i <= 122; ++i) {
		arr_cmd[i] = true;
		arr_64[i] = true;
	}
	for (int i = 65; i <= 90; ++i) {
		arr_64[i] = true;
	}
	for (int i = 48; i <= 57; ++i) {
		arr_cmd[i] = true;
		arr_64[i] = true;
	}
	arr_64[47] = true;
	arr_64[43] = true;
	arr_cmd[46] = true; 
}

enum CMD {
	GET_DATA,
	GET_FILE,
	GET_COMMAND
};

enum CMD cmd = 0;

int STATE = 1;

bool check_string(char* a, char* b) {
	int n = strlen(a);
	for (int i = 0; i < n; ++i) {
		if (a[i] != b[i]) {
			STATE = 1;
			return MESSAGE_INVALID;
		}
	}
	return MESSAGE_VALID;
}

bool check_string_B64(char* a) {
	if (!(a[0] == 'B' && a[1] == '6' && a[2] == '4' && a[3] == ':' && a[4] == ' ')) {
		STATE = 1;
		return MESSAGE_INVALID;
	}
	int n = strlen(a);
	for (int i = 5; i < n; ++i) {
		if (!arr_64[a[i]]) {
			STATE = 1;
			return MESSAGE_INVALID;
		}
	}
	return MESSAGE_VALID;
}

bool check_string_VERSION(char* a) {
	if (a[0] == 'V' && a[1] == 'E' && a[2] == 'R' && a[3] == 'S' &&
		a[4] == 'I' && a[5] == 'O' && a[6] == 'N' && a[7] == ' ' && isdigit(a[8])) {
		return MESSAGE_VALID;
	}
	STATE = 1;
	return MESSAGE_INVALID;
}

bool check_string_CMD(char* a) {
	int s = strlen(a);
	bool check = false;
	int temp;
	switch (cmd)
	{
	case GET_DATA:
		if (a[0] == 'G' && a[1] == 'E' && a[2] == 'T' && a[3] == '_' &&
			a[4] == 'D' && a[5] == 'A' && a[6] == 'T' && a[7] == 'A') {
			check = true;
			temp = 9;
		}
		break;
	case GET_FILE:
		if (a[0] == 'G' && a[1] == 'E' && a[2] == 'T' && a[3] == '_' &&
			a[4] == 'F' && a[5] == 'I' && a[6] == 'L' && a[7] == 'E') {
			check = true;
			temp = 9;
		}
		break;
	case GET_COMMAND:
		if (a[0] == 'G' && a[1] == 'E' && a[2] == 'T' && a[3] == '_' && a[4] == 'C' && a[5] == 'O' &&
			a[6] == 'M' && a[7] == 'M' && a[8] == 'A' && a[9] == 'N' && a[10] == 'D') {
			check = true;
			temp = 12;
		}
		break;
	default:
		break;
	}

	if (check) {
		for (int i = temp; i < s - temp-1; ++i) {
			if (!arr_cmd[a[i]]) {
				STATE = 1;
				return MESSAGE_INVALID;
			}
		}
	}
	else {
		STATE = 1;
		return MESSAGE_INVALID;
	}

	switch (cmd)
	{
	case GET_DATA:
		if (!(a[s - 8] == 'G' && a[s - 7] == 'E' && a[s - 6] == 'T' && a[s - 5] == '_' &&
			a[s - 4] == 'D' && a[s - 3] == 'A' && a[s - 2] == 'T' && a[s - 1] == 'A')) {
			return MESSAGE_INVALID;
		}
		break;
	case GET_FILE:
		if (!(a[s - 8] == 'G' && a[s - 7] == 'E' && a[s - 6] == 'T' && a[s - 5] == '_' &&
			a[s - 4] == 'F' && a[s - 3] == 'I' && a[s - 2] == 'L' && a[s - 1] == 'E')) {
			return MESSAGE_INVALID;
		}
		break;
	case GET_COMMAND:
		if (!(a[s - 11] == 'G' && a[s - 10] == 'E' && a[s - 9] == 'T' &&
			a[s - 8] == '_' && a[s - 7] == 'C' && a[s - 6] == 'O' &&
			a[s - 5] == 'M' && a[s - 4] == 'M' && a[s - 3] == 'A' &&
			a[s - 2] == 'N' && a[s - 1] == 'D')) {
			return MESSAGE_INVALID;
		}
		break;
	default:
		break;
	}

	return MESSAGE_VALID;
}

enum test_status validate_message( struct Message* msg )
{
	switch (STATE)
	{
	case 1://+
		if (!(*msg).direction && check_string((*msg).text_message, "CONNECT")) {
			STATE = 2;
			return MESSAGE_VALID;
		}
		break;
	case 2://+
		if ((*msg).direction && check_string((*msg).text_message, "CONNECT_OK")) {
			STATE = 3;
			return MESSAGE_VALID;
		}
		break;
	case 3://+
		if (!(*msg).direction) {
			if (check_string((*msg).text_message, "GET_VER")) {
				STATE = 4;
				return MESSAGE_VALID;
			}
			else if (check_string((*msg).text_message, "GET_DATA")) {
				STATE = 5;
				cmd = 0;
				return MESSAGE_VALID;
			}
			else if (check_string((*msg).text_message, "GET_FILE")) {
				STATE = 5;
				cmd = 1;
				return MESSAGE_VALID;
			}
			else if (check_string((*msg).text_message, "GET_COMMAND")) {
				STATE = 5;
				cmd = 2;
				return MESSAGE_VALID;
			}
			else if (check_string((*msg).text_message, "GET_B64")) {
				STATE = 6;
				return MESSAGE_VALID;
			}
			else if (check_string((*msg).text_message, "DISCONNECT")) {
				STATE = 7;
				return MESSAGE_VALID;
			}
		}
		break;
	case 4://+
		if ((*msg).direction && check_string_VERSION((*msg).text_message)) {
			STATE = 3;
			return MESSAGE_VALID;
		}
		break;
	case 5:
		if ((*msg).direction && check_string_CMD((*msg).text_message)) {
			STATE = 3;
			return MESSAGE_VALID;
		}
		break;
	case 6://+
		if ((*msg).direction && check_string_B64((*msg).text_message)) {
			STATE = 3;
			return MESSAGE_VALID;
		}
		break;
	case 7://+
		if ((*msg).direction && check_string((*msg).text_message, "DISCONNECT_OK")) {
			STATE = 1;
			return MESSAGE_VALID;
		}
		break;
	default:
		break;
	}
	STATE = 1;
    return MESSAGE_INVALID;
}
