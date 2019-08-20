#include "Common.h"
#include "DataManager.h"
#include "ScanManager.h"

/*
、、、一个最简单的搜索，加入了新的功能
、、、字符搜索，拼音搜索，模糊匹配、高亮匹配
、、、轻量级sqlite数据库，本地小型数据库，但数据量太大就会较慢
、、、不用MySQL，需要安装，较大，资源浪费，
、、、windows下的目录结构	左孩子有兄弟
、、、windows下的文件遍历接口
、、、_findfirst()
、、、_findnext()

、、、FindFirstFile

、、、linxu
、、、findfirst()
、、、findnext()
、、、 多长时间扫描一次，使用多线程实时性扫描浪费cpu资源，同时涉及线程安全，但是实时性好，长时间扫描又影响实时性，使用监控的方式
*/
void TestDirectoryList()//找一层目录下的所有文档
{
	vector<string> dirs;
	vector<string> files;
	DirectoryList("D:\\C语言\\C++",dirs,files);//注意转义字符，所以有两个“\\”
	for (const auto& e : dirs)
	{
		cout << e << endl;
	}
	for (const auto& e : files)
	{
		cout << e << endl;
	}
}

void TestSqlite()
{
	SqliteManager sq;
	sq.Open("test.db");
	string createtb_sql = "create table tb_doc(id INTEGER PRIMARY KEY AUTOINCREMENT, doc_path text, doc_name text)";
	sq.ExecuteSql(createtb_sql);

	string insert_sql = "insert into tb_doc values(NULL, '比特课件','C++')";
	sq.ExecuteSql(insert_sql);

	string insert_sql2 = "insert into tb_doc values(NULL, 'snajio','C++')";
	sq.ExecuteSql(insert_sql2);

	string query_sql = "select * from tb_doc where doc_path = 'snajio'";
	int row, col;
	char** ppRet;
	/*sq.GetTable(query_sql, row, col, ppRet);
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; i < col; ++j)
		{
			cout << ppRet[i*col+j]<<" ";
		}
		cout << endl;
	}*/
	//sqlite3_free_table(ppRet);

	AutoGetTable agt(sq, query_sql, row, col, ppRet);//存在问题，越界？？、------》行列错
	for (int i = 0; i < row-1; ++i)
	{
		for (int j = 0; i < col; ++j)
		{
			cout << ppRet[i*col + j] << " ";
		}
		cout << endl;
	}
}
//高亮操作演示推断
void TestHighLight()
{
	// 测试 
	{
		//1  输入key，高亮key
		////ColourPrintf("liang\n");	只是这一段亮
		string key = "哈哈";
		string str = "呵和哈哈呵和";	//只亮abc，切割成三段，
		size_t pos = str.find(key);
		string prefix, suffix;	//前缀/后缀
		prefix = str.substr(0, pos);
		suffix = str.substr(pos + key.size(), string::npos);
		cout << prefix;
		ColourPrintf(key.c_str());
		cout << suffix << endl;
	}
	{

	/*
	str转化为str_py，源串到拼音串，
	然后将key开始位置与str_py相同字符的位置对应，记录开始位置
	双指针，同时再key与str_py同时走，
	key上的字符走完str——py的指针指向就是结束位置，
	在根据开始的位置和走的结束位置，将str_py划分成三个部分


	如果出现数字/字母就会出现错位，因为我们把所有的都当成汉字处理了,所以 需要跳过这些部分，在进行选择

	*/



	//2.key是拼音，高亮汉字
		string key = "hehe";
		string str = "呵和哈哈呵和";	
		string prefix, suffix;	//前缀/后缀
		string str_py = ChineseConvertPinYinAllSpell(str);
		string key_py = ChineseConvertPinYinAllSpell(key);
		//使用key去比对拼音相应的部分，再对比到汉字,具体看录屏，第三节课后半部分
		size_t pos = str_py.find(key_py);
		if (pos == string::npos)
		{
			cout << "不匹配" << endl;
		}
		else
		{
			
			size_t key_start = pos;
			size_t key_end = pos + key_py.size();
			

			size_t str_i = 0, py_i = 0;
			while (py_i < key_start)
			{
				if (str[str_i] <= 0 && str[str_i]>=127)	////asiik码
				{
					++str_i;
					++py_i;
				}
				else
				{
					char chinese[3] = { '\0' };
					chinese[0] = str[str_i];
					chinese[1] = str[str_i + 1];
					str_i += 2;
					string py_str = ChineseConvertPinYinAllSpell(chinese);
					py_i += py_str.size();
				}
				
			}
			prefix = str.substr(0, str_i);

			size_t str_j = str_i, py_j = py_i;
			while (py_j < key_end)
			{
				if (str[str_j] <= 0 && str[str_j] >= 127)	//asiik码
				{
					++str_j;
					++py_j;
				}
				else
				{
					char chinese[3] = { '\0' };
					chinese[0] = str[str_j];
					chinese[1] = str[str_j + 1];
					str_j += 2;
					string py_str = ChineseConvertPinYinAllSpell(chinese);
					py_j += py_str.size();
				}
			}
			key = str.substr(str_i, str_j - str_i);
			suffix = str.substr(str_j, string::npos);

			cout << prefix;
			ColourPrintf(key.c_str());
			cout << suffix << endl;
		}
	}
	{
		//3.key是拼音首字母，高亮对应的汉字
		string key = "hh";
		string str = "呵和哈哈呵和";	//冲突问题	,模糊匹配
		string prefix, suffix;	//前缀/后缀
	}
	
	
}

void TestScanManager()
{
	ScanManager scanmgr;
	//scanmgr.Scan("D:\\");
	scanmgr.Scan("G:\\233\\25");//扫描的路径
	//ScanManager::CreateIntaince();  //扫描与查询是互不印象的
}
void TestSarch()
{
	//由于编码问题，搜索中文可能会出现多余的数据
	DataManager datamgr;
	datamgr.Init();	//初始化
	vector<std::pair<string, string>> docinfos;
	string key;
	cout << "===================" << endl;
	cout << "请输入要输入的关键字";
	
	while (std::cin >> key)
	{
		docinfos.clear();	//删除储存在vector中的所有元素. 
		datamgr.Search(key,docinfos);	//每一次拿到之后需要清空一下
		string s1 = string("我的今天天气很好");
		string s2 = string("G:\\233\\25\\今天");
		key = "jt";
		docinfos.push_back(std::make_pair(s1, s2));
		printf("%-50s %-50s\n", "名称 ", "路径");
		for (const auto& e : docinfos)
		{
			//cout << e.first << "   " << e.second << endl;
			//printf("%-50s %-50s\n", e.first.c_str(), e.second.c_str());
			string prefix, highlight, suffix;

			const string& name = e.first;
			const string& path = e.second;
			//对比一下
			datamgr.SetHighlight(name, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;
			
			//补齐50个空格
			for (size_t i = name.size(); i < 50; i++)
			{
				cout << " ";
			}
			printf("%-50s\n", path.c_str());
		}
		cout << "===================" << endl;
		cout << "请输入要输入的关键字";
	}
}
int main()
{
	//TestDirectoryList();
	//TestSqlite();
	TestScanManager();
	TestSarch();
	system("pause");
	return 0;
}


//bug
//输入一个字会跳到首字母搜索的范围
//只会搜索一次

//简历中	单例模式	RAII	高亮匹配算法	sqlite3	c++11  thread/mutx
//面试	
//扫描   对比数据  更新数据  
//like模糊匹配	拼音搜索
//高亮显示
//改进
//扫描大目录	效率问题	扫描数据不一致问题	扫描+监控，多线程扫描
//汉字转拼音  生僻字	输入一个两个字，误搜概率大
//没有解决编码问题，高度依赖GBK，检测编码格式，注意转换


/*、
1.创建一个扫描类，调用扫描类中的scan函数将本地文件目录信息保存到数据库中
2.创建一个表的操作类，用户输入查询字符串后，调用查询函数对数据库中包含key的进行查询，结果放入到vector中，然后对查询结果进行高亮显示
 

扫描类
{
	表的操作类对象
	{
		初始化   打开数据库，建表
		获取所有数据库中的资源（scan中调用，用于添加本地文间） RAII
		向表中插入    scan函数中调用
		从表中删除
		更具用户输入key查询组员 RAII
		高亮显示

		数据库操作对象
		{
			打开数据库
			关闭数据库
			执行sql语句
			在数据库中查询  主要是在RAII的构造函数出调用

			数据库对象
		}
	}


	scan函数
}







*/