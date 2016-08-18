#ifndef SRC_LAYER_CONV_H_
#define SRC_LAYER_CONV_H_

#include <string>

#include "layer/param_layer.h"
#include "util/common.h"
#include "transform/activation.h"

#ifndef BLITZ_CPU_ONLY
  #include "util/blitz_gpu_function.h"
#endif

namespace blitz {

template<template <typename> class TensorType, typename DType>
class Conv : public ParamLayer<TensorType, DType> {
 public:
  explicit Conv(
    const string& name, const string& filler_name,
    const string& optimizer_name,
    shared_ptr<Activation<TensorType, DType> > activation,
    const Shape& filter_shape,
    const int stride_height = 1, const int stride_width = 1,
    const int padding_height = 0, const int padding_width = 0,
    const string& kernel = "blas") :
    ParamLayer<TensorType, DType>(name, filler_name,
    optimizer_name, activation), filter_shape_(filter_shape),
    stride_height_(stride_height), stride_width_(stride_width),
    padding_height_(padding_height), padding_width_(padding_width),
    kernel_(kernel) {}
  ~Conv() {}

  virtual void InitImpl(const Shape& input_shape);
  virtual void ForwardPropImpl(shared_ptr<TensorType<DType> > forward_input);
  virtual void BackwardPropImpl(shared_ptr<TensorType<DType> > backward_input);

 private:
  // TODO(keren) bias
  const Shape filter_shape_;

  shared_ptr<TensorType<DType> > unpack_;

  const int stride_height_;
  const int stride_width_;
  const int padding_height_;
  const int padding_width_;

  const string kernel_;

// cudnn handles
#ifndef BLITZ_CPU_ONLY
  cudnnHandle_t cudnn_handle_;
  cudaStream_t cudnn_stream_;

  // algorithms for forward and backwards convolutions
  cudnnConvolutionFwdAlgo_t forward_algorithm_;
  cudnnConvolutionBwdFilterAlgo_t backward_filter_algorithm_;
  cudnnConvolutionBwdDataAlgo_t backward_data_algorithm_;

  cudnnTensorDescriptor_t input_desc_, output_desc_;
  cudnnFilterDescriptor_t filter_desc_;
  cudnnConvolutionDescriptor_t conv_desc_;

  DType *cudnn_alpha_, *cudnn_beta_;
#endif
};

}  // namespace blitz

#endif  // SRC_LAYER_CONV_H_