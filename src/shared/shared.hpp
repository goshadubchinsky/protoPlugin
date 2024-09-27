#ifndef _SHARED_HPP
#define _SHARED_HPP

#include <xsimd/xsimd.hpp>

// Functor for batch size detection
template <typename T>
struct shared
{
    // Declaration of template function to detect batch size
    template <class Arch>
    int batch_size_detector(Arch) const;
};

// Inform the compiler that sse2 and avx2 implementations are in another compilation unit.
extern template int shared<float>::batch_size_detector<xsimd::avx2>(xsimd::avx2) const;
extern template int shared<float>::batch_size_detector<xsimd::sse2>(xsimd::sse2) const;

#endif