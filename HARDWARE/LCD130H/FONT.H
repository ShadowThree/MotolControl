#ifndef __FONT_H
#define __FONT_H 

/*******************************************************************************/

//DevEBox  ��Խ����

//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com
/*******************************************************************************/


extern const unsigned char asc2_1206[95][12]; //12*12 ASCII�ַ�������

extern const unsigned char asc2_1608[95][16]; //16*16 ASCII�ַ�������

extern const unsigned char asc2_2412[95][36]; //24*24 ASICII�ַ�������



//�����ֿ�洢����

struct typFNT_GB16
{
   unsigned char Index[2];	
   unsigned char Msk[32];
};



#define USE_ONCHIP_FLASH_FONT 1    //�����Ƿ���Ƭ���ֿ�

//����5��
#define hz16_num   56   //�˴������趨�ֿ⺺���ַ��Ĵ洢���ֵĸ�����������ӻ��߼����ַ�����Ҫ����Ҳ��Ӧ�޸�

//���ӿƼ�Һ�����Գ�����ɫ���������ʾͼƬ��������ȫ�̼���֧��רע�����ɹ�

extern const struct typFNT_GB16 hz16[];


//�����ֿ�洢����

struct typFNT_GB24
{
       unsigned char Index[2];		// ���ڴ洢һ������
       unsigned char Msk[72];			// ���ڴ洢���溺�ֵ���ģ����
};
/* ��ʽ���£�

{"��",
0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x08,0x01,0x00,0x3C,0x01,0x33,0xC0,0x3E,0xC2,0x00,0x08,0x62,0x00,0x04,0x42,0x00,
0x04,0x82,0x00,0x04,0x9A,0x0C,0x3F,0xFF,0xFE,0x01,0x02,0x30,0x01,0x02,0x30,0x01,0x12,0x30,0x1F,0xE2,0x30,0x01,0x02,0x30,
0x0D,0x42,0x30,0x19,0x36,0x30,0x11,0x14,0x30,0x21,0x14,0x30,0x21,0x08,0x30,0x47,0x10,0x30,0x02,0x20,0x30,0x00,0x00,0x00    
} */



#define hz24_num   9   //�˴������趨�ֿ⺺���ַ��Ĵ洢���ֵĸ�����������ӻ��߼����ַ�����Ҫ����Ҳ��Ӧ�޸�

extern const struct typFNT_GB24 hz24[];




#endif




/*******************************************************************************/

//DevEBox  ��Խ����

//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com
/*******************************************************************************/



































