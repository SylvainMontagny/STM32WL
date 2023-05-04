#include "send_raw_lora.h"
#include "radio.h"
#include "radio_ex.h"
#include "sys_app.h"

volatile uint32_t RadioTxDone_flag = 0;


static testParameter_t testParam = { TEST_LORA, 868100000, EMISSION_POWER, 4, 7, CR4o5, 0, 0, DEFAULT_PAYLOAD_LEN, DEFAULT_FSK_DEVIATION, DEFAULT_LDR_OPT, DEFAULT_GAUSS_BT};
static RadioEvents_t LoRa_RadioEvents;

uint8_t PrepareLoRaFrame(const char* param){
	uint32_t freq;
	int32_t power;
	uint32_t sf;
	uint16_t param_ptr=0; // index to get the position of each given parameter (freq:power:sf:payload)

	APP_LOG_COLOR(RED);

	if((param[0]=='L')&(param[1]=='O')&(param[2]=='R')&(param[3]=='A')&(param[4]=='=')){
		param_ptr=5; // param_ptr is now pointing on the frequency
	}
	else{
		APP_LOG(0,1,"\r\n -- ERROR - Wrong Format -- \r\n");
		APP_LOG(0,1,"\r\n Example: LORA=868100000:14:7:48454C4C4F \r\n");
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
		APP_LOG(0,1,"\r\n Example: LORA=868100000:14:7:48454C4C4F \r\n");
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
		APP_PRINTF("\r\n\r\n -- ERROR - Wrong Payload Size -- \r\n");
		return -1;
	}
	if(!function_hexToString(buff_payload, (char*) payload)){
		APP_PRINTF("\r\n\r\n -- ERROR - Wrong Payload Content -- \r\n");
		return -1;
	}
	APP_LOG_COLOR(GREEN);
	//APP_LOG(0,1,"\r\n  -- Start LoRa Frame Transmission -- \r\n");
	APP_PRINTF("\r\n> Freq=%d / Pow=%d / SF=%d / Payload=%s / Size=%d \r\n",testParam.freq,testParam.power,testParam.loraSf_datarate,buff_payload,payloadLen);
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


