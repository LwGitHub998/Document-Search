//数据
#define _CRT_SECURE_NO_WARNINGS 1
#include "DataManager.h"
#include <assert.h>
void SqliteManager::Open(const string& path)
{
	//sqlitr一个小型的数据库
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
void SqliteManager::ExecuteSql(const string& sql)//执行数据库语句
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
	assert(_db);  //断言表是否打开，没打开就不能拿到表
	char* errmsg;
	int ret = sqlite3_get_table(_db, sql.c_str(), &ppRet, &row, &col, &errmsg);
	// 查询数据库    数据库   sql语句   查询结果 查询多少行  多少列  错误信息
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
//数据库管理模块
void DataManager::Init()//想要有一个表，所以打开数据库，建表。。。。
{
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
	_dbmgr.Open(DB_NAME);//打开数据库
	
	char sql[256];
	sprintf(sql, "create table if not exists tb_doc (id INTEGER PRIMARY KEY, path text, name text,\
										name_pinyin text, name_initials)");//建表
	_dbmgr.ExecuteSql(sql);//执行建表语句

}
void DataManager::GetDoc(const string& path,std::set<string>& dbset)//查找数据库所保存path目录下的文档，并放入set中
{
	char sql[256];
	sprintf(sql, "select name from %s where path = '%s'", TB_NAME,  path.c_str());
	int row, col;
	char** ppRet;
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);//获取数据资源。放入到ppRet中，并且有只能指针托管
	//lock.unlock();
	//
	for (int i = 1; i < row; ++i)//注意行列关系
	{
		for (int j = 0; j <= col; ++j)
		{
			dbset.insert(ppRet[i*col + j]); //保存ppRet从数据库中读取的资源
		}
	}
	//因为将ppRet赋值给了智能指针，因此，智能指针会自动替我们释放这些资源
}
void DataManager::InsertDoc(const string& path, const string& name)
{
	char sql[256];
	//汉字转拼音
	string pinyin = ChineseConvertPinYinAllSpell(name);
	string initials = ChineseConvertPinYinInitials(name);
	sprintf(sql, "insert into %s(path,name,name_pinyin,name_initials) values('%s', '%s','%s','%s')", TB_NAME, path.c_str(), name.c_str(), pinyin.c_str(), initials.c_str());
	
	//std::unique_lock<std::mutex> lock(my::DataManager._mtx);

	_dbmgr.ExecuteSql(sql);
}
void DataManager::DeleteDoc(const string& path, const string& name)//删除注意整个文件夹都删了怎么办
{
	char sql[256];
	sprintf(sql, "delete from %s where path = '%s' and name = '%s'", TB_NAME, path.c_str(), name.c_str());

	string path_ = path;
	path_ += '\\';
	path_ += name;
	sprintf(sql, "delete from %s where path like '%s%'", TB_NAME, path_.c_str());//这里删的是文件夹下的子文件

	_dbmgr.ExecuteSql(sql);
}

void DataManager::Search(const string& key, vector<std::pair<string, string>>&docinfos)
{
	//char sql[256];
	//string like_key = "%";
	//like_key += key;
	//like_key += "%";
	//sprintf(sql, "select name, path from %s where name like '%s'", TB_NAME,like_key.c_str());
	////like 语句模糊匹配用%，还是转义字符，所以如上
	//
	//int row, col;
	//char** ppRet;
	//AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//for (int i = 1; i < row; ++i)//注意行列关系
	//{
	//	docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	//}
	
	//上下两种编码不同，导致搜索出来的结果
	//1.统一编码
	//2.直接使用拼音搜索
	char sql[256] = { '\0' };
	{
		//拼音搜索 or 首字母搜索
		string pinyin = ChineseConvertPinYinAllSpell(key);
		string initials = ChineseConvertPinYinInitials(key);
		sprintf(sql, "select name, path from %s where name_pin like '%%%s%%' or name_initials like '%%%s%%'", TB_NAME, pinyin.c_str(), initials.c_str());	//二者选一

		int row, col;
		char** ppRet;

		
		//为了只锁这一句，添加一个作用域，但是出了作用域agt会自动释放
		//std::unique_lock<std::mutex> lock(my::DataManager._mtx);
		AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
		//lock.unlock();


		//这不能同时使用，两种解决方法   加作用域(使用完就释放)  函数明不同
		for (int i = 1; i < row; ++i)//注意行列关系
		{
			docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
		}
	}
	//不同作用域，但是两次搜索冲突，拼音和首字母搜索二次打印
	//使用set进行过滤，但是需要拼接path与name，需要付出代价
	//{
	//	//首字母搜索
	//	string initials = ChineseConvertPinYinInitials(key);
	//	sprintf(sql, "select name, path from %s where name_initials like '%%%s%%'", TB_NAME, initials.c_str());
	//	int row, col;
	//	char** ppRet;
	//	AutoGetTable agt(_dbmgr, sql, row, col, ppRet);
	//	for (int i = 1; i < row; ++i)//注意行列关系
	//	{
	//		//拼音搜索
	//		docinfos.push_back(std::make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	//	}
	//}
	
}

void DataManager::SetHighlight(const string& str, const string& key, string& prefix, string& highlight, string& suffix)//切割高亮
{
	//原串与key串都有可能是汉字，拼音，字符，数字的组合，需要注意

	//简单版本  汉字匹配	直接匹配  key是原串的字串
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
	//拼音匹配	key是拼音全拼
	{
	//基于GBK编码
	//使用key的拼音去匹配str的全拼
	string key_ap = ChineseConvertPinYinAllSpell(key);
	string str_ap = ChineseConvertPinYinAllSpell(str);

	size_t ht_index = 0;  //高亮末尾位置
	size_t ap_index = 0;
	size_t ht_start = 0, ht_len = 0;  //高亮起始位置   //高亮长度
	size_t ap_start = str_ap.find(key_ap);	//高亮起始

	if (ap_index != string::npos)
	{
		size_t ap_end = ap_start + key_ap.size();	//如果key是汉字+拼音的组合
		while (ap_index < ap_end)
		{
			//高亮位置在起始位置
			if (ap_index == ap_start)
			{
				ht_start = ap_index;
			}
			if (str[ht_index] >= 0 && str[ht_index] <= 127)//ASCII码
			{
				++ht_index;
				++ap_index;
			}
			else
			{
				//汉字
				char chinese[3] = { '\0' };
				//取一个汉字
				chinese[0] = str[ht_index];  //str中保存的是汉字
				chinese[1] = str[ht_index + 1];
				//需要使用整个字符串去匹配，不能只使用某个字符
				string ap_str = ChineseConvertPinYinAllSpell(chinese);  //转为拼音
				//汉字的gbk长度为两个字符，挑一个汉字
				ht_index += 2;
				//全拼则跳过这个汉字对应的拼音的个数
				ap_index += ap_str.size();
			}
			//直到ap_index=ap_start是，
		}
		ht_len = ht_index - ht_start;
		prefix = str.substr(0, ht_start);
		highlight = str.substr(ht_start, ht_len);
		suffix = str.substr(ht_start + ht_len, string::npos);
		return;
	}
}
	//3 key是拼音的首字母
	{
		//汉字+首字母	拼音+首字母
		//全部转化为拼音
		string init_str = ChineseConvertPinYinInitials(str);
		//转义key是为了防止混杂
		string init_key = ChineseConvertPinYinInitials(key);
		size_t init_start = init_str.find(init_key);
		if (init_start != string::npos)
		{
			size_t init_end = init_start + init_key.size();
			size_t init_index = 0, ht_index = 0;
			size_t ht_start = 0, ht_len = 0;
			while (init_index < init_end)
			{
				//高亮位置在起始位置
				if (init_index == init_start)
				{
					ht_start = ht_index;	//保存起始位置
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)//ASCII码
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
	//都没匹配上，就不高亮了
	TRACE_LOG("SetHighlight no match str:%s, key:%s", str.c_str(), key.c_str());	//没匹配上打印一下
	prefix = str;
}