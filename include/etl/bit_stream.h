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
    bit_stream()
      : pdata(nullptr),
        length(0U)
    {
      restart();
    }

    //***************************************************************************
    bit_stream(char *pdata_, size_t length_)
      : pdata(reinterpret_cast<unsigned char*>(pdata_)),
        length(length_)
    {
      restart();
    }

    //***************************************************************************
    bit_stream(unsigned char *pdata_, size_t length_)
      : pdata(pdata_),
        length(length_)
    {
      restart();
    }

    //***************************************************************************
    void set_stream(unsigned char *pdata_, size_t length_)
    {
      pdata  = pdata_;
      length = length_;
      restart();
    }

    //***************************************************************************
    void restart() const
    {
      bits_in_byte   = 8;
      byte_index     = 0U;
      bits_remaining = CHAR_BIT * length;
    }

    //***************************************************************************
    void clear() const
    {
      bits_in_byte   = 8;
      byte_index     = 0U;
      bits_remaining = CHAR_BIT * length;
      std::fill(pdata, pdata + length, 0U);
    }

    //***************************************************************************
    bool empty() const
    {
      return (bits_remaining == 0U);
    }

    //***************************************************************************
    bool put(bool value)
    {
      bool success = false;

      if (pdata != nullptr)
      {
        if (bits_remaining > 0)
        {
          unsigned char chunk = value ? 1 : 0;
          put_chunk(chunk, 1);          
          success = true;
        }
      }

      return success;
    }

    //***************************************************************************
    /// For integral types
    template <typename T>
    typename etl::enable_if<etl::is_integral<T>::value, bool>::type
      put(T value, uint_least8_t width = CHAR_BIT * sizeof(T))
    {
      return put_integral(static_cast<uint32_t>(value), width);
    }

    //***************************************************************************
    /// For floating point types
    template <typename T>
    typename etl::enable_if<etl::is_floating_point<T>::value, bool>::type
      put(T value)
    {
      bool success = true;

      const size_t width = (CHAR_BIT * sizeof(T));

      unsigned char data[sizeof(T)];
      to_bytes(value, data);

      for (int i = 0; i < sizeof(T); ++i)
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
    template <typename T>
    typename etl::enable_if<etl::is_integral<T>::value, bool>::type
      get(T &value, uint_least8_t width  = CHAR_BIT * sizeof(T)) const
    {
      bool success = false;
      uint_least8_t bits = width;

      if (pdata != nullptr)
      {
        // Do we have enough bits?
        if (bits_remaining >= width)
        {
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
        value = etl::sign_extend<T, T>(value, bits);
      }

      return success;
    }

    //***************************************************************************
    /// For floating point types
    template <typename T>
    typename etl::enable_if<etl::is_floating_point<T>::value, bool>::type
      get(T& value) const
    {
      bool success = false;

      if (pdata != nullptr)
      {
        // Do we have enough bits?
        if (bits_remaining >= floating_point_bits<T>::value)
        {
          //unsigned char data[sizeof(T)];

          //bits_remaining -= floating_point_bits<T>::value;
          //success = true;
        }
      }

      return success;
    }

  private:
    
    //***************************************************************************
    /// For unsigned integral types
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

            uint32_t mask;
                       
            if (mask_width == CHAR_BIT * sizeof(uint32_t))
            {
              mask = etl::integral_limits<uint32_t>::max;
            }
            else
            {              
              mask = (1U << mask_width) - 1;
              mask <<= width;
            }           

            uint32_t chunk = value;

            // Move chunk to lowest char bits.
            // Chunks are never larger than one char.
            chunk = (value & mask) >> width;

            put_chunk(static_cast<unsigned char>(chunk), mask_width);
          }

          success = true;
        }
      }

      return success;
    }

    //***************************************************************************
    /// Put a data chunk to the stream
    void put_chunk(unsigned char chunk, unsigned char width)
    {  
      // Move chunk to correct insertion position.
      chunk <<= (bits_in_byte - width);

      pdata[byte_index] |= chunk;

      step(width);
    }

    //***************************************************************************
    /// Get a data chunk from the stream
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
    bool get_bit() const
    {
      bool result = (pdata[byte_index] & (1 << (bits_in_byte - 1))) != 0;

      step(1U);

      return result;
    }

    //***************************************************************************
    /// For floating point types
    template <typename T>
    void from_bytes(const unsigned char* data, T& value)
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
    /// For floating point types
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
    // Step the specified number of bits along the stream
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

    unsigned char         *pdata;
    size_t                length;
    mutable unsigned char bits_in_byte;
    mutable size_t        byte_index;
    mutable size_t        bits_remaining;
  };
}

#include "private/minmax_pop.h"

#endif
