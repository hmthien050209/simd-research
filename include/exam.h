#ifndef EXAM_H_INCLUDED
#define EXAM_H_INCLUDED

#include <immintrin.h>

#include <cstring>
#include <stdexcept>
#include <vector>

class ByteArray {
 private:
  size_t _size;
  size_t _capacity;
  size_t _block_count;
  int8_t *_values;

#if defined(__AVX512BW__) && defined(__AVX512VL__) && defined(__AVX512F__) && \
    defined(__AVX512DQ__)
  bool _avx512 = true;
#else
  bool _avx512 = false;
#endif

  void construct(const size_t &size) {
    _size = size;
#if defined(__AVX2__) && !(defined(__AVX512BW__) && defined(__AVX512VL__) && \
                           defined(__AVX512F__) && defined(__AVX512DQ__))
    _block_count = (_size >> 5) + ((_size & 31) != 0);
    _capacity = _block_count << 5;
#else
    _block_count = (_size >> 6) + ((_size & 63) != 0);
    _capacity = _block_count << 6;
#endif
    _values = static_cast<int8_t *>(calloc(_capacity, sizeof(int8_t)));
    if (!_values) {
      throw std::runtime_error("Failed to allocate memory for ByteArray");
    }
  }

 public:
  // Initialize an empty ByteArray
  ByteArray() : _size(0), _capacity(0), _block_count(0), _values(nullptr) {}
  // Initialize a ByteArray with `size`
  explicit ByteArray(const size_t &size) {  // NOLINT(*-pro-type-member-init)
    construct(size);
  }

  // Initializer list constructor
  ByteArray(                               // NOLINT(*-pro-type-member-init)
      const std::initializer_list<int8_t>  // NOLINT(*-pro-type-member-init)
          &list) {                         // NOLINT(*-pro-type-member-init)
    construct(list.size());
    // ReSharper disable once CppObjectMemberMightNotBeInitialized
    std::copy(list.begin(), list.end(), _values);
  }

  // Initialize a ByteArray with `size`, filled with `value`
  ByteArray(const size_t &size,     // NOLINT(*-pro-type-member-init)
            const int8_t &value) {  // NOLINT(*-pro-type-member-init)
    construct(size);

    std::fill_n(
        // ReSharper disable once CppObjectMemberMightNotBeInitialized
        _values,
        // ReSharper disable once CppObjectMemberMightNotBeInitialized
        _size, value);
  }

  // Copy constructor
  ByteArray(const ByteArray &other) {  // NOLINT(*-pro-type-member-init)
    construct(other._size);
    // ReSharper disable once CppObjectMemberMightNotBeInitialized
    std::memcpy(_values, other._values, other._size * sizeof(int8_t));
  }

  // Move constructor
  ByteArray(ByteArray &&other) noexcept
      : _size(other._size),
        _capacity(other._capacity),
        _block_count(other._block_count),
        _values(other._values) {
    other._values = nullptr;
    other._size = 0;
    other._capacity = 0;
    other._block_count = 0;
  }

  // Copy assignment operator
  ByteArray &operator=(const ByteArray &other) {
    if (this != &other) {
      free(_values);
      construct(other._size);
      std::memcpy(_values, other._values, other._size * sizeof(int8_t));
    }
    return *this;
  }

  // Move assignment operator
  ByteArray &operator=(ByteArray &&other) noexcept {
    if (this != &other) {
      free(_values);
      _size = other._size;
      _capacity = other._capacity;
      _block_count = other._block_count;
      _values = other._values;
      other._values = nullptr;
      other._size = 0;
      other._capacity = 0;
      other._block_count = 0;
    }
    return *this;
  }

  // Operators
  int8_t &operator[](const size_t &index) const { return _values[index]; }

  // Getters
  [[nodiscard]] size_t capacity() const { return _capacity; }
  [[nodiscard]] size_t size() const { return _size; }
  [[nodiscard]] size_t block_count_avx2() const {
    // ReSharper disable once CppDFAConstantConditions
    return _avx512 ? _block_count << 1 : _block_count;
  }
  [[nodiscard]] size_t block_count_avx512() const {
    // ReSharper disable once CppDFAConstantConditions
    if (!_avx512) {
      throw std::runtime_error("AVX512 is not supported");
    }
    return _block_count;
  }

  // Get the `values` array for direct access
  [[nodiscard]] int8_t *data() const { return _values; }

  // Iterators
  [[nodiscard]] int8_t *begin() const { return _values; }
  [[nodiscard]] int8_t *end() const { return _values + _size; }

  ~ByteArray() {
    if (_values) free(_values);
  }
};

std::vector<ByteArray> generate_exams(const int32_t &number_of_exams,
                                      const int32_t &number_of_questions);
ByteArray generate_correct_answers(const int32_t &number_of_questions);
ByteArray generate_points(const int32_t &number_of_questions);

#endif