#pragma once

/// <summary>
/// 数据读取基类
/// 提供一个个读取字符的操作
/// </summary>
class DataReader
{
public:
	virtual ~DataReader() = default;;

	virtual char getChar() = 0;
};

