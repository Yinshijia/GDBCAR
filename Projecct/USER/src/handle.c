#include "handle.h"

uint8 colour[2] = {255, 0},black=1,white=0; //0 �� 1 �ֱ��Ӧ����ɫ
//ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
uchar Weight[60] = { 5,  6,  8,  9, 11, 12, 14, 15, 17, 18,	// 50
					20, 21, 22, 23, 24, 25, 26, 27, 28, 29,	// 40
				    27, 27, 26, 25, 24, 23, 22, 21, 20, 19,	// 30
				    4,  3,  3,  2,  1,    1,  1,  1,  1,  1,	// 20
					1,  1,  1,  1,  1,    1,  1,  1,  1,  1,
					1,  1,  1,  1,  1,    1,  1,  1,  1,  1};	//��Ȩƽ������

uchar Left_Line[62], Right_Line[62], Mid_Line[62];	// ԭʼ���ұ߽�����
uchar Left_Add_Line[62], Right_Add_Line[62];		// ���ұ߽粹������
uchar Left_Add_Flag[62], Right_Add_Flag[62];		// ���ұ߽粹�߱�־λ
uchar Jump[62];
uchar Width_Real[62];	// ʵ����������
uchar Width_Add[62];	// ������������
uchar Width_Min;		// ��С��������

uchar Foresight;	// ǰհ�������ٶȿ���
uchar Out_Side = 0;	// ���߿���
uchar Line_Count;	// ��¼�ɹ�ʶ�𵽵���������

uchar Left_Add_Start, Right_Add_Start;	// ���Ҳ�����ʼ������
uchar Left_Add_Stop, Right_Add_Stop;	// ���Ҳ��߽���������
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 0, Right_Kb = 0;	// ��С���˷�����

uchar Left_Hazard_Flag, Right_Hazard_Flag;	// �����ϰ����־
uchar Left_Max, Right_Min;
int32 Area_Left = 0, Area_Right = 0;	// ���Ҳ������

uchar Starting_Line_Flag = 0;	// �����߱�־λ

/************** ��·��ر��� *************/
uchar Annulus_Count[10] = {1, 1, 1, 1, 0, 0, 1, 0, 1, 0};	// 1��·��0�һ�·
uchar Annulus_Times = 0;	// ��·����
uchar Annulus_Flag = 0;
uchar Annulus_Left = 0;
uchar Annulus_Right = 0;
int16 Annulus_Delay = 0;
uchar Annulus_Mode = 0;		// ׼������·״̬
/************** ��·��ر��� *************/

void Annulus_Control(void)
{
	Annulus_Left = Annulus_Count[Annulus_Times++];
	Annulus_Right = !Annulus_Left;
	
	Annulus_Mode = 1;	// ��ʼ���뻷·
	Annulus_Flag = 1;	// ��·��־λ��λ
	
	if (Annulus_Times >= 8)
	{
		Annulus_Times = 0;
	}
}

/****************** ���㷨 ******************/

/*
*	ͼ���㷨������ʼ��
*
*	˵������Ӱ���һ�����⴦��
*/
void Image_Para_Init(void)
{	
	Mid_Line[61] = 80;
	Left_Line[61] = 1;
	Right_Line[61] = 159;
	Left_Add_Line[61] = 1;
	Right_Add_Line[61] = 159;
	Width_Real[61] = 158;
	Width_Add[61] = 158;
	
	Annulus_Left = 1;	// ��λ��·
	Annulus_Right = 0; 
}



/*
*	ͼ�����㷨
*
*	˵����������ͨͼ�񣬰���ʮ�֡��ϰ�
*/
void Image_Handle(uchar *data)
{
	uchar i;	// ������
	uchar res;	// ���ڽ��״̬�ж�
	float Result;	// ���ڽ��״̬�ж�
	uchar Add_Start_Max;
	uchar Width_Check;
	uchar Weight_Left, Weight_Right;
	uchar Mid_Left, Mid_Right;
	uchar Limit_Left, Limit_Right;
	static uchar Annulus_Count = 0;	// ��·����������ֹ����
	
	Line_Count = 0;	// ����������λ
	Starting_Line_Flag = 0;	// �����߱�־λ��λ
	
	Annulus_Flag = 0;		// ��λ��·��־λ
	
	Left_Hazard_Flag = 0;	// ��λ�����ϰ����־λ
	Right_Hazard_Flag = 0;
	
	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;
	
	/***************************** ��һ�����⴦�� *****************************/
	
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// ����
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	
	/*************************** ��һ�����⴦������ ***************************/
	
	for (i = 59; i >= 15;)	// ������ǰ40��ͼ�񣬸��к������20������
	{
		i -= 2;	// ���д�������С��Ƭ������
		
		if (Left_Add_Flag[i+2])
		{
			Limit_Left = Left_Line[i+2];
		}
		else
		{
			if (Jump[i] >= 2)
			{
				Limit_Left = Left_Add_Line[i+2];
			}
			else
			{
				Limit_Left = Left_Add_Line[i+2]+1;
			}
			
		}
		
		if (Right_Add_Flag[i+2])
		{
			Limit_Right = Right_Line[i+2];
		}
		else
		{
			if (Jump[i] >= 2)
			{
				Limit_Right = Right_Add_Line[i+2];
			}
			else
			{
				Limit_Right = Right_Add_Line[i+2]-1;
			}
		}
		Jump[i] = Corrode_Filter(i, data, Limit_Left, Limit_Right);	// ʹ�ø�ʴ�˲��㷨�ȶԱ�����������Ԥ�������������������
		if (Jump[i] >= 5 && i>= 21)
		{
			Starting_Line_Flag = 1;
		}
			
		if (!data[i*160 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬����������������Ҳ�����ǻ�·
		{
			if (Left_Add_Start && !Left_Add_Stop && Right_Add_Start && !Right_Add_Stop)	//  ���߶��в��ߣ���û�в��߽������������˻�·
			{ 
				if (Left_Add_Start >= Right_Add_Start)
				{
					Add_Start_Max = Left_Add_Start;
				}
				else
				{
					Add_Start_Max = Right_Add_Start;
				}
				if (Width_Real[i+2] > Width_Real[Add_Start_Max] && res == 1)
				{
					if (Left_Ka <= 0 && Right_Ka >= 0)
					{
						if (Left_Add_Start >= 21 && Left_Add_Start <= 55 && Right_Add_Start >= 21 && Right_Add_Start <= 55)
						{
							if (Left_Add_Start >= i+4 && Right_Add_Start >= i+4)
							{
								if (Left_Ka <= -1.2|| Right_Ka >= 1.2)
								{	
									break;									
								}
								else
								{
									Annulus_Control();	// ������·ר���㷨
									break;
								}
							}
						}
					}
				}
			}
			break;
		}
		else	// ʹ��ǰ2���е�������ɨ��߽�
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		}
			
		/**************************** ���߼�⿪ʼ ****************************/
		if (Starting_Line_Flag)
		{
			Width_Check = 7;
		}
		else
		{
			Width_Check = 2;
		}
		if (Width_Real[i] > Width_Min+Width_Check)	// �������ȱ����������ʮ�ֻ�·
		{
			if (Left_Add_Line[i] <  Left_Add_Line[i+2])
			{
				if (!Left_Add_Flag[i])
				{
					Left_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
				}
			}
			if (Right_Add_Line[i] > Right_Add_Line[i+2])
			{
				if (!Right_Add_Flag[i])
				{
					Right_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
				}
			}
			
			if (Left_Add_Flag[i] || Right_Add_Flag[i])
			{
				if (Left_Add_Stop && Right_Add_Stop)
				{
					break;
				}
			}
		}
		
		/**************************** ���߼����� ****************************/
		
		
		/**************************** �ϰ���⿪ʼ ****************************/
		
		else
		{
			if ((i != 59) && (Width_Real[i]+12 < Width_Real[i+2]) && (Jump[i] == 2))
			{
				if (Left_Add_Line[i] > Left_Add_Line[i+2] + 6 && !Left_Add_Start)	// ������ǰһ����߽��нϴ�ͻ����û�в���
				{
					Left_Hazard_Flag = i;	// �ϰ����־λ��λ
				}
				if (Right_Add_Line[i] < Right_Add_Line[i+2] - 6 && !Right_Add_Start)	// ������ǰһ���ұ߽��нϴ�ͻ����û�в���
				{
					Right_Hazard_Flag = i;	// �ϰ����־λ��λ
				}
				
				
			}
		}
		
		/**************************** �ϰ������� ****************************/
		
		
		/*************************** ��һ�ֲ��߿�ʼ ***************************/
		
		if (Left_Add_Flag[i])	// �����Ҫ����
		{
			if (i >= 53)	// ǰ���в��߲���
			{
				if (!Left_Add_Start)
				{
					Left_Add_Start = i;	// ��¼���߿�ʼ��
					Left_Ka = 0;
					Left_Kb = Left_Add_Line[i+2];
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// ʹ��ǰһ֡ͼ����߽�б�ʲ���
			}
			else
			{
				if (!Left_Add_Start)	// ֮ǰû�в���
				{
					Left_Add_Start = i;	// ��¼��ಹ�߿�ʼ��
					if (Left_Hazard_Flag)	// ������ϰ���
					{
						Left_Ka = 0;
						Left_Kb = Left_Add_Line[i+2];	// ��ֱ���ϲ���
					}
					else	// б�ʲ���
					{
						Curve_Fitting(&Left_Ka, &Left_Kb, &Left_Add_Start, Left_Add_Line, Left_Add_Flag, 1);	// ʹ�����㷨���ֱ��
					}
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// �������
			}
		}
		else
		{
			if (Left_Add_Start)	// �Ѿ���ʼ����
			{
				if (!Left_Add_Stop && !Left_Add_Flag[i+2])
				{
					if (Left_Add_Line[i] >= Left_Add_Line[i+2])
					{
						Left_Add_Stop = i;	// ��¼��ಹ�߽�����
					}
				}
			}
		}
		
		if (Right_Add_Flag[i])	// �Ҳ���Ҫ����
		{
			if (i >= 53)	// ǰ���в��߲���
			{
				if (!Right_Add_Start)
				{
					Right_Add_Start = i;	// ��¼���߿�ʼ��
					Right_Ka = 0;
					Right_Kb = Right_Add_Line[i+2];
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// ʹ��ǰһ֡ͼ���ұ߽�б�ʲ���
			}
			else
			{
				if (!Right_Add_Start)	// ֮ǰû�в���
				{
					Right_Add_Start = i;	// ��¼�Ҳಹ�߿�ʼ��
					if (Right_Hazard_Flag)	// �Ҳ����ϰ���
					{
						Right_Ka = 0;
						Right_Kb = Right_Add_Line[i+2];	// ��ֱ���ϲ���
					}
					else
					{
						Curve_Fitting(&Right_Ka, &Right_Kb, &Right_Add_Start, Right_Add_Line, Right_Add_Flag, 2);	// ʹ�����㷨���ֱ��
					}
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// �������
			}
		}
		else
		{
			if (Right_Add_Start)	// �Ѿ���ʼ����
			{
				if (!Right_Add_Stop && !Right_Add_Flag[i+2])
				{
					if (Right_Line[i] <= Right_Line[i+2])
					{
						Right_Add_Stop = i;	// ��¼�Ҳಹ�߽�����
					}
				}
			}
		}
		
		/*************************** ��һ�ֲ��߽��� ***************************/
		
//		if ((Left_Add_Flag[i] && Right_Add_Flag[i]) || (!Left_Add_Flag[i] && !Right_Add_Flag[i]))
//		{
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���¼�����������
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ��������
//		}
//		else
//		{
//			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���¼�����������
//			Mid_Line[i] = Mid_Line[i+2];
//		}

		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// ������С��������
		}
		if (Width_Add[i] <= 20)	// ��������̫С
		{
			break;
		}
		Line_Count = i;	// ��¼�ɹ�ʶ�𵽵���������
	}
	/*************************** �ڶ��ֲ����޸���ʼ ***************************/
	if (!Annulus_Flag)	// ���ǻ�·
	{
		if (Left_Add_Start)		// ��߽���Ҫ����
		{
			Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag, 1);
		}
		if (Right_Add_Start)	// �ұ߽���Ҫ����
		{
			Line_Repair(Right_Add_Start, Right_Add_Stop, data, Right_Line, Right_Add_Line, Right_Add_Flag, 2);
		}
	}
	
	/*************************** �ڶ��ֲ����޸����� ***************************/
	
	
	/****************************** �����޸���ʼ ******************************/
	
	Mid_Line_Repair(Line_Count, data);
	
	/****************************** �����޸����� ******************************/
}


uchar Fres = 0;	// ǰհ
void Fresight_Calculate(void)
{
//	if (Speed_Min < 140)
//	{
//		Fres = 0;
//	}
//	else if (Speed_Min < 150)
//	{
//		Fres = 1;
//	}
//	else if (Speed_Min < 160)
//	{
//		Fres = 2;
//	}
//	else if (Speed_Min < 170)
//	{
//		Fres = 3;
//	}
//	else if (Speed_Min >= 170)
//	{
//		Fres = 4;
//	}
  
  Fres = 0;
  
}


uchar Fresight[5][2] = {{21, 35},	// 130	ǰհ����
						{21, 35},	// 140
						{21, 33},	// 150
						{21, 31},	// 160
						{19, 29}};	// 170
/*
*	����ƽ��
*
*	˵�����������⴦����ֱ��ȡ��ֵ
*/
int32 Point_Average(void)
{
	uchar i, Point;
	int32 Sum = 0;
	static uchar Last_Point = 80;
	
	Fresight_Calculate();	// ����ǰհ
	
	if (Line_Count <= Fresight[Fres][0])
	{
		Line_Count = Fresight[Fres][0];
	}
	
	if (Annulus_Flag)
	{
		if (Line_Count <= Fresight[Fres][1])
		{
			Line_Count = Fresight[Fres][1];
		}
	}
	
	if (Out_Side || Line_Count >= 57)	// �����������ͷͼ���쳣
	{
		if (Last_Point <= 80)
		{
			Point  = 1;			// ʹ���ϴ�Ŀ���
		}
		else
		{
			Point = 159;
		}
	}
	else
	{
		
		for (i = 61; i >= Line_Count+2;)
		{
			i -= 2;
			Sum += Mid_Line[i];
		}

		Point = Sum / ((61-Line_Count)/2);	// ��������ƽ��
		
		Point = Point*0.8 + Last_Point*0.2;	// ��ͨ�˲�
		Point = range_protect(Point, 1, 159);		// �޷�����ֹ�������
		
		/*** �ϰ�������������� ***/
		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Left_Hazard_Flag];	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Left_Hazard_Flag < 40)
//			{
//				Point += 5;
//			}
		}
		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Right_Hazard_Flag];//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Right_Hazard_Flag < 40)
//			{
//				Point -= 5;
//			}
		}
		Last_Point = Point;	// �����ϴ�Ŀ���
	}
	
	return Point;
}


/*
*	����㷨
*
*	˵���������������
*/
int32 Area_Calculate(void)
{
	char i;
	uchar Line_Start = 61;
	uint16 Area_Mid = 79;
	int32 Result;
	static int32 Result_Last = 0;
	
	Area_Left = 0;	// ����������
	Area_Right = 0;	// �Ҳ��������
	
	Fresight_Calculate();	// ����ǰհ

	if (Line_Count <= Fresight[Fres][0])
	{
		Line_Count = Fresight[Fres][0];
	}
	
	if (Annulus_Flag)
	{
		if (Line_Count <= Fresight[Fres][1])
		{
			Line_Count = Fresight[Fres][1];
		}
	}
	
	for (i = Line_Start; i >= Line_Count+2;)
	{
		i -= 2;
		
		if (Mid_Line[i] == Area_Mid)
		{
			Area_Left += (Area_Mid - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - Area_Mid);
		}
		else if (Right_Add_Line[i] <= Area_Mid)	// �����������
		{
			Area_Left += Width_Add[i];
		}
		else if (Left_Add_Line[i] >= Area_Mid)	// ���Ҳ�������
		{
			Area_Right += Width_Add[i];
		}
		else
		{
			Area_Left += (Area_Mid - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - Area_Mid);
		}
	}
	
	Result = 50*(Area_Right - Area_Left)/(Area_Right + Area_Left);
	Result = range_protect(Result, -40, 40);
	
	Result = Result * 0.8 + Result_Last * 0.2;
	Result_Last = Result;
	
	return Result;
}

/*
*	��С���˷�����б��
*
*	˵��������б����Ϊת�������,����Ŵ�100��
*/
int32 Least_Squares(void)
{
	int32 Ave_x, Ave_y, Sum1, Sum2;
	uchar i;
	float f_Slope;
	int32 i_Slope;
	static int32 i_Last_Slope = 0;
	
	for (i = 61; i >= Line_Count; )
	{
		i -= 2;
	}
	i_Last_Slope = i_Slope;
	
	return i_Slope;
}

/*
*	б�ʼ���
*
*	˵����ʹ��б����Ϊת�������������б�ʵĵ���
*/
float Slope_Weight(uchar *Mid)
{
	float Slope;
	
	Slope = 1.0 * (Mid[Line_Count] - 80) / (60-Line_Count);
	
	return Slope;
}

/*
*	�����Ƕȼ���
*
*	˵�������ؽ��Ϊ �ң�0��180�㣬1��135�㣬2��90�㣬>= 1����Ϊֱ��ͻ��
*					 ��0��180�㣬-1��135�㣬-2��90�㣬<= -1����Ϊֱ��ͻ��
*/
float Calculate_Angle(uchar Point_1, uchar Point_2, uchar Point_3)
{
	char K1, K2;
	float Result;
	
	K1 = Point_2 - Point_1;
	K2 = Point_3 - Point_2;
	
	Result = (K2 - K1) * 0.5;
	
	return Result;
}

/*
*	���㷨��ֱ��
*
*	˵�������ֱ�� y = Ka * x + Kb   Mode == 1������߽磬Mode == 2�����ұ߽�
*/
void Curve_Fitting(float *Ka, float *Kb, uchar *Start, uchar *Line, uchar *Add_Flag, uchar Mode)
{
	*Start += 4;
	if (Add_Flag[*Start] == 2)
	{
		if (*Start <= 51)
		{
			*Start += 2;
		}
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
		if (Mode == 2)
		{
			if (*Ka < 0)
			{
				*Ka = 0;
			}
		}
		if (Mode == 1)
		{
			if (*Ka > 0)
			{
				*Ka = 0;
			}
		}
	}
	else
	{
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
	}
	*Kb = 1.0*Line[*Start] - (*Ka * (*Start));
}

/*
*	���㲹������
*
*	˵����ʹ�����㷨������ϳ��Ĳ�������
*/
uchar Calculate_Add(uchar i, float Ka, float Kb)	// ���㲹������
{
	float res;
	int32 Result;
	
	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 159);
	
	return (uchar)Result;
}

uchar Test_Jump;
/*
*	��ʴ�˲�
*
*	˵�����������˳�����ͳ�ƺڰ���������������������߼��
*/
uchar Corrode_Filter(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
	uchar j;
	uchar White_Flag = 0;
	uchar Jump_Count = 0;	// ��������
	
	Test_Jump = 0;
	
	Right_Max = range_protect(Right_Max, 1, 159);	// �����Ҳಿ�����򣬷�ֹ���
	
	for (j = Left_Min; j <= Right_Max; j++)	// ��������ɨ�裬����Ӱ����
	{
		if (!White_Flag)	// �Ȳ��Ұ׵㣬ֻ�˺ڵ㣬���˰׵�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				White_Flag = 1;	// ��ʼ�Һڵ�
			}
		}
		else
		{
			if (!data[i*160 + j])	// ��⵽�ڵ�
			{
				Jump_Count++;	// ��Ϊһ������
				
				Test_Jump = Jump_Count;
				
				if (!data[i*160 + j+1] && j+1 <= Right_Max)	// ���������ڵ�
				{
					if (!data[i*160 + j+2] && j+2 <= Right_Max)	// ���������ڵ�
					{
						if (!data[i*160 + j+3] && j+3 <= Right_Max)	// �����ĸ��ڵ�
						{
							if (!data[i*160 + j+4] && j+4 <= Right_Max)	// ��������ڵ�
							{
								if (!data[i*160 + j+5] && j+5 <= Right_Max)	// ���������ڵ�
								{
									if (!data[i*160 + j+6] && j+6 <= Right_Max)	// �����߸��ڵ�
									{
										if (!data[i*160 + j+7] && j+7 <= Right_Max)	// �����˸��ڵ�
										{
											if (!data[i*160 + j+8] && j+8 <= Right_Max)	// �����Ÿ��ڵ�
											{
												if (!data[i*160 + j+9] && j+9 <= Right_Max)	// ����ʮ���ڵ�
												{
													if (!data[i*160 + j+10] && j+10 <= Right_Max)	// ����11���ڵ�
													{
														White_Flag = 0;	// ��Ϊ���Ǹ��ţ������κδ������´������׵�
														j += 10;
													}
													else if (j+10 <= Right_Max)
													{
														data[i*160 + j] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+1] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+2] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+3] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+4] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+5] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+6] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+7] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+8] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+9] = 255;	// ��������10���ڵ㣬�˳���
														
														j += 10;
													}
													else
													{
														j += 10;
													}
												}
												else if (j+9 <= Right_Max)
												{
													data[i*160 + j] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+1] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+2] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+3] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+4] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+5] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+6] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+7] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+8] = 255;	// ���������Ÿ��ڵ㣬�˳���
													
													j += 9;
												}
												else
												{
													j += 9;
												}
											}
											else if (j+8 <= Right_Max)
											{
												data[i*160 + j] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+1] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+2] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+3] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+4] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+5] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+6] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+7] = 255;	// ���������˸��ڵ㣬�˳���
												
												j += 8;
											}	
											else
											{
												j += 8;
											}
										}
										else if (j+7 <= Right_Max)
										{
											data[i*160 + j] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+1] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+2] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+3] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+4] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+5] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+6] = 255;	// ���������߸��ڵ㣬�˳���										
											
											j += 7;
										}	
										else
										{
											j += 7;
										}
									}
									else if (j+6 <= Right_Max)
									{
										data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+2] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+3] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+4] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+5] = 255;	// �������������ڵ㣬�˳���
										
										j += 6;
									}	
									else
									{
										j += 6;
									}
								}
								else if (j+5 <= Right_Max)
								{
									data[i*160 + j] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+1] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+2] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+3] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+4] = 255;	// ������������ڵ㣬�˳���									
									
									j += 5;
								}	
								else
								{
									j += 5;
								}
							}
							else if (j+4 <= Right_Max)
							{
								data[i*160 + j] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+1] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+2] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+3] = 255;	// ���������ĸ��ڵ㣬�˳���
								
								j += 4;
							}	
							else
							{
								j += 4;
							}
						}
						else if (j+3 <= Right_Max)
						{
							data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
							data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
							data[i*160+j+2] = 255;	// �������������ڵ㣬�˳���
							
							j += 3;
						}	
						else
						{
							j += 3;
						}
					}
					else if (j+2 <= Right_Max)
					{
						data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
						data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
						
						j += 2;
					}	
					else
					{
						j += 2;
					}
				}
				else if (j+1 <= Right_Max)
				{
					data[i*160 + j] = 255;	// ��һ���ڵ㣬�˳���
					
					j += 1;
				}	
				else
				{
					j += 1;
				}
			}
		}
	}
//	if (White_Flag)
//	{
//		Jump_Count++;	// ��Ϊһ������
//	}
	
	return Jump_Count;	// ������������
}

/*
*	��ͷ����
*
*	˵������ĳһ�㿪ʼ��ֱ����������������Զ������
*/
uchar Limit_Scan(uchar i, uchar *data, uchar Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[160*i + Point])	// �������ڵ�
		{
			break;
		}
	}
	
	return i;	// ������Զ������
}

/*
*	��һ�����⴦��
*
*	˵������ʹ�õ�60���е���Ϊ��59��(��һ��)������ʼλ�ã��ɹ����������ұ߽��
 	����59���е㸳ֵ����60�б�����һ֡ͼ��ʹ�á������60���е��ڱ���Ϊ�ڵ㣬��
 	�ֱ�ʹ����������ұ����ķ��������߽磬���������Ƚϴ�Ľ����Ϊ��59�б߽磬
 	����Ȼ���������߽�������쳣��Ϊ���磬���緵��0
*/
uchar First_Line_Handle(uchar *data)
{
	uchar i;	// ������
	uchar Weight_Left, Weight_Right;	// ������������
	uchar Mid_Left, Mid_Right;
	uchar res;
	
	i = 59;
	
	res = Corrode_Filter(i, data, 1, 159);	// ʹ�ø�ʴ�˲��㷨�ȶԱ�����������Ԥ�������������������
	Jump[59] = res;
	
	if (!data[i*160 + Mid_Line[61]])	// ��61���е��ڵ�59��Ϊ�ڵ�
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 159);	// ����������߽�
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 159);// ���Ҳ������߽�
		if (Weight_Left >= Weight_Right && Weight_Left)	// ���������ȴ��������������Ҳ�Ϊ0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// ʹ���������ȡ�����߽�
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// ʹ���ұ�����ȡ�����߽�
		}
		else	// ˵��û�鵽
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// ��ǰһ���е�������ɨ��
	}
	
	Left_Line[61] = Left_Line[59];
	Right_Line[61] = Right_Line[59];
	Left_Add_Line[61] = Left_Add_Line[59];
	Right_Add_Line[61] = Right_Add_Line[59];
	if (Left_Add_Flag[59] && Right_Add_Flag[59])
	{
		Mid_Line[59] = Mid_Line[61];
	}
	else
	{
		Mid_Line[59] = (Right_Line[59] + Left_Line[59]) / 2;
		Mid_Line[61] = Mid_Line[59];	// ���µ�60�������е㣬������һ֡ͼ��ʹ��
	}
	if (Left_Add_Flag[59])
	{
		Left_Add_Start = i;	// ��¼���߿�ʼ��
		Left_Ka = 0;
		Left_Kb = Left_Add_Line[59];
	}
	if (Right_Add_Flag[i])
	{
		Right_Add_Start = i;	// ��¼���߿�ʼ��
		Right_Ka = 0;
		Right_Kb = Right_Add_Line[59];
	}
	
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];
	
	return 1;
}

/*
*	����࿪ʼ�����߽磬������������
*
*	˵����������������Ϊ̽������ʹ�ã��������������ȣ�������߽�����
*/
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Left_Line = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;	// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])//���ڵ�
			{
				Right_Line = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
		
		return (Right_Line - Left_Line);
	}
}

/*
*	���Ҳ࿪ʼ�����߽磬������������
*
*	˵����������������Ϊ̽������ʹ�ã��������������ȣ�������߽�����
*/
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Right_Line = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;	// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Left_Line = j+1;	//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
			
		return (Right_Line - Left_Line);
	}
}

/*
*	����࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 1; j < 160; j++)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Left_Line[i] = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;		// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Right_Line[i] = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// �����������
}

/*
*	���Ҳ࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 159; j > 0; j--)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;		// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Left_Line[i] = j+1;//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// �����������
}

/*
*	���м������������߽�
*
*	˵����������ʹ�ú󽫱���߽�����
*/
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max, uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line)
{
	uchar j;
	
	Left_Add_Flag[i] = 1;	// ��ʼ�����߱�־λ
	Right_Add_Flag[i] = 1;
	
	Left_Min = range_protect(Left_Min, 1, 159);	// �޷�����ֹ����
	if (Left_Add_Flag[i+2])
	{
		Left_Min = range_protect(Left_Min, Left_Add_Line[i+2]-30, 159);
	}
	Right_Max = range_protect(Right_Max, 1, 159);
	if (Right_Add_Flag[i+2])
	{
		Right_Max = range_protect(Right_Max, 1, Right_Add_Line[i+2]+30);
	}
	
	Right_Line[i] = Right_Max;
	Left_Line[i] = Left_Min;	// �����߽��ʼֵ
	
	for (j = Mid; j >= Left_Min; j--)	// ��ǰһ���е�Ϊ���������ұ߽�
	{
		if (!data[i*160 + j])	// ��⵽�ڵ�
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = j+1;	//��¼��ǰjֵΪ����ʵ����߽�
			Left_Add_Line[i] = j+1;	// ��¼ʵ����߽�Ϊ������߽�
			
			break;
		}
	}
	for (j = Mid; j <= Right_Max; j++)	// ��ǰһ���е�Ϊ������Ҳ����ұ߽�
	{
		if (!data[i*160 + j])	//��⵽�ڵ�
		{
			Right_Add_Flag[i] = 0;		//�ұ߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = j-1;	//��¼��ǰjֵΪ�����ұ߽�
			Right_Add_Line[i] = j-1;	// ��¼ʵ���ұ߽�Ϊ������߽�
			
			break;
		}
	}
	if (Left_Add_Flag[i+2])	// ��߽�ǰһ����Ҫ����
	{
		if (Left_Add_Line[i] <= Left_Add_Line[i+2])	// �����޶���Ҫ�ϸ�
		{
			Left_Add_Flag[i] = 1;
		}
	}
	if (Right_Add_Flag[i+2])// �ұ߽�ǰһ����Ҫ����
	{
		if (Right_Add_Line[i] >= Right_Add_Line[i+2])	// �����޶���Ҫ�ϸ�
		{
			Right_Add_Flag[i] = 1;
		}
	}
	if (Left_Add_Flag[i])	// ��߽���Ҫ����
	{
		if (!data[(i-2)*160 + Left_Add_Line[i+2]] || !data[(i-4)*160 + Left_Add_Line[i+2]])	// �����Ƿ������
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = Left_Add_Line[i+2];		//��¼��ǰjֵΪ����ʵ����߽�
			Left_Add_Line[i] = Left_Add_Line[i+2];	// ��¼ʵ����߽�Ϊ������߽�
		}
		else
		{
			if (i >= 55)	// ǰ6��
			{
				Left_Add_Line[i] = Left_Line[59];	// ʹ�õ�������
			}
			else
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];	// ʹ��ǰ2����߽���Ϊ������߽�
			}
		}
	}
	if (Right_Add_Flag[i])	// �ұ߽���Ҫ����
	{
		if (!data[(i-2)*160 + Right_Add_Line[i+2]] || !data[(i-4)*160 + Right_Add_Line[i+2]])	// �����Ƿ������
		{
			Right_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = Right_Add_Line[i+2];		//��¼��ǰjֵΪ����ʵ����߽�
			Right_Add_Line[i] = Right_Add_Line[i+2];	// ��¼ʵ����߽�Ϊ������߽�
		}
		else
		{
			if (i >= 55)	// ǰ6��
			{
				Right_Add_Line[i] = Right_Line[59];	// ʹ�õ�������
			}
			else
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];	// ʹ��ǰ2���ұ߽���Ϊ�����ұ߽�
			}
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// ����ʵ����������
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���㲹����������
	
//	data[80 * i + Right_Line[i] + 2] = 0;//����2λ��ʾ��߽磬������Թ۲�
//	data[80 * i + Left_Line[i] - 2] = 0;//����2λ��ʾ�ұ߽磬������Թ۲�
}

/*
*	�����޸�
*
*	˵������ʼ���ղ�ʹ�ã�ֱ��ʹ������б�ʽ��в���
*/
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag, uchar Mode)
{
	float res;
	uchar i, End;	// ������
	uchar Hazard_Width;
	float Ka, Kb;
	
	if ((Mode == 1) && (Right_Add_Start <= Stop) && Stop && Start <= 53 && !Left_Hazard_Flag)	// ��߽粹��
	{
		for (i = Start+2; i >= Stop+2;)
		{
			i -= 2;
			Line_Add[i] = range_protect(Right_Add_Line[i] - Width_Add[i+2]+5, 1, Right_Add_Line[i]); 
			Width_Add[i] = Width_Add[i+2]-3;
			
			if (Width_Add[i] <= 20)
			{
				Line_Count = i;
				break;
			}
		}
	}
	if ((Mode == 2) && (Left_Add_Start <= Stop) && Stop && Start <= 53 && !Right_Hazard_Flag)	// �ұ߽粹��
	{
		for (i = Start+2; i >= Stop+2;)
		{
			i -= 2;
			Line_Add[i] = range_protect(Left_Add_Line[i] + Width_Add[i+2]-5, Left_Add_Line[i], 159); 
			Width_Add[i] = Width_Add[i+2]-3;
			
			if (Width_Add[i] <= 20)
			{
				Line_Count = i;
				break;
			}
		}
	}
	else
	{
		if (Stop)	// ��ʼ����
		{
			if (((Mode == 1 && Left_Add_Start >= 55) || (Mode == 2 && Right_Add_Start >= 55))
				&& ((Mode == 1 && Line_Add[Left_Add_Stop] > Line_Add[59]) 
				|| (Mode == 2 && Line_Add[Right_Add_Stop] < Line_Add[59])))// ֻ�н�������Ҫ����
			{
				if(Stop >= Line_Count + 4)
				{
					End = Stop - 4;
				}
				else if(Stop >= Line_Count + 2)
				{
					End = Stop - 2;
				}
				else
				{
					End = 0;
				}
				if (End)
				{
					Ka = 1.0*(Line_Add[Stop] - Line_Add[End]) / (Stop - End);
					Kb = 1.0*Line_Add[Stop] - (Ka * Stop);
					
					if (Mode == 1)
					{
						if (Line_Add[59] > 59 * Ka + Kb)
						{
							Ka = 1.0*(Line_Add[Start] - Line_Add[Stop-2]) / (Start - (Stop-2));
							Kb = 1.0*Line_Add[Start] - (Ka * Start);
						}
					}
					else if (Mode == 2)
					{
						if (Line_Add[59] < 59 * Ka + Kb)
						{
							Ka = 1.0*(Line_Add[Start] - Line_Add[Stop-2]) / (Start - (Stop-2));
							Kb = 1.0*Line_Add[Start] - (Ka * Start);
						}
					}
				}
				else
				{
					Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
					Kb = 1.0*Line_Add[Start] - (Ka * Start);
				}
				
				for (i = 61; i > Stop; )
				{
					i -= 2;
					res = i * Ka + Kb;
					Line_Add[i] = range_protect((int32)res, 1, 159);
				}
			}
			else	// ����ʼ�кͽ����м���б�ʲ���
			{
				if (Start <= 57)
				{
					Start +=2;
				}
				if (Stop >= Line_Count + 4)
				{
					Stop -= 4;
				}
				else if (Stop >= Line_Count + 2)
				{
					Stop -= 2;
				}
				Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
				Kb = 1.0*Line_Add[Start] - (Ka * Start);
				
				if (Mode == 1)
				{
					if (Line_Add[59] > 59 * Ka + Kb)
					{
						Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
						Kb = 1.0*Line_Add[Start] - (Ka * Start);
					}
				}
				else if (Mode == 2)
				{
					if (Line_Add[59] < 59 * Ka + Kb)
					{
						Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
						Kb = 1.0*Line_Add[Start] - (Ka * Start);
					}
				}
				
				for (i = Stop; i <= Start; )
				{
					i += 2;
					res = i * Ka + Kb;
					Line_Add[i] = range_protect((int32)res, 1, 159);
				}
			}
		}
	}
	
	if (Mode == 1 && Left_Hazard_Flag)
	{
		for (i = Left_Hazard_Flag; i < 59; )
		{
			i += 2;
			Line_Add[i] = range_protect((int32)Line_Add[i-2]-5, Line_Add[i], 159);
		}
	}
	else if (Mode == 2 && Right_Hazard_Flag)
	{
		for (i = Right_Hazard_Flag; i < 59; )
		{
			i += 2;
			Line_Add[i] = range_protect((int32)Line_Add[i-2]+5, 1, Line_Add[i]);
		}
	}
}

/*
*	�����޸�
*
*	˵������ͨ�������ʹ��ƽ��������ʽ���е㵽��߽����
*/
void Mid_Line_Repair(uchar count, uchar *data)
{
	int res;
	uchar i;	// ������
	uchar Hazard_Width;
	
//	if (Left_Hazard_Flag)
//	{
//		Line_Count = Left_Hazard_Flag;
//		Hazard_Width = Left_Add_Line[Left_Hazard_Flag];
//		for (i = 61; i >= Left_Hazard_Flag+2;)
//		{
//			i -= 2;
//			Left_Add_Line[i] = 0.7*Hazard_Width + 0.3*Left_Add_Line[i];
//		}
//	}
//	if (Right_Hazard_Flag)
//	{
//		Line_Count = Right_Hazard_Flag;
//		Hazard_Width = Right_Add_Line[Right_Hazard_Flag];
//		for (i = 61; i >= Right_Hazard_Flag+2;)
//		{
//			i -= 2;
//			Right_Add_Line[i] = 0.7*Hazard_Width + 0.3*Right_Add_Line[i];
//		}
//	}
	
	for (i = 61; i >= count+2;)
	{
		i -= 2;
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ���������е�
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// ������������
			
	/**//*************************** ��λ����ʾ�߽� ***************************/
	/**/data[i*160 + Left_Add_Line[i] + 6] = 0;	// ��λ����ʾ���ߺ����߽磬����ʱ����
	/**/data[i*160 + Right_Add_Line[i] - 6] = 0;	// ��λ����ʾ���ߺ���ұ߽磬����ʱ����
	/**/data[i*160 + Mid_Line[i]] = 0;			// ��λ����ʾ���ߣ�����ʱ����
	/**/data[i*160 + Left_Line[i] + 1] = 0;		// ��λ����ʾԭʼ��߽磬����ʱ����
	/**/data[i*160 + Right_Line[i] - 1] = 0;		// ��λ����ʾԭʼ�ұ߽磬����ʱ����
	/**//*************************** ��λ����ʾ�߽� ***************************/
	}
	Mid_Line[61] = Mid_Line[59];
}

/****************** ���㷨 ******************/

/*
*	��Ȩƽ��
*
*	˵����Ȩ�����Ҵյģ�Ч������
*/
uchar Point_Weight(void)
{
	uchar i ,Point, Point_Mid;
	static char Last_Point = 80;
	int32 Sum = 0, Weight_Count = 0;
	
	if (Line_Count <= 20)
	{
		Line_Count = 20;
	}
	
	if (Out_Side || Line_Count >= 53)	//�����������ͷͼ���쳣
	{
		if (Last_Point == 80)
		{
			Point = Last_Point;
		}
		else if (Last_Point < 80)
		{
			Point = 1;
		}
		else if (Last_Point > 80)
		{
			Point = 159;
		}
	}
	else
	{
		for (i = 61; i >= Line_Count; )		//ʹ�ü�Ȩƽ��
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 1, 159);

							/*** �ϰ�������������� ***/
//		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Left_Hazard_Flag]+6;	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Left_Hazard_Flag < 80)
//			{
//				Point += 3;
//			}
//		}
//		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Right_Hazard_Flag]-6;//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Right_Hazard_Flag < 80)
//			{
//				Point -= 5;
//			}
//		}
//		Point = Mid_Line[59];
		Point = range_protect(Point, 2, 158);
		Last_Point = Point;
		
							/***** ʹ����Զ�����ݺ�Ŀ�����Ϊǰհ *****/
		if (Line_Count >= 25)
		{
			Point_Mid = Mid_Line[60-30];
		}
		else
		{
			Point_Mid = Mid_Line[60-Line_Count];
		}
	}
	Foresight = 0.8 * Error_Transform(Point_Mid, 80)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
			  + 0.2 * Error_Transform(Point, 	 80);
	
	return Point;
}

char Error_Transform(uchar Data, uchar Set)
{
	char Error;
	
	Error = Set - Data;
	if (Error < 0)
	{
		Error = -Error;
	}
	
	return Error;
}



/*!
*  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
*  @param      dst             ͼ���ѹĿ�ĵ�ַ
*  @param      src             ͼ���ѹԴ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  while(srclen --)
  {
    tmpsrc = *src++;
    *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
  }
}

/*!
*  @brief      ��ֵ��ͼ��ѹ�����ռ� �� ʱ�� ѹ����
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  uint8 i, j, k;
  uint32 Count = 0;
  
  for (i = 0; i < 60/2; i++)
  {
	  for (j = 0; j < 160/8; j++)
	  {
		  Count = (i*2+1)*160+j*8;
		  for (k = 0; k < 8; k++)		  
		  {
			  tmpsrc <<= 1;
			  if (!dst[Count++])
			  {
				  tmpsrc |= 1;
			  }
		  }
		  *src++ = tmpsrc;
	  }
  }
}

/*!
*  @brief      ȡ�߽���
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_getline(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc,buff_y,buff_x;
  
  while(srclen --)
  {
    tmpsrc=0;
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x80;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x40;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x20;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x10;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x08;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x04;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x02;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if((*dst != buff_y && srclen % 10 !=0 ) || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x01;
      *(dst-1)=colour[black];
    }
    
    *src++ = tmpsrc;
  }
}

/*!
*  @brief      �����ֵ
*  @param      image           ͼ��Դ��ַ
*  @param      col             ͼ��col
*  @param      row             ͼ��row
*  @since      v5.0            image_threshold = OtsuThreshold(image[0],COL,ROW);
*  Sample usage:
*/
uint8 OtsuThreshold( uint8 *image, uint16 col, uint16 row)
{
     uint16 OtsuThresholdOut;
     uint16 GrayList[256]={0};   //�Ҷ�ֵ����ͳ������
     uint16 i, pixelSum = col * row;   //ͼ���С
     uint16 Camera_Graylevel=0;
     uint16 Graylevel=0;         //�Ҷ�ֵ
     uint16 cnts=0,cnt=0;
     float avfor=0,avback=0; 
     float total=0,total_low=0,total_high=0,Variance=0,VarianceMax=0;
     //���ֱ��ͼ
     for(Graylevel=0;Graylevel<256;Graylevel++)
     {
       GrayList[Graylevel]=0;
     }
     //����ֱ��ͼ

       for(i=0;i<pixelSum;i++)
       {   
         Camera_Graylevel=*(image+i);//��ȡ�Ҷ�ֵ
         GrayList[Camera_Graylevel]+=1;//ֱ��ͼͳ��ͼ���������ص�
         total+=Camera_Graylevel;//total����ͼ��ĻҶ�ֵ�ܺ�
       }

     for(Graylevel=0;Graylevel<256;Graylevel++)
     {
       cnt=GrayList[Graylevel];//��ǰ�Ҷ�ֵ�ļ�������
       if(cnt==0)   continue;
       total_low+=cnt*Graylevel;//����Ҷ�ֵ��0����ǰֵ�����ص�Ҷ�ֵ���ܺ�
       cnts+=cnt;//����Ҷ�ֵ��0����ǰֵ�����ص���Ŀ���ܺ�

       if(cnts==pixelSum)  break;//ͳ�����������ص㣬֮��ĻҶ�ֵ�Ѿ�û���κ����ص���ϣ�������ϣ��˳�
       
       total_high=total-total_low;//�����Ҷ�=�Ҷ��ܺ�-ǰ���Ҷ�(��û�й�һ��)
       avfor=(int)(total_low/cnts);//ǰ����һ��

         avback=(int)(total_high/(pixelSum-cnts));//������һ�� 
         Variance=(avback-avfor)*(avback-avfor)*(cnts*pixelSum-cnts*cnts)/(pixelSum*pixelSum);//���㷽�� 
           
       if(VarianceMax<Variance)
       {
         VarianceMax=Variance;
         OtsuThresholdOut=Graylevel;//�������ĻҶ�ֵ��Ϊ�����ֵ
       }
     }
     return OtsuThresholdOut;
}

/*
 * ��򷨼�����ֵ����ͼ����ж�ֵ��
*/
void handlegray(void)
{
	uint8 *p;
	int image_threshold;
	p = image[0];
	image_threshold = OtsuThreshold(image[0],COL,ROW);
    //uart_putchar(uart2,0x00);uart_putchar(uart2,0xff);uart_putchar(uart2,0x01);uart_putchar(uart2,0x01);//��������
    for(int i = 0; i < ROW; i++)
    {
		for(int j = 0;j < COL;j++)
		{
			if(image[i][j] > image_threshold)  
			{
				//uart_putchar(uart2,0xff);
				image[i][j] = 1;
			}
        	else
			{
				//uart_putchar(uart2,0x00);
				image[i][j] = 0;
			}
		}
    }


}