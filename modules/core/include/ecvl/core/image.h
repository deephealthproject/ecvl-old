#ifndef ECVL_IMAGE_H_
#define ECVL_IMAGE_H_

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <opencv2/core.hpp>

#include "datatype.h"
#include "memorymanager.h"
#include "iterators.h"

namespace ecvl {

class MetaData {
public:
    virtual bool Query(const std::string& name, std::string& value) const = 0;
    virtual ~MetaData() {}
};

/** @anchor ColorType

   Enum class representing the ECVL supported color spaces.

*/
enum class ColorType {
    none,
    GRAY,
    RGB,
    BGR,
    HSV,
    YCbCr,
};

/** @brief Image class

*/
class Image {
public:
    DataType            elemtype_;  /**< @brief Type of Image pixels, must be one of the
                                         values available in @ref DataType.        */
    uint8_t             elemsize_;  /**< @brief Size (in bytes) of Image pixels.          */
    std::vector<int>    dims_;      /**< @brief Vector of Image dimensions. Each dimension
                                         is given in pixels/voxels. */
    std::vector<int>    strides_;   /**< @brief Vector of Image strides. 
                                    
                                         Strides represent
                                         the number of bytes the pointer on data
                                         has to move to reach the next pixel/voxel
                                         on the correspondent size. */
    std::string         channels_;  /**< @brief String which describes how Image planes
                                         are organized. 
                                         
                                         A single character provides
                                         the information related to the corresponding
                                         channel. The possible values are:
                                            - 'x': horizontal spatial dimension
                                            - 'y': vertical spatial dimension
                                            - 'z': depth spatial dimension
                                            - 'c': color dimension
                                            - 't': temporal dimension
                                            - 'o': any other dimension
                                         For example, "xyc" describes a 2-dimensional
                                         Image structured in color planes. This could
                                         be for example a ColorType::GRAY Image with
                                         dims_[2] = 1 or a ColorType::RGB Image with
                                         dims_[2] = 3 an so on. The ColorType constrains
                                         the value of the dimension corresponding to
                                         the color channel.
                                         Another example is "cxy" with dims_[0] = 3 and
                                         ColorType::BGR. In this case the color dimension
                                         is the one which changes faster as it is done
                                         in other libraries such as OpenCV. */
    ColorType           colortype_; /**< @brief Image ColorType. 
                                    
                                         If this is different from ColorType::none
                                         the channels_ string must contain a 'c' and the
                                         corresponding dimension must have the appropriate
                                         value. See @ref ColorType for the possible values. */
    uint8_t*            data_;      /**< @brief Pointer to Image data. 
                                    
                                         If the Image is not the owner
                                         of data, for example when using Image views, this 
                                         attribute will point to the data of another Image.
                                         The possession or not of the data depends on the 
                                         MemoryManager. */
    size_t              datasize_;  /**< @brief Size of Image data in bytes. */
    bool                contiguous_;/**< @brief Whether the image is stored contiguously or not in memory. */

    MetaData* meta_;                /**< @brief Pointer to Image MetaData. */
    MemoryManager* mem_;            /**< @brief Pointer to the MemoryManager employed by the Image. 
                                    
                                         It can be DefaultMemoryManager or ShallowMemoryManager. The 
                                         former is responsible for allocating and deallocating data,
                                         when using the DefaultMemoryManager the Image is the owner
                                         of data. When ShallowMemoryManager is employed the Image
                                         does not own data and operations on memory are not allowed
                                         or does not produce any effect.*/

    /** @brief Generic non-const Begin Iterator.
    
    This function gives you a non-const generic Begin Iterator that can be used both for contiguous and 
    non-contiguous non-const Images. It is useful to iterate over a non-const Image. If the Image is contiguous 
    prefer the use of ContiguousIterato which in most cases improve the performance.
    */
    template<typename T>
    Iterator<T> Begin() { return Iterator<T>(*this); }  
    
    /** @brief Generic non-const End Iterator.

    This function gives you a non-const generic End Iterator that can be used both for contiguous and
    non-contiguous non-const Images. It is useful to iterate over over a non-const Image.
    */
    template<typename T>
    Iterator<T> End() { return Iterator<T>(*this, dims_); }

    /** @brief Generic const Begin Iterator.

    This function gives you a const generic Begin Iterator that can be used both for contiguous and
    non-contiguous const Images. It is useful to iterate over a const Image. If the Image is contiguous 
    prefer the use of ConstContiguousIterator which in most cases improve the performance.
    */
    template<typename T>
    ConstIterator<T> Begin() const { return ConstIterator<T>(*this); }  
    
    /** @brief Generic const End Iterator.

    This function gives you a const generic End Iterator that can be used both for contiguous and
    non-contiguous const Images. It is useful to iterate over a const Image.
    */
    template<typename T>
    ConstIterator<T> End() const { return ConstIterator<T>(*this, dims_); }  

    /** @brief Contiguous non-const Begin Iterator.

    This function gives you a contiguous non-const Begin Iterator that can be used only for contiguous
    Images. If the Image is contiguous it is preferable to the non-contiguous iterator since it has usually
    better performance.
    */
    template<typename T>
    ContiguousIterator<T> ContiguousBegin() { return ContiguousIterator<T>(*this); }
    
    /** @brief Contiguous non-const End Iterator.

    This function gives you a contiguous non-const End Iterator that can be used only for contiguous
    Images.
    */
    template<typename T>
    ContiguousIterator<T> ContiguousEnd() { return ContiguousIterator<T>(*this, dims_); }

    /** @brief Contiguous const Begin Iterator.

    This function gives you a contiguous const Begin Iterator that can be used only for contiguous Images. 
    If the Image is contiguous it is preferable to the non-contiguous iterator since it has usually better 
    performance.
    */
    template<typename T>
    ConstContiguousIterator<T> ContiguousBegin() const { return ConstContiguousIterator<T>(*this); }
    
    /** @brief Contiguous const End Iterator.

    This function gives you a contiguous const End Iterator that can be used only for contiguous Images.
    */
    template<typename T>
    ConstContiguousIterator<T> ContiguousEnd() const { return ConstContiguousIterator<T>(*this, dims_); }

    /** @brief Default constructor

        The default constructor creates an empty image without any data.
    */
    Image() :
        elemtype_{ DataType::none },
        elemsize_{ DataTypeSize(elemtype_) },
        dims_{},
        strides_{},
        channels_{},
        colortype_{ ColorType::none },
        data_{ nullptr },
        datasize_{ 0 },
        contiguous_{ true },
        meta_{ nullptr },
        mem_{ nullptr }
    {
    }

    /** @brief Initializing constructor

        The initializing constructor creates a proper image and allocates the data.
    */
    Image(const std::vector<int>& dims, DataType elemtype, std::string channels, ColorType colortype) :
        elemtype_{ elemtype },
        elemsize_{ DataTypeSize(elemtype_) },
        dims_{ dims },
        strides_{},
        channels_{ move(channels) },
        colortype_{ colortype },
        data_{ nullptr },
        datasize_{ 0 },
        contiguous_{ true },
        meta_{ nullptr },
        mem_{ DefaultMemoryManager::GetInstance() }
    {
        // Compute strides
        strides_ = { elemsize_ };
        int dsize = dims_.size();
        for (int i = 0; i < dsize - 1; ++i) {
            strides_.push_back(strides_[i] * dims_[i]);
        }
        // Compute datasize
        datasize_ = elemsize_;
        datasize_ = std::accumulate(begin(dims_), end(dims_), datasize_, std::multiplies<size_t>());
        data_ = mem_->Allocate(datasize_);
    }

    /** @brief Copy constructor.
    
    The copy constructor creates an new Image copying (Deep Copy) the input one.
    The new Image will be contiguous regardless of the contiguity of the to be 
    copied Image.
    */
    Image(const Image& img) :
        elemtype_{ img.elemtype_ },
        elemsize_{ img.elemsize_ },
        dims_{ img.dims_ },
        strides_{ img.strides_ },
        channels_{ img.channels_ },
        colortype_{ img.colortype_ },
        data_{},
        datasize_{ img.datasize_ },
        contiguous_{ img.contiguous_ },
        meta_{ img.meta_ },
        mem_{ img.mem_ }
    {
        if (mem_ == ShallowMemoryManager::GetInstance()) {
            // When copying from non owning memory we become owners of the original data.
            mem_ = DefaultMemoryManager::GetInstance();
        }
        if (mem_ == DefaultMemoryManager::GetInstance()) {
            if (contiguous_) {
                data_ = mem_->AllocateAndCopy(datasize_, img.data_);
            }
            else {
                // When copying a non contiguous image, we make it contiguous
                contiguous_ = true;
                // Compute strides
                strides_ = { elemsize_ };
                int dsize = dims_.size();
                for (int i = 0; i < dsize - 1; ++i) {
                    strides_.push_back(strides_[i] * dims_[i]);
                }
                // Compute datasize
                datasize_ = elemsize_;
                datasize_ = std::accumulate(begin(dims_), end(dims_), datasize_, std::multiplies<size_t>());
                data_ = mem_->Allocate(datasize_);
                // Copy with iterators
                // TODO: optimize so that we can memcpy one block at a time on the first dimension
                // This will require Iterators to increment more than one
                auto p = data_;
                auto i = Begin<uint8_t>(), e = End<uint8_t>();
                for (; i != e; ++i) {
                    memcpy(p++, i.ptr_, elemsize_);
                }
            }
        }
    }

    /** @brief Move constructor

        Move constructor
    */
    Image(Image&& img) :
        elemtype_{ img.elemtype_ },
        elemsize_{ img.elemsize_ },
        dims_{ move(img.dims_) },
        strides_{ move(img.strides_) },
        channels_{ move(img.channels_) },
        colortype_{ img.colortype_ },
        data_{ img.data_ },
        datasize_{ img.datasize_ },
        contiguous_{ img.contiguous_ },
        meta_{ img.meta_ },
        mem_{ img.mem_ }
    {
        img.data_ = nullptr;
    }

    friend void swap(Image& lhs, Image& rhs) {
        using std::swap;
        swap(lhs.elemtype_, rhs.elemtype_);
        swap(lhs.elemsize_, rhs.elemsize_);
        swap(lhs.dims_, rhs.dims_);
        swap(lhs.strides_, rhs.strides_);
        swap(lhs.channels_, rhs.channels_);
        swap(lhs.colortype_, rhs.colortype_);
        swap(lhs.data_, rhs.data_);
        swap(lhs.datasize_, rhs.datasize_);
        swap(lhs.contiguous_, rhs.contiguous_);
        swap(lhs.meta_, rhs.meta_);
        swap(lhs.mem_, rhs.mem_);
    }

    Image& operator=(Image rhs) {
        swap(*this, rhs);
        return *this;
    }

    /** @brief Allocates new contiguous data if needed.
    
    The Create method allocates Image data as specified by the input parameters.
    The procedures tries to avoid the allocation of new memory when possible.
    The resulting image will be contiguous in any case.
    Calling this method on an Image that does not own data will always cause 
    a new allocation, and the Image will become the owner of the data.

    @param[in] dims New Image dimensions.
    @param[in] elemtype New Image DataType.
    @param[in] channels New Image channels.
    @params[in] colortype New Image colortype.
    */
    void Create(const std::vector<int>& dims, DataType elemtype, std::string channels, ColorType colortype);

    /** @brief Destructor

    If the Image is the owner of data they will be deallocate. Otherwise nothing will happen.
    */
    ~Image() {
        if (mem_)
            mem_->Deallocate(data_);
    }

    /** @brief To check whether the Image contains or not data, regardless the owning status. */
    bool IsEmpty() const { return data_ == nullptr; }

    /** @brief To check whether the Image is owner of the data. */
    bool IsOwner() const { return mem_ != ShallowMemoryManager::GetInstance(); }

    /** @brief Returns a non-const pointer to data at given coordinates. */
    uint8_t* Ptr(const std::vector<int>& coords) {
        assert(coords.size() == strides_.size());
        return std::inner_product(begin(coords), end(coords), begin(strides_), data_);
    }
    /** @brief Returns a const pointer to data at given coordinates. */
    const uint8_t* Ptr(const std::vector<int>& coords) const {
        assert(coords.size() == strides_.size());
        return std::inner_product(begin(coords), end(coords), begin(strides_), data_);
    }    

};

#include "iterators_impl.inc.h"
template <DataType DT>
class View : public Image {
public:
    using basetype = typename TypeInfo<DT>::basetype;

    View(Image& img)
    {
        auto culo = DT;
        if (DT != img.elemtype_)
            throw std::runtime_error("View type is different from Image type");
        elemtype_ = img.elemtype_;
        elemsize_ = img.elemsize_;
        dims_ = img.dims_;
        strides_ = img.strides_;
        channels_ = img.channels_;
        colortype_ = img.colortype_;
        data_ = img.data_;
        datasize_ = img.datasize_;
        contiguous_ = img.contiguous_;
        meta_ = img.meta_;
        mem_ = ShallowMemoryManager::GetInstance();
    }

    View(Image& img, const std::vector<int>& start, const std::vector<int>& size) : View(img)
    {
        dims_.clear();
        int ssize = size.size();
        for (int i = 0; i < ssize; ++i) {
            if (start[i] < 0 || start[i] >= img.dims_[i])
                throw std::runtime_error("Start of crop outside image limits");
            dims_.push_back(img.dims_[i] - start[i]);
            if (size[i] > dims_[i]) {
                throw std::runtime_error("Crop outside image limits");
            }
            if (size[i] >= 0) {
                dims_[i] = size[i];
            }
        }

        // Check if image has a color dimension
        auto cpos = channels_.find('c');
        if (cpos != std::string::npos) {
            // If we are cropping the color channel, we fix the color information
            if (dims_[cpos] != img.dims_[cpos]) {
                if (dims_[cpos] == 1) {
                    colortype_ = ColorType::GRAY;
                }
                else {
                    channels_[cpos] = 'o';
                    colortype_ = ColorType::none;
                }
            }
        }

        data_ = img.Ptr(start);
        datasize_ = 0; // This is set to zero, because when the View is not contiguous, it's useless to relay on this information
        contiguous_ = false;
    }

    basetype& operator()(const std::vector<int>& coords) {
        return *reinterpret_cast<basetype*>(Ptr(coords));
    }

    Iterator<basetype> Begin() { return Iterator<basetype>(*this); }
    Iterator<basetype> End() { return Iterator<basetype>(*this, dims_); }
};

template <DataType DT>
class ConstView : public Image {
public:
    using basetype = typename TypeInfo<DT>::basetype;

    ConstView(const Image& img) {
        elemtype_ = img.elemtype_;
        elemsize_ = img.elemsize_;
        dims_ = img.dims_;
        strides_ = img.strides_;
        channels_ = img.channels_;
        colortype_ = img.colortype_;
        data_ = img.data_;
        datasize_ = img.datasize_;
        contiguous_ = img.contiguous_;
        meta_ = img.meta_;
        mem_ = ShallowMemoryManager::GetInstance();
    }

    const basetype& operator()(const std::vector<int>& coords) {
        return *reinterpret_cast<const basetype*>(Ptr(coords));
    }

    ConstIterator<basetype> Begin() { return ConstIterator<basetype>(*this); }
    ConstIterator<basetype> End() { return ConstIterator<basetype>(*this, dims_); }
};

template <DataType DT>
class ContiguousView : public Image {
public:
    using basetype = typename TypeInfo<DT>::basetype;

    ContiguousView(Image& img) {
        elemtype_ = img.elemtype_;
        elemsize_ = img.elemsize_;
        dims_ = img.dims_;
        strides_ = img.strides_;
        channels_ = img.channels_;
        colortype_ = img.colortype_;
        data_ = img.data_;
        datasize_ = img.datasize_;
        contiguous_ = img.contiguous_;
        meta_ = img.meta_;
        mem_ = ShallowMemoryManager::GetInstance();
    }

    basetype& operator()(const std::vector<int>& coords) {
        return *reinterpret_cast<basetype*>(Ptr(coords));
    }

    ContiguousIterator<basetype> Begin() { return ContiguousIterator<basetype>(*this); }
    ContiguousIterator<basetype> End() { return ContiguousIterator<basetype>(*this, dims_); }
};

template <DataType DT>
class ConstContiguousView : public Image {
public:
    using basetype = typename TypeInfo<DT>::basetype;

    ConstContiguousView(Image& img) {
        elemtype_ = img.elemtype_;
        elemsize_ = img.elemsize_;
        dims_ = img.dims_;
        strides_ = img.strides_;
        channels_ = img.channels_;
        colortype_ = img.colortype_;
        data_ = img.data_;
        datasize_ = img.datasize_;
        contiguous_ = img.contiguous_;
        meta_ = img.meta_;
        mem_ = ShallowMemoryManager::GetInstance();
    }

    const basetype& operator()(const std::vector<int>& coords) {
        return *reinterpret_cast<const basetype*>(Ptr(coords));
    }

    ConstContiguousIterator<basetype> Begin() { return ConstContiguousIterator<basetype>(*this); }
    ConstContiguousIterator<basetype> End() { return ConstContiguousIterator<basetype>(*this, dims_); }
};

template <DataType DT>
class ContiguousViewXYC : public Image {
public:
    using basetype = typename TypeInfo<DT>::basetype;

    ContiguousViewXYC(Image& img) {
        if (img.channels_ != "xyc")
            throw std::runtime_error("ContiguousView2D can be built only from \"xyc\" images");
        if (!img.contiguous_)
            throw std::runtime_error("ContiguousView2D can be built only from images with contiguous data");
        elemtype_ = img.elemtype_;
        elemsize_ = img.elemsize_;
        dims_ = img.dims_;
        strides_ = img.strides_;
        channels_ = img.channels_;
        colortype_ = img.colortype_;
        data_ = img.data_;
        datasize_ = img.datasize_;
        contiguous_ = img.contiguous_;
        meta_ = img.meta_;
        mem_ = ShallowMemoryManager::GetInstance();
    }

    int width() const { return dims_[0]; }
    int height() const { return dims_[1]; }
    int channels() const { return dims_[2]; }

    basetype& operator()(int x, int y, int c) {
        return *reinterpret_cast<basetype*>(data_ + c * strides_[2] + y * strides_[1] + x * strides_[0]);
    }

    ContiguousIterator<basetype> Begin() { return ContiguousIterator<basetype>(*this); }
    ContiguousIterator<basetype> End() { return ContiguousIterator<basetype>(*this, dims_); }
};


/** @brief Changes the order of the Image dimensions.

The RearrangeChannels procedure changes the order of the input Image dimensions saving
the result into the output Image. The new order of dimensions can be specified as a
string through the "channels" parameter. Input and output Images can be the same. The
number of channels of the input Image must be the same of required channels.

@param[in] src Input Image on which to rearrange dimensions.
@param[out] dst The output rearranged Image. Can be the src Image.
@param[in] channels Desired order of Image channels.

*/
void RearrangeChannels(const Image& src, Image& dst, const std::string& channels);

// Template copy image
template<DataType SDT, DataType DDT>
struct StructCopyImage {
    static void ActualFunction(const Image& src, Image& dst)
    {
        using dsttype = typename TypeInfo<DDT>::basetype;

        ConstView<SDT> vsrc(src);
        View<DDT> vdst(dst);
        auto is = vsrc.Begin(), es = vsrc.End();
        auto id = vdst.Begin();
        for (; is != es; ++is, ++id) {
            *id = static_cast<dsttype>(*is);
        }
    }
};

/** @brief Copies the source Image into the destination Image.

The CopyImage() procedure takes an Image and copies its data into the destination Image.
Source and destination cannot be the same Image. Source cannot be a Image with DataType::none.
The optional new_type parameter can 
be used to change the DataType of the destination Image. This function is mainly designed to 
change the DataType of an Image, copying its data into a new Image or to copy an Image into a
View as a patch. So if you just want to copy an Image as it is, use the copy constructor or = 
instead. Anyway, the procedure will handle all the possible situations that may happen trying 
to avoid unnecessary allocations.
When the DataType is not specified the function will have the following behaviors:
    - if the destination Image is empty the source will be directly copied into the destination. 
    - if source and destination have different size in memory or different channels and the destination 
        is the owner of data, the procedure will overwrite the destination Image creating a new Image 
        (channels and dimensions will be the same of the source Image, pixels type (DataType) will be the 
        same of the destination Image if they are not none or the same of the source otherwise).
    - if source and destination have different size in memory or different channels and the destination is not
        the owner of data, the procedure will throw an exception.
    - if source and destination have different color types and the destination is the owner of 
        data, the procedure produces a destination Image with the same color type of the source.
    - if source and destination have different color types and the destination is not the owner 
        of data, the procedure will throw an exception.
When the DataType is specified the function will have the same behavior, but the destination Image will have 
the specified DataType.

@param[in] src Source Image to be copied into destination Image.
@param[out] dst Destination Image that will hold a copy of the source Image. Cannot be the source Image.
@param[in] new_type Desired type for the destination Image after the copy. If none (default) the destination 
            Image will preserve its type if it is not empty, otherwise it will have the same type of the 
            source Image. 
*/
void CopyImage(const Image& src, Image& dst, DataType new_type = DataType::none);

} // namespace ecvl

#endif // !ECVL_IMAGE_H_


