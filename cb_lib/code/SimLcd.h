#ifndef _SIM_LCD_H_
#define _SIM_LCD_H_


// 规格：后续改为不依赖位图
typedef struct
{
	int	vWidth;		// 窗口宽
	int vHeight;	// 窗口高
	int vScale;		// 位图放大倍数
	const char  *strBmpPath;	// 位图路径
}CSlcdSpecs;


int SimLcd_Init(CSlcdSpecs *ptLcdPara);
int SimLcd_Flush(void);
void SimLcd_SetKeyFunc(FKeyDeal fnKey);
void SimLcd_SetMouseFunc(FMouseDeal fnMouse);
// 检查本线程是否退出，若已退出则强制进程也退出
int SimLcd_CheckExit(void);


#endif
