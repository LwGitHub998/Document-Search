//���ݿ���ز���
#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "Common.h"
// SqliteManager�Ƕ�Sqlite�Ľӿڽ���һ��򵥵ķ�װ
class SqliteManager   //��Ĳ���
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
	//���gettable����ӿڽ�����RAII�ķ�װ
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	sqlite3* _db; // ���ݿ����ָ��
};

//RAII

class AutoGetTable//����
{
public:
	AutoGetTable(SqliteManager& sm, const string& sql, int& row, int& col, char**& ppRet)
	{
		sm.GetTable(sql, row, col, ppRet); //��ppRt�ж�ȡ���ݿ������
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
//���ݹ���ģ��
#define TB_NAME "tb_doc" //�����Ǳ������table
#define DB_NAME "db_name.db"  //������һ�����ݿ�����֣���������ֱ���޸� database

//sqligt���̰߳�ȫ����
class DataManager //��Ĳ���
{
public:
	void Init();//����
	void GetDoc(const string& path, std::set<string>& dbset);//����pathĿ¼�µ��ĵ�
	void InsertDoc(const string& path, const string& doc);
	void DeleteDoc(const string& path, const string& doc);
	void Search(const string& key, vector<std::pair<string, string>>&docinfos);//����
	void SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);	//�и����

private:
	SqliteManager _dbmgr;
};
namespace my
{
	//Ϊ�˷�����м��������ǽ�DataManager��Ƴ�Ϊ����
	class DataManager //��Ĳ���
	{
	public:
		static DataManager* GetInstance()
		{
			static DataManager datamgr;
			datamgr.Init();

			return &datamgr;
		}
		void Init();//����
		void GetDoc(const string& path, std::set<string>& dbset);//����pathĿ¼�µ��ĵ�
		void InsertDoc(const string& path, const string& doc);
		void DeleteDoc(const string& path, const string& doc);
		void Search(const string& key, vector<std::pair<string, string>>&docinfos);//����
		void SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix);	//�и����

	private:
		DataManager(){};
		SqliteManager _dbmgr;
		std::mutex _mtx;
	};
	//������ʹ�õ�datamgr �滻��ΪDataManager::GetInstance(), �����ͽ����˼�����֮�����Ϲ�ϵ������
	//ֱ��ʹ��DataManager::GetInstance();  ��ɲ���
};