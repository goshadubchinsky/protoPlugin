#include "shared.hpp"

// Definition of the template function outside the class
template <typename T>
template <class Arch>
int shared<T>::batch_size_detector(Arch) const
{
    using batch = xsimd::batch<T, Arch>;
    return batch::size;
}

// Explicit template instantiations for specific architectures
template int shared<float>::batch_size_detector<xsimd::avx2>(xsimd::avx2) const;
template int shared<float>::batch_size_detector<xsimd::sse2>(xsimd::sse2) const;
