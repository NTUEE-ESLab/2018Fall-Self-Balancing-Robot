
#include "IAP.H"


/***************************************************************
** ��������: STC_ISP
** ��������: STC12C5A60S2��ϵ���¼����
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
 void STC_ISP()
 {
    
	IN_OFF=1; //����2

    if(!IN_OFF){IAP_CONTR=0x60;} //�жϴ����Ƿ������ݹ���
  
 }

