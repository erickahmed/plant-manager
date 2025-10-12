#ifndef TWI_H
#define TWI_H

extern volatile uint16_t *pLastMoistureVal;

void twiInit(void);
void twiListen(void);
void twiRespond(void);

#endif
