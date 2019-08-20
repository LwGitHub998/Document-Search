
#include "ScanManager.h"

void ScanManager::Scan(const string& path)
{
	//�ȶ�  �ļ�ϵͳ�����ݿ�
	vector<string> localdirs;//��Ŀ¼����
	vector<string> localfiles;//���ļ�����
	DirectoryList(path, localdirs, localfiles); //�����ļ�
	
	std::set<string> localset;
	localset.insert(localfiles.begin(), localfiles.end());
	localset.insert(localdirs.begin(), localdirs.end());

	//���ݿ⣺�漰����������-----��Ҫ��һ���߳�
	std::set<string> dbset;//����տ�ʼ�ǿյ�
	_datamgr.GetDoc(path, dbset);
	//�ļ�ɨ�������ݿ�Ա�

	auto localit = localset.begin();
	auto dbit = dbset.begin();
	while (localit != localset.end() && dbit != dbset.end())
	{
		if (*localit < *dbit)//�����У����ݿ�û�У������������ݵ����ݿ�
		{
			_datamgr.InsertDoc(path, *localit);//��Ӧ���е����ݿ�
			++localit;
		}
		else if (*localit>*dbit)//����û�У����ݿ��У�ɾ�����ݿ�����
		{
			_datamgr.DeleteDoc(path, *dbit);//��Ӧ���е����ݿ�
			++dbit;
		}
		else
		{
			++localit;
			++dbit;
		}
	}
	while (localit != localset.end())//��������
	{
		_datamgr.InsertDoc(path, *localit);//��Ӧ���е����ݿ�
		++localit;
	}
	while (dbit != dbset.end())//ɾ������
	{
		_datamgr.DeleteDoc(path, *dbit);//��Ӧ���е����ݿ���ɾ��Ӧ��ע�������ļ��ж�ɾ����ô��
		++dbit;
	}

	//�ݹ�ȶ���Ŀ¼����
	for (const auto& subdirs : localdirs) //localdirs ��Ŀ¼��vector
	{
		string subpath = path;
		subpath += '\\';
		subpath += subdirs; 
		Scan(subpath);//Ҫ�Ǿ���·��
	}
}