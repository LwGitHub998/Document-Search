//数据库相关操作
#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "Common.h"
// SqliteManager是对Sqlite的接口进行一层简单的封装
class SqliteManager   //库的操作
{
public:
	SqliteManager()
		:_db(nullptr)
	{}
	~SqliteManager()
	{
		Close();
	}
	void Open(const string& path);
	void Close();
	void ExecuteSql(const string& sql);
	void GetTable(const string& sql, int& row, int& col, char**& ppRet);
	//针对gettable这个接口进行了RAII的封装
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	sqlite3* _db; // 数据库对象指针
};

//RAII

class AutoGetTable//智能
{
public:
	AutoGetTable(SqliteManager& sm, const string& sql, int& row, int& col, char**& ppRet)
	{
		sm.GetTable(sql, row, col, ppRet); //往ppRt中读取数据库的数据
		_ppRet = ppRet;
	}

	~AutoGetTable()
	{
		sqlite3_free_table(_ppRet);
	}

	AutoGetTable(const AutoGetTable&) = delete;
	AutoGetTable operator = (const AutoGetTable&) = delete;
private:
	char** _ppRet;

};


///////////////////////////////////////////
//数据管理模块
#define TB_NAME "tb_doc" //这里是表的名字table
#define DB_NAME "db_name.db"  //这里是一个数据库的名字，可在这里直接修改 database

//sqligt的线程安全问题
class DataManager //表的操作
{
public:
	void Init();//建表
	void GetDoc(const string& path, std::set<string>& dbset);//查找path目录下的文档
	void InsertDoc(const string& path, const string& doc);
	void DeleteDoc(const string& path, const string& doc);
	void Search(const string& key, vector<std::pair<string, string>>&docinfos);//查找
	void SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);	//切割高亮

private:
	SqliteManager _dbmgr;
};
namespace my
{
	//为了方便进行加锁，我们将DataManager设计成为单例
	class DataManager //表的操作
	{
	public:
		static DataManager* GetInstance()
		{
			static DataManager datamgr;
			datamgr.Init();

			return &datamgr;
		}
		void Init();//建表
		void GetDoc(const string& path, std::set<string>& dbset);//查找path目录下的文档
		void InsertDoc(const string& path, const string& doc);
		void DeleteDoc(const string& path, const string& doc);
		void Search(const string& key, vector<std::pair<string, string>>&docinfos);//查找
		void SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);	//切割高亮

	private:
		DataManager(){};
		SqliteManager _dbmgr;
		std::mutex _mtx;
	};
	//将所有使用到datamgr 替换成为DataManager::GetInstance(), 这样就降低了几个类之间的耦合关系，解耦
	//直接使用DataManager::GetInstance();  完成操作
};