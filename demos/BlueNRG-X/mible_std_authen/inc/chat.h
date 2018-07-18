
#ifndef _CHAT_H_
#define _CHAT_H_

uint8_t CHAT_DeviceInit(void);
void APP_Tick(void);
void Process_InputData(uint8_t* data_buffer, uint16_t Nb_bytes);

#endif // _CHAT_H_
