/*
* ECVL - European Computer Vision Library
* Version: 0.1
* copyright (c) 2020, UniversitÓ degli Studi di Modena e Reggio Emilia (UNIMORE), AImageLab
* Authors: 
*    Costantino Grana (costantino.grana@unimore.it)
*    Federico Bolelli (federico.bolelli@unimore.it)
*    Michele Cancilla (michele.cancilla@unimore.it)
*    Laura Canalini (laura.canalini@unimore.it)
*    Stefano Allegretti (stefano.allegretti@unimore.it)
* All rights reserved.
*/

#ifndef ECVL_EDDL_H_
#define ECVL_EDDL_H_

#include <eddl/apis/eddl.h>
#include <eddl/apis/eddlT.h>
#include "ecvl/core/image.h"
#include "ecvl/dataset_parser.h"
#include <filesystem>

namespace ecvl {
/** @brief DeepHealth Deep Learning Dataset.

This class extends the DeepHealth Dataset with Deep Learning specific members.

@anchor DLDataset
*/
class DLDataset : public Dataset {
public:
    int batch_size_; /**< @brief Size of each dataset mini batch. */
    int n_channels_; /**< @brief Number of color channels of the images. */
    int current_split_ = 0; /**< @brief Current split from which images are loaded. */
    std::vector<int> resize_dims_; /**< @brief Dimensions (HxW) to which Dataset images must be resized. */
    std::array<int, 3> current_batch_ = { 0,0,0 }; /**< @brief Number of batches already loaded for each split. */
    ColorType ctype_; /**< @brief ecvl::ColorType of the Dataset images. */
    ColorType ctype_gt_; /**< @brief ecvl::ColorType of the Dataset ground truth images. */

    /**
    @param[in] filename Path to the Dataset file.
    @param[in] batch_size Size of each dataset mini batch.
    @param[in] resize_dims Dimensions to which Dataset images must be resized.
    @param[in] ctype ecvl::ColorType of the Dataset images.
    @param[in] ctype_gt ecvl::ColorType of the Dataset ground truth images.
    */
    DLDataset(const std::filesystem::path& filename,
        const int batch_size,
        const std::vector<int>& resize_dims,
        ColorType ctype = ColorType::BGR,
        ColorType ctype_gt = ColorType::GRAY, bool verify = false) :

        Dataset{ filename, verify },
        batch_size_{ batch_size },
        resize_dims_{ resize_dims },
        n_channels_{ this->samples_[0].LoadImage(ctype).Channels() },
        ctype_{ ctype },
        ctype_gt_{ ctype_gt }
    {}

    /** @brief Returns the current Split.
    @return Current Split in use.
    */
    std::vector<int>& GetSplit();

    /** @brief Reset the batch counter of the current Split. */
    void ResetCurrentBatch();

    /** @brief Reset the batch counter of each Split. */
    void ResetAllBatches();

    /** @brief Set the current Split.
    @param[in] split_str std::string representing the Split to set.
        split_str can assume one of the following values: "training", "validation", and "test".
    */
    void SetSplit(const std::string& split_str);

    /** @brief Load a batch into _images_ and _labels_ `tensor`.
    @param[out] images `tensor` which stores the batch of images.
    @param[out] labels `tensor` which stores the batch of labels.
    */
    void LoadBatch(tensor& images, tensor& labels);
};

/** @brief Convert an EDDL Tensor into an ECVL Image.

Tensor dimensions must be \f$C\f$ x \f$Y\f$ x \f$X\f$ or \f$Z\f$ x \f$C\f$ x \f$Y\f$ x \f$X\f$, where: \n
\f$Z\f$ = depth \n
\f$C\f$ = color channels \n
\f$Y\f$ = height \n
\f$X\f$ = width

@param[in] t Input EDDL Tensor.
@param[out] img Output ECVL Image.
@param[in] c_type ecvl::ColorType of input data (optional). \n
If c_type is ColorType::none (default), it is assumed that: \n
If the input has 4 channels, the color type is assumed to be ColorType::RGBA. \n
If the input has 3 channels, the color type is assumed to be ColorType::BGR. \n
If the input has 1 channels, the color type is assumed to be ColorType::GRAY. \n
In any other case, the color type is assumed to be ColorType::none.

*/
void TensorToImage(tensor& t, Image& img, ColorType c_type = ColorType::none);

/** @brief Convert an EDDL Tensor into an ECVL View.

Tensor dimensions must be \f$C\f$ x \f$Y\f$ x \f$X\f$ or \f$Z\f$ x \f$C\f$ x \f$Y\f$ x \f$X\f$, where: \n
\f$Z\f$ = depth \n
\f$C\f$ = color channels \n
\f$Y\f$ = height \n
\f$X\f$ = width

@param[in] t Input EDDL Tensor.
@param[out] v Output ECVL View.
@param[in] c_type ecvl::ColorType of input data (optional). \n
If c_type is ColorType::none (default), it is assumed that: \n
If the input has 4 channels, the color type is assumed to be ColorType::RGBA. \n
If the input has 3 channels, the color type is assumed to be ColorType::BGR. \n
If the input has 1 channels, the color type is assumed to be ColorType::GRAY. \n
In any other case, the color type is assumed to be ColorType::none.

*/
void TensorToView(tensor& t, View<DataType::float32>& v, ColorType c_type = ColorType::none);

/** @brief Insert an ECVL Image into an EDDL Tensor.

This function is useful to insert into an EDDL Tensor more than one image, specifying how many images are already stored in the Tensor.

@param[in] img Input ECVL Image.
@param[out] t Output EDDL Tensor. It must be created with the right dimensions before calling this function.
@param[in] offset How many images are already stored in the Tensor.

*/
void ImageToTensor(Image& img, tensor& t, const int& offset);

/** @brief Convert an ECVL Image into an EDDL Tensor.

Image must have 3 or 4 dimensions. \n
If the Image has 3 dimensions, the output Tensor will be created with shape \f$C\f$ x \f$Y\f$ x \f$X\f$. \n
If the Image has 4 dimensions, the output Tensor will be created with shape \f$Z\f$ x \f$C\f$ x \f$Y\f$ x \f$X\f$.

@param[in] img Input ECVL Image.
@param[out] t Output EDDL Tensor. It is created inside the function.

*/
void ImageToTensor(const Image& img, tensor& t);

/** @brief Load the training split of a Dataset (images and labels) into EDDL tensors.

@param[in] dataset Dataset object listing all the samples.
@param[in] size Dimensions (width and height) at which all the images have to be resized.
@param[out] images Tensor which contains all the images.
@param[out] labels Tensor which contains all the labels.
@param[in] ctype ColorType which specifies color space for images in the dataset.

*/
void TrainingToTensor(const Dataset& dataset, const std::vector<int>& size, tensor& images, tensor& labels, ColorType ctype = ColorType::BGR);

/** @brief Load the validation split of a Dataset (images and labels) into EDDL tensors.

@param[in] dataset Dataset object listing all the samples.
@param[in] size Dimensions (width and height) at which all the images have to be resized.
@param[out] images Tensor which contains all the images.
@param[out] labels Tensor which contains all the labels.
@param[in] ctype ColorType which specifies color space for images in the dataset.

*/
void ValidationToTensor(const Dataset& dataset, const std::vector<int>& size, tensor& images, tensor& labels, ColorType ctype = ColorType::BGR);

/** @brief Load the test split of a Dataset (images and labels) into EDDL tensors.

@param[in] dataset Dataset object listing all the samples.
@param[in] size Dimensions (width and height) at which all the images have to be resized.
@param[out] images Tensor which contains all the images.
@param[out] labels Tensor which contains all the labels.
@param[in] ctype ColorType which specifies color space for images in the dataset.

*/
void TestToTensor(const Dataset& dataset, const std::vector<int>& size, tensor& images, tensor& labels, ColorType ctype = ColorType::BGR);

/** @example example_ecvl_eddl.cpp
 Example of using ECVL with EDDL.
*/
} // namespace ecvl

#endif // ECVL_EDDL_H_