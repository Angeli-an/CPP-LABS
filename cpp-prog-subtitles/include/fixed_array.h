#ifndef FIXED_ARRAY_H
#define FIXED_ARRAY_H

template< typename T >
class FixedArray
{
	T* data;
	int capacity;
	int count;

  public:
	FixedArray(int cap = 1000) : capacity(cap), count(0) { data = new T[capacity]; }
	~FixedArray() { delete[] data; }

	void push_back(const T& value)
	{
		if (count >= capacity)
		{
			capacity *= 2;
			T* newData = new T[capacity];
			for (int i = 0; i < count; i++)
				newData[i] = data[i];
			delete[] data;
			data = newData;
		}
		data[count++] = value;
	}

	void clear() { count = 0; }
	int size() const { return count; }
	const T& operator[](int i) const { return data[i]; }
	T& operator[](int i) { return data[i]; }
};

#endif
