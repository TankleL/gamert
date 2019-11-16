#pragma once

#include "pre-req.hpp"

template<typename DataType, size_t rank>
class VVector
{
public:
	VVector() noexcept
	{
		memset(_data.data(), 0, _data.size() * sizeof(DataType));
	}

	VVector(const std::array<DataType, rank>& data) noexcept
		: _data(data)
	{}

	VVector(std::array<DataType, rank>&& data) noexcept
		: _data(std::move(data))
	{}

	VVector(const VVector& rhs) noexcept
		: _data(rhs._data)
	{}

	VVector(VVector&& rhs) noexcept
		: _data(std::move(rhs._data))
	{}
	
	VVector& operator=(const std::array<DataType, rank>& data) noexcept
	{
		_data = data;
		return *this;
	}

	VVector& operator=(std::array<DataType, rank>&& data) noexcept
	{
		_data = std::move(data);
		return *this;
	}

	VVector& operator=(const VVector& rhs) noexcept
	{
		_data = rhs._data;
		return *this;
	}

	VVector& operator=(VVector&& rhs) noexcept
	{
		_data = std::move(rhs._data);
		return *this;
	}

public:
	DataType& operator[](size_t index)
	{
		return _data[index];
	}

	DataType operator[](size_t index) const
	{
		return _data[index];
	}

private:
	alignas(16) std::array<DataType, rank> _data;
};


typedef VVector<float, 2>	VFVec2;
typedef VVector<float, 3>	VFVec3;
typedef VVector<float, 4>	VFVec4;
typedef VVector<double, 2>	VDvec2;
typedef VVector<double, 3>	VDvec3;
typedef VVector<double, 4>	VDvec4;

