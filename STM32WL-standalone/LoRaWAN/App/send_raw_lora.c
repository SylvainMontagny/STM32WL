#include "send_raw_lora.h"
#include "radio.h"
#include "radio_ex.h"
#include "sys_app.h"
#include "stm32_tiny_sscanf.h"
#include "stm32_seq.h"

volatile uint32_t RadioTxDone_flag = 0;


static testParameter_t testParam = { TEST_LORA, 868100000, EMISSION_POWER, 4, 7, CR4o5, 0, 0, DEFAULT_PAYLOAD_LEN, DEFAULT_FSK_DEVIATION, DEFAULT_LDR_OPT, DEFAULT_GAUSS_BT};
static RadioEvents_t LoRa_RadioEvents;
static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint8_t PrepareLoRaFrame(const char* param){
	uint32_t freq;
	int32_t power;
	uint32_t sf;
	uint16_t param_ptr=0; // index to get the position of each given parameter (freq:power:sf:payload)

	APP_LOG_COLOR(RED);

	if((param[0]=='L')&(param[1]=='O')&(param[2]=='R')&(param[3]=='A')&(param[4]=='=')){
		param_ptr=5; // param_ptr is now pointing on the frequency
	}
	else if((param[0]=='E')&(param[1]=='R')&(param[2]=='R')&(param[3]=='O')&(param[4]=='R')){
		APP_LOG(0,1,"\r\n\r\n -- ERROR - Payload too long (Size > 100) -- \r\n");
		return PARAM_LORA_ERROR;
	}
	else{
		APP_LOG(0,1,"\r\n -- ERROR - Wrong Format -- \r\n");
		APP_LOG(0,1,"\r\n Example: LORA=868100000:14:7:01020304 \r\n");
		return PARAM_LORA_ERROR;
	}

	/*Radio parameters configuration*/
	/* Read and get the value of frequency, power and spreading factor */
	if (3 != tiny_sscanf(&param[param_ptr], "%d:%d:%d",
							&freq,
							&power,
							&sf))
	{
		APP_LOG(0,1,"\r\n\r\n -- ERROR - Wrong Format -- \r\n");
		APP_LOG(0,1,"\r\n Example: LORA=868100000:14:7:01020304 \r\n");
		return PARAM_LORA_ERROR;
	}

	/* 1: Frequency check and set */
	if( ((freq<867100000) || (freq>868500000)) || ( (freq!=867100000) && (freq!=867300000) && (freq!=867500000) && (freq!=867700000) && (freq!=867900000) && (freq!=868100000) && (freq!=868300000) && (freq!=868500000)))
	{
		APP_LOG(0,1,"\r\n\r\n -- ERROR - Wrong Frequency -- \r\n");
		APP_LOG(0,1,"Please select a valid frequency among these:\r\n"
				   "* 867100000 \r\n"
				   "* 867300000 \r\n"
				   "* 867500000 \r\n"
				   "* 867700000 \r\n"
				   "* 867900000 \r\n"
				   "* 868100000 \r\n"
				   "* 868300000 \r\n"
				   "* 868500000 \r\n");
		return PARAM_LORA_ERROR;
	}
	else
	{
		testParam.freq=freq;
		param_ptr+=10; // param_ptr is now pointing on the power
	}

	/* 2: Power check and set */
	if ((power >= -9) && (power <= 22))
	{
		testParam.power=power;
		param_ptr+=2; // param_ptr is now pointing on the spreading factor
		if((power>=10)||(power<0)){param_ptr++;}; // One byte is added if the power is on 2 bytes
	}
	else
	{
		APP_LOG(0,1,"\r\n\r\n -- ERROR - Wrong Power -- \r\n");
		APP_LOG(0,1,"Please select a power between -9dB and 22dB\r\n");
		return PARAM_LORA_ERROR;
	}

	/* 3: Spreading factor check and set */
	if ((sf >= 5) && (sf <= 12))
	{
		testParam.loraSf_datarate=sf;
		param_ptr+=2; // param_ptr is now pointing on the payload
		if(sf>=10){param_ptr++;}; //One byte is added if the sf is on 2 bytes
	}
	else
	{
		APP_LOG(0,1,"\r\n\r\n -- ERROR - Wrong Spreading Factor -- \r\n");
		APP_LOG(0,1,"Please choose a SF between 5 and 12\r\n");
		return PARAM_LORA_ERROR;
	}

	if((SendLoRaFrame(&param[param_ptr]))==PARAM_LORA_ERROR){
		return PARAM_LORA_ERROR;
	}
	return PARAM_LORA_OK;
}

int8_t SendLoRaFrame(const char* buff_payload){
	TxConfigGeneric_t LoRa_TxConfig;
	uint8_t payload[256] = {0};
	uint32_t payloadLen = 0;
	uint8_t isHex=0;

	// I - Radio callback configuration
	LoRa_RadioEvents.TxDone = LoRa_OnTxDone;
	LoRa_RadioEvents.RxDone = LoRa_OnRxDone;
	LoRa_RadioEvents.TxTimeout = LoRa_OnTxTimeout;
	LoRa_RadioEvents.RxTimeout = LoRa_OnRxTimeout;
	LoRa_RadioEvents.RxError = LoRa_OnRxError;
	Radio.Init(&LoRa_RadioEvents);

	if(((strlen(buff_payload)%2)==0)&(strlen(buff_payload)!=0)){
		payloadLen= strlen(buff_payload)/2;
		//APP_PRINTF(" / payload=%s / size=%d \r\n", buff_payload, payloadLen);
	}
	else{
		APP_PRINTF("\r\n\r\n -- ERROR - Odd Payload Size -- \r\n");
		return -1;
	}
	if(function_hexToString(buff_payload, (char*) payload)){
		isHex=1;
	}
	else if(is_valid_base64(buff_payload)){
		isHex=0;
		payloadLen = function_base64ToString(buff_payload,(char*) payload);
		if(payloadLen==0){
			APP_PRINTF("\r\n\r\n -- ERROR - Wrong Base64 Payload Content -- \r\n");
			return -1;
		}
	}
	else{
		if(isHex){
			APP_PRINTF("\r\n\r\n -- ERROR - Wrong Hex Payload Content -- \r\n");
		}
		else{
			APP_PRINTF("\r\n\r\n -- ERROR - Wrong Base64 Payload Content -- \r\n");
		}
		return -1;
	}
	APP_LOG_COLOR(GREEN);
	APP_LOG(0,1,"\r\n> Packet transmitted \r\n");
	APP_PRINTF("> Freq=%d / Pow=%d / SF=%d / Payload=%s / Size=%d \r\n\r\n",testParam.freq,testParam.power,testParam.loraSf_datarate,buff_payload,payloadLen);
	Radio.SetChannel(testParam.freq);
	LoRa_TxConfig.lora.Bandwidth = RADIO_LORA_BW_125;
	LoRa_TxConfig.lora.SpreadingFactor = (RADIO_LoRaSpreadingFactors_t) testParam.loraSf_datarate; /*BitRate*/
	LoRa_TxConfig.lora.Coderate = CR4o5;
	LoRa_TxConfig.lora.LowDatarateOptimize = DEFAULT_LDR_OPT;
	LoRa_TxConfig.lora.PreambleLen = LORA_PREAMBLE_LENGTH;
	LoRa_TxConfig.lora.LengthMode = RADIO_LORA_PACKET_VARIABLE_LENGTH; // edit sylvain default RADIO_LORA_PACKET_VARIABLE_LENGTH, OK c'est la bonne valeur à priori
	LoRa_TxConfig.lora.CrcMode = RADIO_LORA_CRC_ON;
	LoRa_TxConfig.lora.IqInverted = RADIO_LORA_IQ_NORMAL; // edit sylvain default = RADIO_LORA_IQ_NORMAL , c'est bien NORMAL qu'il faut, sinon on inverse les upchirp en downchirp

	Radio.RadioSetTxGenericConfig(GENERIC_LORA, &LoRa_TxConfig, testParam.power, TX_TIMEOUT_VALUE);
	Radio.SetPublicNetwork(true); /*set private syncword*/ // edit sylvain default value=false
	Radio.Send(payload, payloadLen);
	UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_RadioOnTstRF); // Wait event generated by LoRaOnTxDone()
	Radio.Sleep();
	//APP_LOG(0,1,"\r\n -- End LoRa Frame Transmission -- \r\n");
	return 1;
}

int hex_to_int(char c){
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first * 10 + second;
    if (result > 9) result--;
    return result;

}

int hex_to_ascii(char c, char d)
{
	int high = hex_to_int(c) * 16;
	int low = hex_to_int(d);
	return high+low;
}

int8_t function_hexToString(const char* st, char* payload)
{
	int length = strlen(st);
    int i;
    int a=0;
    char buf = 0;
    char value_st =0;
    for(i = 0; i < length; i++){
    	value_st = st[i];
    	if (value_st >= 97){value_st = value_st - 32;}
		if(value_st<'0'||value_st>'F'||((value_st>'9')&(value_st<'A'))){
			return false;
		}
		else
		{
			if(i % 2 != 0)
			{
				payload[a]=hex_to_ascii(buf, value_st);
				a++;
			}
			else
			{
				buf = value_st;
			}
		}
    }
    return true;
}

int8_t isBase64(char c) {
    int8_t ret = (c == '=' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            c == '+' || c == '/');
    return ret;
}

// check if the buffer is in base64
int is_valid_base64(const char* base64_buffer) {
    size_t len = strlen(base64_buffer);

    // Check payload length
    if (len % 4 != 0) {
        return 0;  // Invalid length
    }

    // Check char
    for (size_t i = 0; i < len; i++) {
        if (!isBase64(base64_buffer[i])) {
            return 0;  // Invalid char
        }
    }
    return 1;  // valid buffer
}

int get_base64_value(char c) {
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    if (c >= '0' && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return -1;
}

// convert from Base 64 to String and return the length of the String
uint8_t function_base64ToString(const char* base64_buffer, char* payload) {
    size_t base64_len = strlen(base64_buffer);
    size_t string_len = ((base64_len + 3) / 4) * 3;
    size_t i = 0, j = 0;
    int nb_equal=0;

    while (i < base64_len) {
        unsigned int value[4];
        value[0] = value[1] = value[2] = value[3] = 0;

        // Read four characters from the base64 buffer
        for (int k = 0; k < 4; k++) {

            if (!isBase64(base64_buffer[i])) {
                // Invalid base64 character
                return false;
            }

            if(base64_buffer[i] != '='){
            	value[k]=strchr(base64_table, base64_buffer[i]) - base64_table;
            	if(nb_equal>0){
            		// '=' must not be inserted in the message, only at the end
            		return false;
            	}
            }
            else{
            	nb_equal++;
            }
            i++;
        }

        // Decode the four base64 characters (value[0] to value[3]) to three bytes
        unsigned int byte0, byte1, byte2;

        byte0 = (value[0] << 2) | (value[1] >> 4);
		byte1 = (value[1] << 4) | (value[2] >> 2);
		byte2 = (value[2] << 6) | value[3];


        // Append the three bytes to the string buffer
        payload[j++] = byte0;
        payload[j++] = byte1;
        payload[j++] = byte2;
    }
    string_len-=nb_equal;

    // Null-terminate the string buffer
    payload[string_len] = '\0';

    return string_len;
}

void LoRa_OnTxDone(void){
	  RadioTxDone_flag = 1;
	  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
}

void LoRa_OnRxDone(void){
}

void LoRa_OnTxTimeout(void){
}

void LoRa_OnRxTimeout(void){
}

void LoRa_OnRxError(void){
}


