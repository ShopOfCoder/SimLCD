#ifndef __KEY_LCD_IN_H__
#define __KEY_LCD_IN_H__

#include <string.h>
#include <stdlib.h>
#include <io.h>


// �����¼�
#define KEY_EVENT_DOWN		0
#define KEY_EVENT_UP		1
// ����¼�
#define MOUSE_EVENT_DOWN	0
#define MOUSE_EVENT_UP		1
#define MOUSE_EVENT_MOVE   	2
// �ص�
typedef void (*FKeyDeal)(int vEvent, int vKeyVal);
typedef void (*FMouseDeal)(int vEvent, int vX, int vY);


#endif
