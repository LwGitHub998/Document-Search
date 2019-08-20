//扫描管理模块
//目录是一个树形结构，windows是森林结构（多棵树）
//此项目下的树形结构是左孩子右兄弟表示方法，左边放孩子，右边是兄弟，方便
//为什么不用而二叉树，因为一层要是有n个孩子，表示不了
//windows下是FindFirstFile，麻烦因为是宽字符(wstring),但是你搜索的时候用的是string
//C++用的是_findfirst
//改进
//扫描目录动态
//单线程扫描
//多线程扫描多个盘
//动态监控目录变化
//linux不支持拼音搜索，并且需要字符集转换

#pragma once
#include "Common.h"
#include "DataManager.h"

class ScanManager   //扫描
{
	//改造为单利模式，利用线程去扫描
public:
	ScanManager()	//构造函数私有化
	{
		_datamgr.Init();
	}
	void Scan(const string& path);//递归接口
private:
	DataManager _datamgr;//数据库
};

namespace my
{
//ScanManager设计成单例，整个程序只有一个扫描模块
	class ScanManager   //扫描
	{
		//改造为单例模式，利用线程去扫描
	public:

		void Scan(const string& path);//递归接口
		void StartScan()  //采用一个线程一中去扫描磁盘目录
		{
			while (1)
			{
				Scan("D:\\C语言\\C++");	//先把目录写死
				//std::this_sleep_for 休眠
				Sleep(10);
			}
		}
		//1.创建对象必须调用CreateIntaince
		static ScanManager* CreateIntaince()
		{
			//饿汉模式
			static ScanManager  scanmgr;
			static std::thread thd(&StartScan, &scanmgr);  //创建一个线程去执行
			thd.detach(); //线程分离，这个线程只负责扫描
			return &scanmgr;
		}
		
	private:
		ScanManager()	//构造函数私有化
		{
			//_datamgr.Init();
		}
		ScanManager(const ScanManager&);
		//DataManager _datamgr;//数据库
		//vector<string> entrys:	动态目录
	};
};

