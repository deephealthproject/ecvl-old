#ifndef DATATYPE_MATRIX_H_
#define DATATYPE_MATRIX_H_

#include "datatype.h"

namespace ecvl {

template<template<DataType src, DataType dst>class _StructFun>
struct Table2D {

    using fun_type = decltype(&_StructFun<static_cast<DataType>(0), static_cast<DataType>(0)>::actual_function);

    template<int i>
    struct integer {};

    template <int i>
    constexpr void FillData(integer<i>) {
        constexpr auto arr = DataTypeArray();
        constexpr int src = i / DataTypeSize();
        constexpr int dst = i % DataTypeSize();
        data[i] = _StructFun<arr[src], arr[dst]>::actual_function;
        FillData(integer<i + 1>());
    }

    constexpr void FillData(integer< DataTypeSize()* DataTypeSize() >) {}

    constexpr Table2D() : data() {
        FillData<0>(integer<0>());
    }

    inline fun_type operator()(DataType src, DataType dst) const {
        int row = static_cast<int>(src);
        int col = static_cast<int>(dst);
        return data[row * DataTypeSize() + col];
    }

    fun_type data[DataTypeSize() * DataTypeSize()];
};

} // namespace ecvl

#endif // !DATATYPE_MATRIX_H_

