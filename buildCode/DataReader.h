#pragma once

/// <summary>
/// ���ݶ�ȡ����
/// �ṩһ������ȡ�ַ��Ĳ���
/// </summary>
class DataReader
{
public:
	virtual ~DataReader() {};

	virtual char getChar() = 0;
};

