#ifndef NDARRAY_HPP
#define NDARRAY_HPP

#include <initializer_list>
#include <type_traits>

#include <array>
#include <iterator>
#include <stdexcept>

namespace detail
{
	template< typename SizeList >
	size_t compute_total_count(const SizeList& sizes)
	{
		size_t total = 1;
		for (auto s : sizes)
			total *= s;
		return total;
	}

	template< size_t N >
	size_t compute_total_count(const std::array< size_t, N >& sizes)
	{
		size_t total = 1;
		for (size_t i = 0; i < N; ++i)
			total *= sizes[i];
		return total;
	}
}	

template< typename T, size_t N >
class NDArrayConstView
{
  private:
	const T* data_;
	std::array< size_t, N > shape_;
	size_t total_count_;

	template< typename U, size_t M >
	friend class NDArray;
	template< typename U, size_t M >
	friend class NDArrayView;
	template< typename U, size_t M >
	friend class NDArrayConstView;

  public:
	using value_type = T;
	using size_type = size_t;
	using const_view = NDArrayConstView< T, N - 1 >;

	NDArrayConstView(const T* data, const std::array< size_t, N >& shape) :
		data_(data), shape_(shape), total_count_(detail::compute_total_count(shape))
	{
	}

	NDArrayConstView(const NDArrayConstView&) = default;
	NDArrayConstView(NDArrayConstView&&) = default;
	NDArrayConstView& operator=(const NDArrayConstView&) = default;
	NDArrayConstView& operator=(NDArrayConstView&&) = default;

	size_type count() const { return shape_[0]; }
	size_type total_count() const { return total_count_; }
	size_type dim() const { return N; }

	auto operator[](size_type i) const
	{
		if (i >= shape_[0])
			throw std::out_of_range("Index out of range");
		if constexpr (N == 1)
		{
			return static_cast< const T& >(data_[i]);
		}
		else
		{
			std::array< size_t, N - 1 > sub_shape;
			for (size_t j = 1; j < N; ++j)
				sub_shape[j - 1] = shape_[j];
			size_t offset = i * detail::compute_total_count(sub_shape);
			return NDArrayConstView< T, N - 1 >(data_ + offset, sub_shape);
		}
	}

	const T& at(std::initializer_list< size_t > idx) const
	{
		if (idx.size() != N)
			throw std::out_of_range("Wrong number of dimensions");
		const T* cur = data_;
		size_t block = total_count_;
		auto it = idx.begin();
		for (size_t d = 0; d < N; ++d, ++it)
		{
			if (*it >= shape_[d])
				throw std::out_of_range("Index out of range");
			block /= shape_[d];
			cur += (*it) * block;
		}
		return static_cast< const T& >(*cur);
	}

	const T* begin() const { return data_; }
	const T* end() const { return data_ + total_count_; }
	const T* cbegin() const { return data_; }
	const T* cend() const { return data_ + total_count_; }
};

template< typename T, size_t N >
class NDArrayView
{
  private:
	T* data_;
	std::array< size_t, N > shape_;
	size_t total_count_;

	template< typename U, size_t M >
	friend class NDArray;
	template< typename U, size_t M >
	friend class NDArrayView;
	template< typename U, size_t M >
	friend class NDArrayConstView;

  public:
	using value_type = T;
	using size_type = size_t;

	NDArrayView(T* data, const std::array< size_t, N >& shape) :
		data_(data), shape_(shape), total_count_(detail::compute_total_count(shape))
	{
	}

	NDArrayView(const NDArrayView&) = default;
	NDArrayView(NDArrayView&&) = default;
	NDArrayView& operator=(const NDArrayView&) = default;
	NDArrayView& operator=(NDArrayView&&) = default;

	size_type count() const { return shape_[0]; }
	size_type total_count() const { return total_count_; }
	size_type dim() const { return N; }

	auto operator[](size_type i)
	{
		if (i >= shape_[0])
			throw std::out_of_range("Index out of range");
		if constexpr (N == 1)
		{
			return static_cast< T& >(data_[i]);
		}
		else
		{
			std::array< size_t, N - 1 > sub_shape;
			for (size_t j = 1; j < N; ++j)
				sub_shape[j - 1] = shape_[j];
			size_t offset = i * detail::compute_total_count(sub_shape);
			return NDArrayView< T, N - 1 >(data_ + offset, sub_shape);
		}
	}

	auto operator[](size_type i) const
	{
		if (i >= shape_[0])
			throw std::out_of_range("Index out of range");
		if constexpr (N == 1)
		{
			return static_cast< const T& >(data_[i]);
		}
		else
		{
			std::array< size_t, N - 1 > sub_shape;
			for (size_t j = 1; j < N; ++j)
				sub_shape[j - 1] = shape_[j];
			size_t offset = i * detail::compute_total_count(sub_shape);
			return NDArrayConstView< T, N - 1 >(data_ + offset, sub_shape);
		}
	}

	T& at(std::initializer_list< size_t > idx)
	{
		if (idx.size() != N)
			throw std::out_of_range("Wrong number of dimensions");
		T* cur = data_;
		size_t block = total_count_;
		auto it = idx.begin();
		for (size_t d = 0; d < N; ++d, ++it)
		{
			if (*it >= shape_[d])
				throw std::out_of_range("Index out of range");
			block /= shape_[d];
			cur += (*it) * block;
		}
		return static_cast< T& >(*cur);
	}

	const T& at(std::initializer_list< size_t > idx) const
	{
		if (idx.size() != N)
			throw std::out_of_range("Wrong number of dimensions");
		const T* cur = data_;
		size_t block = total_count_;
		auto it = idx.begin();
		for (size_t d = 0; d < N; ++d, ++it)
		{
			if (*it >= shape_[d])
				throw std::out_of_range("Index out of range");
			block /= shape_[d];
			cur += (*it) * block;
		}
		return static_cast< const T& >(*cur);
	}

	T* begin() { return data_; }
	T* end() { return data_ + total_count_; }
	const T* begin() const { return data_; }
	const T* end() const { return data_ + total_count_; }
	const T* cbegin() const { return data_; }
	const T* cend() const { return data_ + total_count_; }
};

template< typename T, size_t N >
class NDArray
{
  private:
	T* data_;
	std::array< size_t, N > shape_;
	size_t total_count_;

	void cleanup()
	{
		if (data_)
		{
			delete[] data_;
			data_ = nullptr;
		}
	}

  public:
	using value_type = T;
	using size_type = size_t;

	NDArray() : data_(nullptr), shape_({ 0 }), total_count_(0) {}

	NDArray(const std::array< size_t, N >& shape, const T& val = T()) :
		shape_(shape), total_count_(detail::compute_total_count(shape))
	{
		data_ = new T[total_count_];
		for (size_t i = 0; i < total_count_; ++i)
			data_[i] = val;
	}

	NDArray(std::initializer_list< std::initializer_list< T > > init)
	{
		static_assert(N == 2, "Only 2D init");
		shape_[0] = init.size();
		if (shape_[0] == 0)
			throw std::invalid_argument("Empty initializer list");
		shape_[1] = init.begin()->size();
		for (const auto& row : init)
			if (row.size() != shape_[1])
				throw std::invalid_argument("Inconsistent row sizes");
		total_count_ = shape_[0] * shape_[1];
		data_ = new T[total_count_];
		size_t i = 0;
		for (const auto& row : init)
			for (const auto& v : row)
				data_[i++] = v;
	}

	NDArray(const NDArray& o) : shape_(o.shape_), total_count_(o.total_count_)
	{
		data_ = new T[total_count_];
		for (size_t i = 0; i < total_count_; ++i)
			data_[i] = o.data_[i];
	}

	NDArray(NDArray&& o) : data_(o.data_), shape_(o.shape_), total_count_(o.total_count_)
	{
		o.data_ = nullptr;
		o.total_count_ = 0;
		o.shape_ = {};
	}

	NDArray& operator=(const NDArray& o)
	{
		if (this != &o)
		{
			cleanup();
			shape_ = o.shape_;
			total_count_ = o.total_count_;
			data_ = new T[total_count_];
			for (size_t i = 0; i < total_count_; ++i)
				data_[i] = o.data_[i];
		}
		return *this;
	}

	NDArray& operator=(NDArray&& o)
	{
		if (this != &o)
		{
			cleanup();
			data_ = o.data_;
			shape_ = o.shape_;
			total_count_ = o.total_count_;
			o.data_ = nullptr;
			o.total_count_ = 0;
			o.shape_ = {};
		}
		return *this;
	}

	~NDArray() { cleanup(); }

	size_type count() const { return shape_[0]; }
	size_type total_count() const { return total_count_; }
	size_type dim() const { return N; }

	auto operator[](size_type i)
	{
		if (i >= shape_[0])
			throw std::out_of_range("Index out of range");
		if constexpr (N == 1)
		{
			return static_cast< T& >(data_[i]);
		}
		else
		{
			std::array< size_t, N - 1 > sub;
			for (size_t j = 1; j < N; ++j)
				sub[j - 1] = shape_[j];
			size_t off = i * detail::compute_total_count(sub);
			return NDArrayView< T, N - 1 >(data_ + off, sub);
		}
	}

	auto operator[](size_type i) const
	{
		if (i >= shape_[0])
			throw std::out_of_range("Index out of range");
		if constexpr (N == 1)
		{
			return static_cast< const T& >(data_[i]);
		}
		else
		{
			std::array< size_t, N - 1 > sub;
			for (size_t j = 1; j < N; ++j)
				sub[j - 1] = shape_[j];
			size_t off = i * detail::compute_total_count(sub);
			return NDArrayConstView< T, N - 1 >(data_ + off, sub);
		}
	}

	T& at(std::initializer_list< size_t > idx)
	{
		if (idx.size() != N)
			throw std::out_of_range("Wrong dims");
		T* cur = data_;
		size_t blk = total_count_;
		auto it = idx.begin();
		for (size_t d = 0; d < N; ++d, ++it)
		{
			if (*it >= shape_[d])
				throw std::out_of_range("Index");
			blk /= shape_[d];
			cur += (*it) * blk;
		}
		return static_cast< T& >(*cur);
	}

	const T& at(std::initializer_list< size_t > idx) const
	{
		if (idx.size() != N)
			throw std::out_of_range("Wrong dims");
		const T* cur = data_;
		size_t blk = total_count_;
		auto it = idx.begin();
		for (size_t d = 0; d < N; ++d, ++it)
		{
			if (*it >= shape_[d])
				throw std::out_of_range("Index");
			blk /= shape_[d];
			cur += (*it) * blk;
		}
		return static_cast< const T& >(*cur);
	}

	bool is_equal(const NDArray& rhs) const
	{
		if (shape_ != rhs.shape_)
			return false;
		for (size_t i = 0; i < total_count_; ++i)
			if (data_[i] != rhs.data_[i])
				return false;
		return true;
	}

	T* begin() { return data_; }
	T* end() { return data_ + total_count_; }
	const T* begin() const { return data_; }
	const T* end() const { return data_ + total_count_; }
	const T* cbegin() const { return data_; }
	const T* cend() const { return data_ + total_count_; }

	template< size_t M >
	NDArrayView< T, M > reshape(const std::array< size_t, M >& new_shape)
	{
		size_t nt = detail::compute_total_count(new_shape);
		if (nt != total_count_)
			throw std::invalid_argument("Total mismatch");
		return NDArrayView< T, M >(data_, new_shape);
	}

	void swap(NDArray& o)
	{
		T* tmp_data = data_;
		data_ = o.data_;
		o.data_ = tmp_data;
		auto tmp_shape = shape_;
		shape_ = o.shape_;
		o.shape_ = tmp_shape;
		auto tmp_count = total_count_;
		total_count_ = o.total_count_;
		o.total_count_ = tmp_count;
	}
};

#endif
