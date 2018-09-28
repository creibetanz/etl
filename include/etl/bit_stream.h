#ifndef ETL_BIT_STREAM_INCLUDED
#define ETL_BIT_STREAM_INCLUDED

#include <stdint.h>
#include <limits.h>

#include "etl/platform.h"
#include "etl/static_assert.h"
#include "etl/type_traits.h"
#include "etl/nullptr.h"
#include "etl/endianness.h"
#include "etl/integral_limits.h"
#include "etl/binary.h"

#include "etl/stl/algorithm.h"
#include "etl/stl/iterator.h"

#include "private/minmax_push.h"

namespace etl
{
  //***************************************************************************
  /// Encodes and decodes bitstreams.
  /// Data must be stored in the stream in network order.
  //***************************************************************************
  class bit_stream
  {
  public:

    //***************************************************************************
    /// Default constructor.
    //***************************************************************************
    bit_stream()
      : pdata(nullptr),
        length(0U)
    {
      restart();
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    bit_stream(char *begin_, char* end_)
      : pdata(reinterpret_cast<unsigned char*>(begin_)),
        length(std::distance(begin_, end_))
    {
      restart();
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    bit_stream(unsigned char *begin_, unsigned char* end_)
      : pdata(begin_),
      length(std::distance(begin_, end_))
    {
      restart();
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    bit_stream(char *pdata_, size_t length_)
      : pdata(reinterpret_cast<unsigned char*>(pdata_)),
        length(length_)
    {
      restart();
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    bit_stream(unsigned char *pdata_, size_t length_)
      : pdata(pdata_),
        length(length_)
    {
      restart();
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    void set_stream(char *pdata_, size_t length_)
    {
      pdata  = reinterpret_cast<unsigned char*>(pdata_);
      length = length_;
      restart();
    }

    //***************************************************************************
    /// Construct from begin and length.
    //***************************************************************************
    void set_stream(unsigned char *pdata_, size_t length_)
    {
      pdata  = pdata_;
      length = length_;
      restart();
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    void set_stream(char *begin_, char* end_)
    {
      set_stream(begin_, std::distance(begin_, end_));
    }

    //***************************************************************************
    /// Construct from range.
    //***************************************************************************
    void set_stream(unsigned char *begin_, unsigned char* end_)
    {
      set_stream(begin_, std::distance(begin_, end_));
    }

    //***************************************************************************
    /// Sets the indexes back to the beginning of the stream.
    //***************************************************************************
    void restart() const
    {
      bits_in_byte   = 8;
      byte_index     = 0U;
      bits_remaining = CHAR_BIT * length;
    }

    //***************************************************************************
    /// Returns <b>true</b> if the bitsteam indexes have reached the end.
    //***************************************************************************
    bool empty() const
    {
      return (bits_remaining == 0U);
    }

    //***************************************************************************
    /// Puts a boolean to the stream
    //***************************************************************************
    bool put(bool value)
    {
      bool success = false;

      if (pdata != nullptr)
      {
        if (bits_remaining > 0)
        {
          unsigned char chunk = value ? 1 : 0;
          put_integral(chunk, 1);
          success = true;
        }
      }

      return success;
    }

    //***************************************************************************
    /// For integral types
    //***************************************************************************
    template <typename T>
    typename etl::enable_if<etl::is_integral<T>::value, bool>::type
      put(T value, uint_least8_t width = CHAR_BIT * sizeof(T))
    {
      return put_integral(static_cast<uint32_t>(value), width);
    }

    //***************************************************************************
    /// For floating point types
    //***************************************************************************
    template <typename T>
    typename etl::enable_if<etl::is_floating_point<T>::value, bool>::type
      put(T value)
    {
      bool success = true;

      unsigned char data[sizeof(T)];
      to_bytes(value, data);

      for (size_t i = 0; i < sizeof(T); ++i)
      {
        if (!put_integral(data[i], CHAR_BIT))
        {
          success = false;
        }
      }

      return success;
    }

    //***************************************************************************
    /// For bool types
    //***************************************************************************
    bool get(bool &value)
    {
      bool success = false;

      if (pdata != nullptr)
      {
        // Do we have enough bits?
        if (bits_remaining > 0)
        {
          value = get_bit();
          success = true;
        }
      }

      return success;
    }

    //***************************************************************************
    /// For integral types
    //***************************************************************************
    template <typename T>
    typename etl::enable_if<etl::is_integral<T>::value, bool>::type
      get(T &value, uint_least8_t width = CHAR_BIT * sizeof(T)) const
    {
      bool success = false;
      uint_least8_t bits = width;

      if (pdata != nullptr)
      {
        // Do we have enough bits?
        if (bits_remaining >= width)
        {
          value = 0;

          // Get the bits from the stream.
          while (width != 0)
          {
            unsigned char mask_width = static_cast<unsigned char>(std::min(width, bits_in_byte));
            unsigned char chunk = get_chunk(mask_width);

            width -= mask_width;
            value |= chunk << width;
          }

          success = true;
        }
      }

      // Sign extend if signed type and not already full bit width.
      if (etl::is_signed<T>::value && (bits != (CHAR_BIT * sizeof(T))))
      {
        typedef typename etl::make_signed<T>::type ST;
        value = etl::sign_extend<ST, ST>(value, bits);
      }

      return success;
    }

    //***************************************************************************
    /// For floating point types
    //***************************************************************************
    template <typename T>
    typename etl::enable_if<etl::is_floating_point<T>::value, bool>::type
      get(T& value) const
    {
      bool success = false;

      if (pdata != nullptr)
      {
        uint_least8_t width = CHAR_BIT * sizeof(T);

        // Do we have enough bits?
        if (bits_remaining >= width)
        {
          // Temporary storage.
          unsigned char data[sizeof(T)];

          for (size_t i = 0; i < sizeof(T); ++i)
          {
             get(data[i], CHAR_BIT);
          }

          from_bytes(data, value);

          bits_remaining -= width;

          success = true;
        }
      }

      return success;
    }

  private:

    //***************************************************************************
    /// For unsigned integral types
    //***************************************************************************
    bool put_integral(uint32_t value, uint_least8_t width)
    {
      bool success = false;

      if (pdata != nullptr)
      {
        // Do we have enough bits?
        if (bits_remaining >= width)
        {
          // Send the bits to the stream.
          while (width != 0)
          {
            unsigned char mask_width = static_cast<unsigned char>(std::min(width, bits_in_byte));
            width -= mask_width;
            uint32_t mask = ((uint32_t(1U) << mask_width) - 1U) << width;

            // Move chunk to lowest char bits.
            // Chunks are never larger than one char.
            uint32_t chunk = ((value & mask) >> width) << (bits_in_byte - mask_width);

            put_chunk(static_cast<unsigned char>(chunk), mask_width);
          }

          success = true;
        }
      }

      return success;
    }

    //***************************************************************************
    /// Put a data chunk to the stream
    //***************************************************************************
    void put_chunk(unsigned char chunk, unsigned char width)
    {
      // Clear if new byte.
      if (bits_in_byte == 8)
      {
        pdata[byte_index] = 0U;
      }

      pdata[byte_index] |= chunk;
      step(width);
    }

    //***************************************************************************
    /// Get a data chunk from the stream
    //***************************************************************************
    unsigned char get_chunk(unsigned char width) const
    {
      unsigned char value = pdata[byte_index];

      value >>= (bits_in_byte - width);

      unsigned char mask;

      if (width == CHAR_BIT)
      {
        mask = etl::integral_limits<unsigned char>::max;
      }
      else
      {
        mask = (1U << width) - 1;
      }

      value &= mask;

      step(width);

      return value;
    }

    //***************************************************************************
    /// Get a bool from the stream
    //***************************************************************************
    bool get_bit() const
    {
      bool result = (pdata[byte_index] & (1 << (bits_in_byte - 1))) != 0;

      step(1U);

      return result;
    }

    //***************************************************************************
    /// Helper function for floating point types
    //***************************************************************************
    template <typename T>
    void from_bytes(const unsigned char* data, T& value) const
    {
      unsigned char temp[sizeof(T)];

      std::copy(data, data + sizeof(T), temp);

      // Network to host.
      if (etl::endianness::value() == etl::endian::little)
      {
        std::reverse_copy(data, data + sizeof(T), temp);
      }
      else
      {
        std::copy(data, data + sizeof(T), temp);
      }

      value = *reinterpret_cast<T*>(temp);
    }

    //***************************************************************************
    /// Helper function for floating point types
    //***************************************************************************
    template <typename T>
    void to_bytes(T value, unsigned char* data)
    {
      unsigned char* pf = reinterpret_cast<unsigned char*>(&value);

      std::copy(pf, pf + sizeof(T), data);

      // Host to network.
      if (etl::endianness::value() == etl::endian::little)
      {
        std::reverse_copy(pf, pf + sizeof(T), data);
      }
      else
      {
        std::copy(pf, pf + sizeof(T), data);
      }
    }

    //***************************************************************************
    /// Step the specified number of bits along the stream.
    /// The width will never be larger than 'bits_in_byte'.
    //***************************************************************************
    void step(unsigned char width) const
    {
      bits_in_byte -= width;

      if (bits_in_byte == 0)
      {
        ++byte_index;
        bits_in_byte = 8;
      }

      bits_remaining -= width;
    }

    unsigned char         *pdata;         ///< The start of the bitstream buffer.
    size_t                length;         ///< The length, in unsigned char, of the bitstream buffer.
    mutable unsigned char bits_in_byte;   ///< The number of available bits in the current char.
    mutable size_t        byte_index;     ///< The index of the char in the bitstream buffer.
    mutable size_t        bits_remaining; ///< The number of bits still available in the bitstream buffer.
  };
}

#include "private/minmax_pop.h"

#endif
