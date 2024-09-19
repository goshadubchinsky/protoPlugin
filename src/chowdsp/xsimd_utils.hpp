#include <xsimd/xsimd.hpp>
#include <algorithm> // For std::fill

void fill_with_zeros(float* z, std::size_t ORDER) {
    using batch_type = xsimd::batch<float>; // SIMD batch type for float
    batch_type zero_batch; // Create a batch (default constructor should initialize to zeros)

    std::size_t batch_size = batch_type::size; // Number of elements in a batch

    // Fill complete batches
    std::size_t num_batches = ORDER / batch_size;
    std::size_t remainder = ORDER % batch_size;

    for (std::size_t i = 0; i < num_batches; ++i) {
        zero_batch.store(z + i * batch_size, xsimd::aligned_mode()); // Store with aligned mode
    }

    // Fill remaining elements if ORDER is not a multiple of batch_size
    if (remainder > 0) {
        std::fill(z + num_batches * batch_size, z + num_batches * batch_size + remainder, 0.0f);
    }
}
