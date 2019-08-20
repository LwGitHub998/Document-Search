//����
#define _CRT_SECURE_NO_WARNINGS 1
#include "DataManager.h"
#include <assert.h>
void SqliteManager::Open(const string& path)
{
	//sqlitrһ��С�͵����ݿ�
	int ret = sqlite3_open(path.c_str(), &_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_open\n");
	}
	else
	{
		TRACE_LOG("sqlite3_open success");
	}
}
void SqliteManager::Close()
{
	int ret = sqlite3_close(_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_close\n");
	}
	else
	{
		TRACE_LOG("sqlite3_close success");
	}
}
void SqliteManager::ExecuteSql(const string& sql)//ִ�����ݿ����
{
	assert(_db);

	char* errmsg;
	int ret = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_exec:(%s) errmsg:%s\n", sql.c_str(), errmsg);
		sqlite3_free(errmsg);

	}
	else
	{
		TRACE_LOG("sqlite3_exec:(%s) success",sql.c_str());
	}
}
void SqliteManager::GetTable(const string& sql, int& row, int& col, char**& ppRet)//
{
	assert(_db);  //���Ա��Ƿ�򿪣�û�򿪾Ͳ����õ���
	char* errmsg;
	int ret = sqlite3_get_table(_db, sql.c_str(), &ppRet, &row, &col, &errmsg);
	// ��ѯ���ݿ�    ���ݿ�   sql���   ��ѯ��� ��ѯ������  ������  ������Ϣ
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_get_table:(%s) errmsg:%s\n", sql.c_str(), errmsg);
		sqlite3_free(errmsg);
	}
	else
	{
		TRACE_LOG("sqlite3_get_table:(%s) success", sql.c_str());
	}
}
//���ݿ����ģ��
void DataManager::Init()//��Ҫ��һ�������Դ����ݿ⣬����������
{
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
	_dbmgr.Open(DB_NAME);//�����ݿ�
	
	char sql[256];
	sprintf(sql, "create table if not exists tb_doc (id INTEGER PRIMARY KEY, path text, name text,\
										name_pinyin text, name_initials)");//����
	_dbmgr.ExecuteSql(sql);//ִ�н������

}
void DataManager::GetDoc(const string& path,std::set<string>& dbset)//�������ݿ�������pathĿ¼�µ��ĵ���������set��
{
	char sql[256];
	sprintf(sql, "select name from %s where path = '%s'", TB_NAME,  path.c_str());
	int row, col;
	char** ppRet;
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);//��ȡ������Դ�����뵽ppRet�У�������ֻ��ָ���й�
	//lock.unlock();
	//
	for (int i = 1; i < row; ++i)//ע�����й�ϵ
	{
		for (int j = 0; j <= col; ++j)
		{
			dbset.insert(ppRet[i*col + j]); //����ppRet�����ݿ��ж�ȡ����Դ
		}
	}
	//��Ϊ��ppRet��ֵ��������ָ�룬��ˣ�����ָ����Զ��������ͷ���Щ��Դ
}
void DataManager::InsertDoc(const string& path, const string& name)
{
	char sql[256];
	//����תƴ��
	string pinyin = ChineseConvertPinYinAllSpell(name);
	string initials = ChineseConvertPinYinInitials(name);
	sprintf(sql, "insert into %s(path,name,name_pinyin,name_initials) values('%s', '%s','%s','%s')", TB_NAME, path.c_str(), name.c_str(), pinyin.c_str(), initials.c_str());
	
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);

	_dbmgr.ExecuteSql(sql);
}
void DataManager::DeleteDoc(const string& path, const string& name)//ɾ��ע�������ļ��ж�ɾ����ô��
{
	char sql[256];
	sprintf(sql, "delete from %s where path = '%s' and name = '%s'", TB_NAME, path.c_str(), name.c_str());

	string path_ = path;
	path_ += '\\';
	path_ += name;
	sprintf(sql, "delete from %s where path like '%s%'", TB_NAME, path_.c_str());//����ɾ�����ļ����µ����ļ�

	_dbmgr.ExecuteSql(sql);
}

void DataManager::Search(const string& key, vector<std::pair<string, string>>&docinfos)
{
	//char sql[256];
	//string like_key = "%";
	//like_key += key;
	//like_key += "%";
	//sprintf(sql, "select name, path from %s where name like '%s'", TB_NAME,like_key.c_str());
	////like ���ģ��ƥ����%������ת���ַ�����������
	//
	//int row, col;
	//char** ppRet;
	//AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//for (int i = 1; i < row; ++i)//ע�����й�ϵ
	//{
	//	docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	//}
	
	//�������ֱ��벻ͬ���������������Ľ��
	//1.ͳһ����
	//2.ֱ��ʹ��ƴ������
	char sql[256] = { '\0' };
	{
		//ƴ������ or ����ĸ����
		string pinyin = ChineseConvertPinYinAllSpell(key);
		string initials = ChineseConvertPinYinInitials(key);
		sprintf(sql, "select name, path from %s where name_pin like '%%%s%%' or name_initials like '%%%s%%'", TB_NAME, pinyin.c_str(), initials.c_str());	//����ѡһ

		int row, col;
		char** ppRet;

		
		//Ϊ��ֻ����һ�䣬���һ�������򣬵��ǳ���������agt���Զ��ͷ�
		//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
		AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
		//lock.unlock();


		//�ⲻ��ͬʱʹ�ã����ֽ������   ��������(ʹ������ͷ�)  ��������ͬ
		for (int i = 1; i < row; ++i)//ע�����й�ϵ
		{
			docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
		}
	}
	//��ͬ�����򣬵�������������ͻ��ƴ��������ĸ�������δ�ӡ
	//ʹ��set���й��ˣ�������Ҫƴ��path��name����Ҫ��������
	//{
	//	//����ĸ����
	//	string initials = ChineseConvertPinYinInitials(key);
	//	sprintf(sql, "select name, path from %s where name_initials like '%%%s%%'", TB_NAME, initials.c_str());
	//	int row, col;
	//	char** ppRet;
	//	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//	for (int i = 1; i < row; ++i)//ע�����й�ϵ
	//	{
	//		//ƴ������
	//		docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	//	}
	//}
	
}

void DataManager::SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix)//�и����
{
	//ԭ����key�����п����Ǻ��֣�ƴ�����ַ������ֵ���ϣ���Ҫע��

	//�򵥰汾  ����ƥ��	ֱ��ƥ��  key��ԭ�����ִ�
	{
		size_t ht_start = str.find(key);
		if (ht_start != string::npos)
		{
			prefix = str.substr(0, ht_start);
			highlight = key;
			suffix = str.substr(ht_start + key.size(), string::npos);
			return;
		}

	}
	//ƴ��ƥ��	key��ƴ��ȫƴ
	{
	//����GBK����
	//ʹ��key��ƴ��ȥƥ��str��ȫƴ
	string key_ap = ChineseConvertPinYinAllSpell(key);
	string str_ap = ChineseConvertPinYinAllSpell(str);

	size_t ht_index = 0;  //����ĩβλ��
	size_t ap_index = 0;
	size_t ht_start = 0, ht_len = 0;  //������ʼλ��   //��������
	size_t ap_start = str_ap.find(key_ap);	//������ʼ

	if (ap_index != string::npos)
	{
		size_t ap_end = ap_start + key_ap.size();	//���key�Ǻ���+ƴ�������
		while (ap_index < ap_end)
		{
			//����λ������ʼλ��
			if (ap_index == ap_start)
			{
				ht_start = ap_index;
			}
			if (str[ht_index] >= 0 && str[ht_index] <= 127)//ASCII��
			{
				++ht_index;
				++ap_index;
			}
			else
			{
				//����
				char chinese[3] = { '\0' };
				//ȡһ������
				chinese[0] = str[ht_index];  //str�б�����Ǻ���
				chinese[1] = str[ht_index + 1];
				//��Ҫʹ�������ַ���ȥƥ�䣬����ֻʹ��ĳ���ַ�
				string ap_str = ChineseConvertPinYinAllSpell(chinese);  //תΪƴ��
				//���ֵ�gbk����Ϊ�����ַ�����һ������
				ht_index += 2;
				//ȫƴ������������ֶ�Ӧ��ƴ���ĸ���
				ap_index += ap_str.size();
			}
			//ֱ��ap_index=ap_start�ǣ�
		}
		ht_len = ht_index - ht_start;
		prefix = str.substr(0, ht_start);
		highlight = str.substr(ht_start, ht_len);
		suffix = str.substr(ht_start + ht_len, string::npos);
		return;
	}
}
	//3 key��ƴ��������ĸ
	{
		//����+����ĸ	ƴ��+����ĸ
		//ȫ��ת��Ϊƴ��
		string init_str = ChineseConvertPinYinInitials(str);
		//ת��key��Ϊ�˷�ֹ����
		string init_key = ChineseConvertPinYinInitials(key);
		size_t init_start = init_str.find(init_key);
		if (init_start != string::npos)
		{
			size_t init_end = init_start + init_key.size();
			size_t init_index = 0, ht_index = 0;
			size_t ht_start = 0, ht_len = 0;
			while (init_index < init_end)
			{
				//����λ������ʼλ��
				if (init_index == init_start)
				{
					ht_start = ht_index;	//������ʼλ��
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)//ASCII��
				{
					++ht_index;
					++init_index;
				}
				else
				{
					ht_index += 2;
					++init_index;
				}

			}
			ht_len = ht_index - ht_start;
			prefix = str.substr(0, ht_start);
			highlight = str.substr(ht_start, ht_len);
			suffix = str.substr(ht_start + ht_len, string::npos);
		}
	}
	//��ûƥ���ϣ��Ͳ�������
	TRACE_LOG("SetHighlight no match str:%s, key:%s", str.c_str(), key.c_str());	//ûƥ���ϴ�ӡһ��
	prefix = str;
}