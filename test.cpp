#include "Common.h"
#include "DataManager.h"
#include "ScanManager.h"

/*
������һ����򵥵��������������µĹ���
�������ַ�������ƴ��������ģ��ƥ�䡢����ƥ��
������������sqlite���ݿ⣬����С�����ݿ⣬��������̫��ͻ����
����������MySQL����Ҫ��װ���ϴ���Դ�˷ѣ�
������windows�µ�Ŀ¼�ṹ	�������ֵ�
������windows�µ��ļ������ӿ�
������_findfirst()
������_findnext()

������FindFirstFile

������linxu
������findfirst()
������findnext()
������ �೤ʱ��ɨ��һ�Σ�ʹ�ö��߳�ʵʱ��ɨ���˷�cpu��Դ��ͬʱ�漰�̰߳�ȫ������ʵʱ�Ժã���ʱ��ɨ����Ӱ��ʵʱ�ԣ�ʹ�ü�صķ�ʽ
*/
void TestDirectoryList()//��һ��Ŀ¼�µ������ĵ�
{
	vector<string> dirs;
	vector<string> files;
	DirectoryList("D:\\C����\\C++",dirs,files);//ע��ת���ַ���������������\\��
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

	string insert_sql = "insert into tb_doc values(NULL, '���ؿμ�','C++')";
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

	AutoGetTable agt(sq, query_sql, row, col, ppRet);//�������⣬Խ�磿����------�����д�
	for (int i = 0; i < row-1; ++i)
	{
		for (int j = 0; i < col; ++j)
		{
			cout << ppRet[i*col + j] << " ";
		}
		cout << endl;
	}
}
//����������ʾ�ƶ�
void TestHighLight()
{
	// ���� 
	{
		//1  ����key������key
		////ColourPrintf("liang\n");	ֻ����һ����
		string key = "����";
		string str = "�Ǻ͹����Ǻ�";	//ֻ��abc���и�����Σ�
		size_t pos = str.find(key);
		string prefix, suffix;	//ǰ׺/��׺
		prefix = str.substr(0, pos);
		suffix = str.substr(pos + key.size(), string::npos);
		cout << prefix;
		ColourPrintf(key.c_str());
		cout << suffix << endl;
	}
	{

	/*
	strת��Ϊstr_py��Դ����ƴ������
	Ȼ��key��ʼλ����str_py��ͬ�ַ���λ�ö�Ӧ����¼��ʼλ��
	˫ָ�룬ͬʱ��key��str_pyͬʱ�ߣ�
	key�ϵ��ַ�����str����py��ָ��ָ����ǽ���λ�ã�
	�ڸ��ݿ�ʼ��λ�ú��ߵĽ���λ�ã���str_py���ֳ���������


	�����������/��ĸ�ͻ���ִ�λ����Ϊ���ǰ����еĶ����ɺ��ִ�����,���� ��Ҫ������Щ���֣��ڽ���ѡ��

	*/



	//2.key��ƴ������������
		string key = "hehe";
		string str = "�Ǻ͹����Ǻ�";	
		string prefix, suffix;	//ǰ׺/��׺
		string str_py = ChineseConvertPinYinAllSpell(str);
		string key_py = ChineseConvertPinYinAllSpell(key);
		//ʹ��keyȥ�ȶ�ƴ����Ӧ�Ĳ��֣��ٶԱȵ�����,���忴¼���������ڿκ�벿��
		size_t pos = str_py.find(key_py);
		if (pos == string::npos)
		{
			cout << "��ƥ��" << endl;
		}
		else
		{
			
			size_t key_start = pos;
			size_t key_end = pos + key_py.size();
			

			size_t str_i = 0, py_i = 0;
			while (py_i < key_start)
			{
				if (str[str_i] <= 0 && str[str_i]>=127)	////asiik��
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
				if (str[str_j] <= 0 && str[str_j] >= 127)	//asiik��
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
		//3.key��ƴ������ĸ��������Ӧ�ĺ���
		string key = "hh";
		string str = "�Ǻ͹����Ǻ�";	//��ͻ����	,ģ��ƥ��
		string prefix, suffix;	//ǰ׺/��׺
	}
	
	
}

void TestScanManager()
{
	ScanManager scanmgr;
	//scanmgr.Scan("D:\\");
	scanmgr.Scan("G:\\233\\25");//ɨ���·��
	//ScanManager::CreateIntaince();  //ɨ�����ѯ�ǻ���ӡ���
}
void TestSarch()
{
	//���ڱ������⣬�������Ŀ��ܻ���ֶ��������
	DataManager datamgr;
	datamgr.Init();	//��ʼ��
	vector<std::pair<string, string>> docinfos;
	string key;
	cout << "===================" << endl;
	cout << "������Ҫ����Ĺؼ���";
	
	while (std::cin >> key)
	{
		docinfos.clear();	//ɾ��������vector�е�����Ԫ��. 
		datamgr.Search(key,docinfos);	//ÿһ���õ�֮����Ҫ���һ��
		string s1 = string("�ҵĽ��������ܺ�");
		string s2 = string("G:\\233\\25\\����");
		key = "jt";
		docinfos.push_back(std::make_pair(s1, s2));
		printf("%-50s %-50s\n", "���� ", "·��");
		for (const auto& e : docinfos)
		{
			//cout << e.first << "   " << e.second << endl;
			//printf("%-50s %-50s\n", e.first.c_str(), e.second.c_str());
			string prefix, highlight, suffix;

			const string& name = e.first;
			const string& path = e.second;
			//�Ա�һ��
			datamgr.SetHighlight(name, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;
			
			//����50���ո�
			for (size_t i = name.size(); i < 50; i++)
			{
				cout << " ";
			}
			printf("%-50s\n", path.c_str());
		}
		cout << "===================" << endl;
		cout << "������Ҫ����Ĺؼ���";
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
//����һ���ֻ���������ĸ�����ķ�Χ
//ֻ������һ��

//������	����ģʽ	RAII	����ƥ���㷨	sqlite3	c++11  thread/mutx
//����	
//ɨ��   �Ա�����  ��������  
//likeģ��ƥ��	ƴ������
//������ʾ
//�Ľ�
//ɨ���Ŀ¼	Ч������	ɨ�����ݲ�һ������	ɨ��+��أ����߳�ɨ��
//����תƴ��  ��Ƨ��	����һ�������֣����Ѹ��ʴ�
//û�н���������⣬�߶�����GBK���������ʽ��ע��ת��


/*��
1.����һ��ɨ���࣬����ɨ�����е�scan�����������ļ�Ŀ¼��Ϣ���浽���ݿ���
2.����һ����Ĳ����࣬�û������ѯ�ַ����󣬵��ò�ѯ���������ݿ��а���key�Ľ��в�ѯ��������뵽vector�У�Ȼ��Բ�ѯ������и�����ʾ
 

ɨ����
{
	��Ĳ��������
	{
		��ʼ��   �����ݿ⣬����
		��ȡ�������ݿ��е���Դ��scan�е��ã�������ӱ����ļ䣩 RAII
		����в���    scan�����е���
		�ӱ���ɾ��
		�����û�����key��ѯ��Ա RAII
		������ʾ

		���ݿ��������
		{
			�����ݿ�
			�ر����ݿ�
			ִ��sql���
			�����ݿ��в�ѯ  ��Ҫ����RAII�Ĺ��캯��������

			���ݿ����
		}
	}


	scan����
}







*/