#ifndef ECVL_ARITHMETIC_H_
#define ECVL_ARITHMETIC_H_

#include "ecvl/core/datatype_matrix.h"
#include "ecvl/core/image.h"

namespace ecvl {

/** @brief Saturate a value (of any type) to the specified type.

Given an input of any type the saturate_cast function provide an
output return value of the specified type applying saturation. When
the input value in greater than the maximum possible value (max) for
the output type, the max value is returned. When the input value in
lower than the minimum possible value (min) for the output type, the
min value is returned.

@param[in] v Input value (of any type).

@return Input value after cast and saturation.
*/
template<DataType ODT, typename IDT>
typename TypeInfo<ODT>::basetype saturate_cast(IDT v) {
    using basetype = typename TypeInfo<ODT>::basetype;

    if (v > std::numeric_limits<basetype>::max()) {
        return std::numeric_limits<basetype>::max();
    }
    if (v < std::numeric_limits<basetype>::min()) {
        return std::numeric_limits<basetype>::min();
    }

    return static_cast<basetype>(v);
}

/** @brief Saturate a value (of any type) to the specified type.

Given an input of any type the saturate_cast function provide an
output return value of the specified type applying saturation. When
the input value in greater than the maximum possible value (max) for
the output type, the max value is returned. When the input value in
lower than the minimum possible value (min) for the output type, the
min value is returned.

@param[in] v Input value (of any type).

@return Input value after cast and saturation.
*/
template<typename ODT, typename IDT>
ODT saturate_cast(const IDT& v) {

    if (v > std::numeric_limits<ODT>::max()) {
        return std::numeric_limits<ODT>::max();
    }
    if (v < std::numeric_limits<ODT>::min()) {
        return std::numeric_limits<ODT>::min();
    }

    return static_cast<ODT>(v);
}

// In-place Add between Images TODO doc
void Add(Image& src1_dst, const Image& src2);
// Template implementation for in-place Add between Images
// TODO doc
template <DataType a, DataType b>
struct StructAdd {
    static void actual_function(Image& src1_dst, const Image& src2) {
        using dsttype = typename TypeInfo<a>::basetype;

        // TODO check before performing Add

        View<a> vsrc1_dst(src1_dst);
        ConstView<b> vsrc2(src2);
        auto is1 = vsrc1_dst.Begin(), es1 = vsrc1_dst.End();
        auto is2 = vsrc2.Begin();
        for (; is1 != es1; ++is1, ++is2) {
            *is1 = static_cast<dsttype>(*is1 + *is2);
        }
    }
};

// In-place Sub between Images TODO doc
void Sub(Image& src1_dst, const Image& src2);
// Template implementation for in-place Sub between Images
// TODO doc
template <DataType a, DataType b>
struct StructSub {
    static void actual_function(Image& src1_dst, const Image& src2) {
        using dsttype = typename TypeInfo<a>::basetype;

        // TODO check before performing Add

        View<a> vsrc1_dst(src1_dst);
        ConstView<b> vsrc2(src2);
        auto is1 = vsrc1_dst.Begin(), es1 = vsrc1_dst.End();
        auto is2 = vsrc2.Begin();
        for (; is1 != es1; ++is1, ++is2) {
            *is1 = static_cast<dsttype>(*is1 - *is2);
        }
    }
};

// In-place Mul between images TODO doc
void Mul(Image& src1_dst, const Image& src2);
/** @brief Template struct for in-place multiplication between images
of any ecvl::DataType. */
// TODO doc
template <DataType a, DataType b>
struct StructMul {
    static void actual_function(Image& src1_dst, const Image& src2) {
        using dsttype = typename TypeInfo<a>::basetype;

        // TODO check before performing Add

        View<a> vsrc1_dst(src1_dst);
        ConstView<b> vsrc2(src2);
        auto is1 = vsrc1_dst.Begin(), es1 = vsrc1_dst.End();
        auto is2 = vsrc2.Begin();
        for (; is1 != es1; ++is1, ++is2) {
            *is1 = static_cast<dsttype>(*is1 * *is2);
        }
    }
};

// In-place Div between Images TODO doc
void Div(Image& src1_dst, const Image& src2);
// Template implementation for in-place Div between Images
// TODO doc
template <DataType a, DataType b>
struct StructDiv {
    static void actual_function(Image& src1_dst, const Image& src2) {
        using dsttype = typename TypeInfo<a>::basetype;

        // TODO check before performing Add

        View<a> vsrc1_dst(src1_dst);
        ConstView<b> vsrc2(src2);
        auto is1 = vsrc1_dst.Begin(), es1 = vsrc1_dst.End();
        auto is2 = vsrc2.Begin();
        for (; is1 != es1; ++is1, ++is2) {
            *is1 = static_cast<dsttype>(*is1 / *is2);
        }
    }
};

// Struct template specialization of the in-place multiplication between Image and scalar. 
template<DataType DT, typename T>
struct StructScalarMul{
    static Image& ActualFunction(Image& img, T d, bool saturate)
    {
        View<DT> v(img);
        auto i = v.Begin(), e = v.End();
        for (; i != e; ++i) {
            auto& p = *i;
            if (saturate) {
                p = saturate_cast<DT>(p * d);
            }
            else {
                p = static_cast<typename TypeInfo<DT>::basetype>(p * d);
            }
        }
        return img;
    }
};

/** @brief In-place multiplication between an Image and a scalar value,
without type promotion. @anchor Mul

The Mul() function multiplies an input image by a scalar value and stores
the result in the same image. The type of the image will not change. By
default a saturation will be applied. If it is not the desired behavior
change the "saturate" parameter to false.

@param[in,out] img Image to be multiplied (in-place) by a scalar value.
@param[in] d Scalar value to use for the multiplication.
@param[in] saturation Whether to apply saturation or not. Default is true.

@return Reference to the Image containing the result of the multiplication.
*/
template<typename T>
Image& Mul(Image& img, T value, bool saturate = true) {                               
    static constexpr Table1D<StructScalarMul, T> table;          
    return table(img.elemtype_)(img, value, saturate);
}

/** @overload [Image& Mul(Image& img, T value, bool saturate = true)] */
template<typename T>
Image& Mul(T value, Image& img, bool saturate = true) {                               
    return Mul(img, value, saturate);
}

// Struct template specialization of the in-place sum between Image and scalar. 
template<DataType DT, typename T>
struct StructScalarAdd{
    static Image& ActualFunction(Image& img, T value, bool saturate)
    {
        View<DT> v(img);
        auto i = v.Begin(), e = v.End();
        for (; i != e; ++i) {
            auto& p = *i;
            if (saturate) {
                p = saturate_cast<DT>(p + value);
            }
            else {
                p = static_cast<typename TypeInfo<DT>::basetype>(p + value);
            }
        }
        return img;
    }
};

/** @brief In-place sum between an Image and a scalar value, without type
promotion. @anchor Sum

The Sum() function sums a scalar value to the input Image and stores
the result in the same image. The type of the image will not change. By
default a saturation will be applied. If it is not the desired behavior
change the "saturate" parameter to false.

@param[in,out] img Image to be summed (in-place) by a scalar value.
@param[in] d Scalar value to use for the sum.
@param[in] saturation Whether to apply saturation or not. Default is true.

@return Reference to the image containing the result of the sum.
*/
template <typename T>
Image& Add(Image& img, T value, bool saturate = true) {
    static constexpr Table1D<StructScalarAdd, T> table;          
    return table(img.elemtype_)(img, value, saturate);
}

/** @overload [Image& Sum(Image& img, T value, bool saturate = true)] */
template <typename T>
Image& Add(T value, Image& img, bool saturate = true) {
    return Add(img, value, saturate);
}

} // namespace ecvl

#endif // !ECVL_ARITHMETIC_H_

