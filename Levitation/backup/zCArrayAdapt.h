#pragma once

template <class T>
class zCArrayAdapt
{
public:
	unsigned int GetSizeInBytes() const
	{
		return NumInArray * sizeof(T);
	}

	T Get(unsigned int idx)
	{
		return Array[idx];
	}

	T* Array;
	int	NumInArray;
};