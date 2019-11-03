#pragma once

#include "pre-req.hpp"
#include "vvector.hpp"

template<typename DataType, size_t M, size_t N>
class VMatrix
{
public:
	VMatrix() noexcept
	{}

	VMatrix(const VMatrix& rhs) noexcept
		: _data(rhs._data)
	{}

	VMatrix(VMatrix&& rhs) noexcept
		: _data(std::move(rhs))
	{}

	VMatrix(const std::array<VVector<DataType, M>, N>& data) noexcept
		: _data(data)
	{}

	VMatrix(std::array<VVector<DataType, M>, N>&& data) noexcept
		: _data(std::move(data))
	{}

	VMatrix& operator=(const VMatrix& rhs) noexcept
	{
		_data = rhs._data;
		return *this;
	}

	VMatrix& operator=(VMatrix&& rhs) noexcept
	{
		_data = std::move(rhs._data);
		return *this;
	}

	VMatrix& operator=(const std::array<VVector<DataType, M>, N>& data) noexcept
	{
		_data = data;
		return *this;
	}

	VMatrix& operator=(std::array<VVector<DataType, M>, N>&& data) noexcept
	{
		_data = std::move(data);
		return *this;
	}

public:
	DataType& operator[](size_t m)
	{
		return _data[m];
	}

	const DataType& operator[](size_t m) const
	{
		return _data[m];
	}

private:
	std::array<VVector<DataType, M>, N>	_data;
};

typedef VMatrix<float, 2, 2>	VFMat2;
typedef VMatrix<float, 3, 3>	VFMat3;
typedef VMatrix<float, 4, 4>	VFMat4;
typedef VMatrix<double, 2, 2>	VDMat2;
typedef VMatrix<double, 3, 3>	VDMat3;
typedef VMatrix<double, 4, 4>	VDMat4;
