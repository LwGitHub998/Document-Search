//ɨ�����ģ��
//Ŀ¼��һ�����νṹ��windows��ɭ�ֽṹ���������
//����Ŀ�µ����νṹ���������ֵܱ�ʾ��������߷ź��ӣ��ұ����ֵܣ�����
//Ϊʲô���ö�����������Ϊһ��Ҫ����n�����ӣ���ʾ����
//windows����FindFirstFile���鷳��Ϊ�ǿ��ַ�(wstring),������������ʱ���õ���string
//C++�õ���_findfirst
//�Ľ�
//ɨ��Ŀ¼��̬
//���߳�ɨ��
//���߳�ɨ������
//��̬���Ŀ¼�仯
//linux��֧��ƴ��������������Ҫ�ַ���ת��

#pragma once
#include "Common.h"
#include "DataManager.h"

class ScanManager   //ɨ��
{
	//����Ϊ����ģʽ�������߳�ȥɨ��
public:
	ScanManager()	//���캯��˽�л�
	{
		_datamgr.Init();
	}
	void Scan(const string& path);//�ݹ�ӿ�
private:
	DataManager _datamgr;//���ݿ�
};

namespace my
{
//ScanManager��Ƴɵ�������������ֻ��һ��ɨ��ģ��
	class ScanManager   //ɨ��
	{
		//����Ϊ����ģʽ�������߳�ȥɨ��
	public:

		void Scan(const string& path);//�ݹ�ӿ�
		void StartScan()  //����һ���߳�һ��ȥɨ�����Ŀ¼
		{
			while (1)
			{
				Scan("D:\\C����\\C++");	//�Ȱ�Ŀ¼д��
				//std::this_sleep_for ����
				Sleep(10);
			}
		}
		//1.��������������CreateIntaince
		static ScanManager* CreateIntaince()
		{
			//����ģʽ
			static ScanManager  scanmgr;
			static std::thread thd(&StartScan, &scanmgr);  //����һ���߳�ȥִ��
			thd.detach(); //�̷߳��룬����߳�ֻ����ɨ��
			return &scanmgr;
		}
		
	private:
		ScanManager()	//���캯��˽�л�
		{
			//_datamgr.Init();
		}
		ScanManager(const ScanManager&);
		//DataManager _datamgr;//���ݿ�
		//vector<string> entrys:	��̬Ŀ¼
	};
};

