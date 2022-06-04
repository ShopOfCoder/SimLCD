#include "KeyLcdIn.h"
#include "SimLcd.h"


int SimKey_Init(FKeyDeal fnKeyDeal, FMouseDeal fnMouseDeal)
{
	if(fnKeyDeal != NULL){
		SimLcd_SetKeyFunc(fnKeyDeal);
	}
	if(fnMouseDeal != NULL){
		SimLcd_SetMouseFunc(fnMouseDeal);
	}	

	return 0;
}


