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
		: _data(std::move(rhs._data))
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
	VVector<DataType, M>& operator[](size_t m)
	{
		return _data[m];
	}

	const VVector<DataType, M>& operator[](size_t m) const
	{
		return _data[m];
	}

	template <size_t VecN>
	void set_column(size_t m, const VVector<DataType, VecN>& data)
	{
		assert(VecN <= N);
		for (auto i = 0; i < data.get_rank(); ++i)
		{
			_data[m][i] = data[i];
		}
	}

private:
	std::array<VVector<DataType, M>, N>	_data;
};

template<typename DataType, size_t M1, size_t N1, size_t M2, size_t N2>
VMatrix<DataType, M2, N1> operator* (
	const VMatrix<DataType, M1, N1>& left,
	const VMatrix<DataType, M2, N2>& right)
{
	assert(M1 == N2);
	VMatrix<DataType, M2, N1> res;
	for (auto j = 0; j < N1; ++j)
	{
		for (auto i = 0; i < M2; ++i)
		{
			res[i][j] = 0.;
			for(auto k = 0; k < M1; ++k)
			{
				res[i][j] += left[k][j] * right[i][k];
			}
		}
	}
	return res;
}

typedef VMatrix<float, 2, 2>	VFMat2;
typedef VMatrix<float, 3, 3>	VFMat3;
typedef VMatrix<float, 4, 4>	VFMat4;
typedef VMatrix<double, 2, 2>	VDMat2;
typedef VMatrix<double, 3, 3>	VDMat3;
typedef VMatrix<double, 4, 4>	VDMat4;

const static VFMat2 VFMat2E(
	{ VFVec2({1.f, 0.f})
	, VFVec2({0.f, 1.f}) });
const static VFMat3 VFMat3E(
	{ VFVec3({1.f, 0.f, 0.f})
	, VFVec3({0.f, 1.f, 0.f})
	, VFVec3({0.f, 0.f, 1.f}) });
const static VFMat4 VFMat4E(
	{ VFVec4({1.f, 0.f, 0.f, 0.f})
	, VFVec4({0.f, 1.f, 0.f, 0.f})
	, VFVec4({0.f, 0.f, 1.f, 0.f})
	, VFVec4({0.f, 0.f, 0.f, 1.f}) });

