#ifndef _SIM_LCD_H_
#define _SIM_LCD_H_


// ��񣺺�����Ϊ������λͼ
typedef struct
{
	int	vWidth;		// ���ڿ�
	int vHeight;	// ���ڸ�
	int vScale;		// λͼ�Ŵ���
	const char  *strBmpPath;	// λͼ·��
}CSlcdSpecs;


int SimLcd_Init(CSlcdSpecs *ptLcdPara);
int SimLcd_Flush(void);
void SimLcd_SetKeyFunc(FKeyDeal fnKey);
void SimLcd_SetMouseFunc(FMouseDeal fnMouse);
// ��鱾�߳��Ƿ��˳��������˳���ǿ�ƽ���Ҳ�˳�
int SimLcd_CheckExit(void);


#endif
