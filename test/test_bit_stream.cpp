/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2018 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "UnitTest++.h"

#include "etl/bit_stream.h"

#include <array>
#include <numeric>

namespace
{
  SUITE(test_bit_stream)
  {
    //*************************************************************************
    TEST(put_bool)
    {
      unsigned char storage = 0;
      unsigned char compare_data = 0x5A;

      etl::bit_stream bit_stream(&storage, 1);

      CHECK(bit_stream.put(false));
      CHECK(bit_stream.put(true));
      CHECK(bit_stream.put(false));
      CHECK(bit_stream.put(true));
      CHECK(bit_stream.put(true));
      CHECK(bit_stream.put(false));
      CHECK(bit_stream.put(true));
      CHECK(bit_stream.put(false));

      // One too many.
      CHECK(!bit_stream.put(true));

      CHECK_EQUAL(int(compare_data), int(storage));
    }

    //*************************************************************************
    TEST(put_int8_t)
    {
      std::array<unsigned char, 256> storage;

      std::array<unsigned char, 256> compare_data;
      std::iota(compare_data.begin(), compare_data.end(), 0);

      etl::bit_stream bit_stream(storage.data(), storage.size());

      for (size_t i = 0; i < 256; ++i)
      {
        CHECK(bit_stream.put(int8_t(i)));
      }

      // One too many.
      CHECK(!bit_stream.put(int8_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_uint8_t)
    {
      std::array<unsigned char, 256> storage;

      std::array<unsigned char, 256> compare_data;
      std::iota(compare_data.begin(), compare_data.end(), 0);

      etl::bit_stream bit_stream(storage.data(), storage.size());

      for (size_t i = 0; i < 256; ++i)
      {
        CHECK(bit_stream.put(uint8_t(i)));
      }

      // One too many.
      CHECK(!bit_stream.put(uint8_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_int16_t)
    {
      // Tests assume big endian.
      std::array<unsigned char, sizeof(int16_t) * 4> storage;
      std::array<unsigned char, sizeof(int16_t) * 4> compare_data = { 0x00, 0x01, 0x5A, 0xA5, 0xA5, 0x5A, 0xFF, 0xFF };

      CHECK(compare_data.size() == storage.size());

      etl::bit_stream bit_stream(storage.data(), storage.size());

      CHECK(bit_stream.put(int16_t(0x0001)));
      CHECK(bit_stream.put(int16_t(0x5AA5)));
      CHECK(bit_stream.put(int16_t(0xA55A)));
      CHECK(bit_stream.put(int16_t(0xFFFF)));

      // One too many.
      CHECK(!bit_stream.put(int16_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_uint16_t)
    {
      // Tests assume big endian.
      std::array<unsigned char, sizeof(uint16_t) * 4> storage;
      std::array<unsigned char, sizeof(uint16_t) * 4> compare_data = { 0x00, 0x01, 0x5A, 0xA5, 0xA5, 0x5A, 0xFF, 0xFF };

      CHECK(compare_data.size() == storage.size());

      etl::bit_stream bit_stream(storage.data(), storage.size());

      CHECK(bit_stream.put(uint16_t(0x0001)));
      CHECK(bit_stream.put(uint16_t(0x5AA5)));
      CHECK(bit_stream.put(uint16_t(0xA55A)));
      CHECK(bit_stream.put(uint16_t(0xFFFF)));

      // One too many.
      CHECK(!bit_stream.put(uint16_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_int32_t)
    {
      // Tests assume big endian.
      std::array<unsigned char, sizeof(int32_t) * 4> storage;
      std::array<unsigned char, sizeof(int32_t) * 4> compare_data = { 0x00, 0x00, 0x00, 0x01,
                                                                      0x5A, 0xA5, 0xA5, 0x5A,
                                                                      0xA5, 0x5A, 0x5A, 0xA5,
                                                                      0xFF, 0xFF, 0xFF, 0xFF };

      CHECK(compare_data.size() == storage.size());

      etl::bit_stream bit_stream(storage.data(), storage.size());

      CHECK(bit_stream.put(int32_t(0x00000001)));
      CHECK(bit_stream.put(int32_t(0x5AA5A55A)));
      CHECK(bit_stream.put(int32_t(0xA55A5AA5)));
      CHECK(bit_stream.put(int32_t(0xFFFFFFFF)));

      // One too many.
      CHECK(!bit_stream.put(int32_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_uint32_t)
    {
      // Tests assume big endian.
      std::array<unsigned char, sizeof(uint32_t) * 4> storage;
      std::array<unsigned char, sizeof(uint32_t) * 4> compare_data = { 0x00, 0x00, 0x00, 0x01,
                                                                       0x5A, 0xA5, 0xA5, 0x5A,
                                                                       0xA5, 0x5A, 0x5A, 0xA5,
                                                                       0xFF, 0xFF, 0xFF, 0xFF };

      CHECK(compare_data.size() == storage.size());

      etl::bit_stream bit_stream(storage.data(), storage.size());

      CHECK(bit_stream.put(uint32_t(0x00000001)));
      CHECK(bit_stream.put(uint32_t(0x5AA5A55A)));
      CHECK(bit_stream.put(uint32_t(0xA55A5AA5)));
      CHECK(bit_stream.put(uint32_t(0xFFFFFFFF)));

      // One too many.
      CHECK(!bit_stream.put(uint32_t(0)));

      for (size_t i = 0; i < storage.size(); ++i)
      {
        CHECK_EQUAL(int(compare_data[i]), int(storage[i]));
      }
    }

    //*************************************************************************
    TEST(put_float)
    {
      // Tests assume little endian.
      std::array<unsigned char, sizeof(float) * 4> storage;

      etl::bit_stream bit_stream(storage.data(), storage.size());

      float fmax = std::numeric_limits<float>::max();
      unsigned char* cfmax = reinterpret_cast<unsigned char*>(&fmax);

      float fmin = std::numeric_limits<float>::min();
      unsigned char* cfmin = reinterpret_cast<unsigned char*>(&fmin);

      float flowest = std::numeric_limits<float>::lowest();
      unsigned char* cflowest = reinterpret_cast<unsigned char*>(&flowest);

      float fpi = 3.1415927f;
      unsigned char* cfpi = reinterpret_cast<unsigned char*>(&fpi);

      CHECK(bit_stream.put(fmax));
      CHECK(bit_stream.put(fmin));
      CHECK(bit_stream.put(flowest));
      CHECK(bit_stream.put(fpi));

      // One too many.
      CHECK(!bit_stream.put(0.0f));

      // Check max
      unsigned char* pdata = storage.data();

      for (size_t i = 0, j = (sizeof(float) - 1); i < sizeof(float); ++i, --j)
      {
        CHECK_EQUAL(int(cfmax[j]), int(pdata[i]));
      }

      // Check min
      pdata += sizeof(float);

      for (size_t i = 0, j = (sizeof(float) - 1); i < sizeof(float); ++i, --j)
      {
        CHECK_EQUAL(int(cfmin[j]), int(pdata[i]));
      }

      // Check lowest
      pdata += sizeof(float);

      for (size_t i = 0, j = (sizeof(float) - 1); i < sizeof(float); ++i, --j)
      {
        CHECK_EQUAL(int(cflowest[j]), int(pdata[i]));
      }

      // Check PI
      pdata += sizeof(float);

      for (size_t i = 0, j = (sizeof(float) - 1); i < sizeof(float); ++i, --j)
      {
        CHECK_EQUAL(int(cfpi[j]), int(pdata[i]));
      }
    }

    //*************************************************************************
    TEST(put_double)
    {
      // Tests assume little endian.
      std::array<unsigned char, sizeof(double) * 4> storage;

      etl::bit_stream bit_stream(storage.data(), storage.size());

      double fmax = std::numeric_limits<double>::max();
      unsigned char* cfmax = reinterpret_cast<unsigned char*>(&fmax);

      double fmin = std::numeric_limits<double>::min();
      unsigned char* cfmin = reinterpret_cast<unsigned char*>(&fmin);

      double flowest = std::numeric_limits<double>::lowest();
      unsigned char* cflowest = reinterpret_cast<unsigned char*>(&flowest);

      double fpi = 3.1415927;
      unsigned char* cfpi = reinterpret_cast<unsigned char*>(&fpi);

      CHECK(bit_stream.put(fmax));
      CHECK(bit_stream.put(fmin));
      CHECK(bit_stream.put(flowest));
      CHECK(bit_stream.put(fpi));

      // One too many.
      CHECK(!bit_stream.put(0.0));

      // Check max
      unsigned char* pdata = storage.data();

      for (size_t i = 0, j = (sizeof(double) - 1); i < sizeof(double); ++i, --j)
      {
        CHECK_EQUAL(int(cfmax[j]), int(pdata[i]));
      }

      // Check min
      pdata += sizeof(double);

      for (size_t i = 0, j = (sizeof(double) - 1); i < sizeof(double); ++i, --j)
      {
        CHECK_EQUAL(int(cfmin[j]), int(pdata[i]));
      }

      // Check lowest
      pdata += sizeof(double);

      for (size_t i = 0, j = (sizeof(double) - 1); i < sizeof(double); ++i, --j)
      {
        CHECK_EQUAL(int(cflowest[j]), int(pdata[i]));
      }

      // Check PI
      pdata += sizeof(double);

      for (size_t i = 0, j = (sizeof(double) - 1); i < sizeof(double); ++i, --j)
      {
        CHECK_EQUAL(int(cfpi[j]), int(pdata[i]));
      }
    }

    //*************************************************************************
    TEST(put_long_double)
    {
      // Tests assume little endian.
      std::array<unsigned char, sizeof(long double) * 4> storage;

      etl::bit_stream bit_stream(storage.data(), storage.size());

      long double fmax = std::numeric_limits<long double>::max();
      unsigned char* cfmax = reinterpret_cast<unsigned char*>(&fmax);

      long double fmin = std::numeric_limits<long double>::min();
      unsigned char* cfmin = reinterpret_cast<unsigned char*>(&fmin);

      long double flowest = std::numeric_limits<long double>::lowest();
      unsigned char* cflowest = reinterpret_cast<unsigned char*>(&flowest);

      long double fpi = 3.1415927L;
      unsigned char* cfpi = reinterpret_cast<unsigned char*>(&fpi);

      CHECK(bit_stream.put(fmax));
      CHECK(bit_stream.put(fmin));
      CHECK(bit_stream.put(flowest));
      CHECK(bit_stream.put(fpi));

      // One too many.
      CHECK(!bit_stream.put(0.0));

      // Check max
      unsigned char* pdata = storage.data();

      for (size_t i = 0, j = (sizeof(long double) - 1); i < sizeof(long double); ++i, --j)
      {
        CHECK_EQUAL(int(cfmax[j]), int(pdata[i]));
      }

      // Check min
      pdata += sizeof(long double);

      for (size_t i = 0, j = (sizeof(long double) - 1); i < sizeof(long double); ++i, --j)
      {
        CHECK_EQUAL(int(cfmin[j]), int(pdata[i]));
      }

      // Check lowest
      pdata += sizeof(long double);

      for (size_t i = 0, j = (sizeof(long double) - 1); i < sizeof(long double); ++i, --j)
      {
        CHECK_EQUAL(int(cflowest[j]), int(pdata[i]));
      }

      // Check PI
      pdata += sizeof(long double);

      for (size_t i = 0, j = (sizeof(long double) - 1); i < sizeof(long double); ++i, --j)
      {
        CHECK_EQUAL(int(cfpi[j]), int(pdata[i]));
      }
    }

    //*************************************************************************
    TEST(put_get_bool)
    {
      std::array<bool, 8> flags =
      {
        false, true, false, true, true, false, true, false
      };

      unsigned char storage = 0;
      etl::bit_stream bit_stream(&storage, 1);

      for (size_t i = 0; i < flags.size(); ++i)
      {
        bit_stream.put(flags[i]);
      }

      bit_stream.restart();

      for (size_t i = 0; i < flags.size(); ++i)
      {
        bool flag;
        CHECK(bit_stream.get(flag));
        CHECK_EQUAL(flags[i], flag);
      }
    }

    //*************************************************************************
    TEST(put_get_int8_t)
    {
      std::array<unsigned char, 4 * sizeof(int8_t)> storage;
      std::array<int8_t, 4> put_data = { int8_t(0x01), int8_t(0x5A), int8_t(0xA5), int8_t(0xFF) };
      std::array<int8_t, 4> get_data = { int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x00) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], (int)get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], (int)get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], (int)get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], (int)get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_int8_t_5bits)
    {
      std::array<unsigned char, 4 * sizeof(int8_t)> storage;
      std::array<int8_t, 4> put_data    = { int8_t(0x01), int8_t(0x15), int8_t(0xA5), int8_t(0x1F) }; // 1, -11, 10, -1
      std::array<int8_t, 4> expect_data = { int8_t(0x01), int8_t(0xF5), int8_t(0x05), int8_t(0xFF) }; // 1, -11, 10, -1
      std::array<int8_t, 4> get_data    = { int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x00) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 5);
      bit_stream.put(put_data[1], 5);
      bit_stream.put(put_data[2], 5);
      bit_stream.put(put_data[3], 5);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 5));
      CHECK_EQUAL((int)expect_data[0], (int)get_data[0]);

      CHECK(bit_stream.get(get_data[1], 5));
      CHECK_EQUAL((int)expect_data[1], (int)get_data[1]);

      CHECK(bit_stream.get(get_data[2], 5));
      CHECK_EQUAL((int)expect_data[2], (int)get_data[2]);

      CHECK(bit_stream.get(get_data[3], 5));
      CHECK_EQUAL((int)expect_data[3], (int)get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint8_t)
    {
      std::array<unsigned char, 4 * sizeof(uint8_t)> storage;
      std::array<uint8_t, 4> put_data = { uint8_t(0x01), uint8_t(0x5A), uint8_t(0xA5), uint8_t(0xFF) };
      std::array<uint8_t, 4> get_data = { uint8_t(0x00), uint8_t(0x00), uint8_t(0x00), uint8_t(0x00) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], (int)get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], (int)get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], (int)get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], (int)get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint8_t_5bits)
    {
      std::array<unsigned char, 4 * sizeof(uint8_t)> storage;
      std::array<uint8_t, 4> put_data    = { uint8_t(0x01), uint8_t(0x5A), uint8_t(0xA5), uint8_t(0xFF) }; // 1, -11, 10, -1
      std::array<uint8_t, 4> expect_data = { uint8_t(0x01), uint8_t(0x1A), uint8_t(0x05), uint8_t(0x1F) }; // 1, -11, 10, -1
      std::array<uint8_t, 4> get_data    = { uint8_t(0x00), uint8_t(0x00), uint8_t(0x00), uint8_t(0x00) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 5);
      bit_stream.put(put_data[1], 5);
      bit_stream.put(put_data[2], 5);
      bit_stream.put(put_data[3], 5);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 5));
      CHECK_EQUAL((int)expect_data[0], (int)get_data[0]);

      CHECK(bit_stream.get(get_data[1], 5));
      CHECK_EQUAL((int)expect_data[1], (int)get_data[1]);

      CHECK(bit_stream.get(get_data[2], 5));
      CHECK_EQUAL((int)expect_data[2], (int)get_data[2]);

      CHECK(bit_stream.get(get_data[3], 5));
      CHECK_EQUAL((int)expect_data[3], (int)get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_int16_t)
    {
      std::array<unsigned char, 4 * sizeof(int16_t)> storage;
      std::array<int16_t, 4> put_data = { int16_t(0x0001), int16_t(0xA55A), int16_t(0x5AA5), int16_t(0xFFFF) };
      std::array<int16_t, 4> get_data = { int16_t(0x0000), int16_t(0x0000), int16_t(0x0000), int16_t(0x0000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_int16_t_10bits)
    {
      std::array<unsigned char, 4 * sizeof(int16_t)> storage;
      std::array<int16_t, 4> put_data    = { int16_t(0x0001), int16_t(0xA55A), int16_t(0x5AA5), int16_t(0xFFFF) };
      std::array<int16_t, 4> expect_data = { int16_t(0x0001), int16_t(0x015A), int16_t(0xFEA5), int16_t(0xFFFF) };
      std::array<int16_t, 4> get_data    = { int16_t(0x0000), int16_t(0x0000), int16_t(0x0000), int16_t(0x0000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 10);
      bit_stream.put(put_data[1], 10);
      bit_stream.put(put_data[2], 10);
      bit_stream.put(put_data[3], 10);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 10));
      CHECK_EQUAL(expect_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1], 10));
      CHECK_EQUAL(expect_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2], 10));
      CHECK_EQUAL(expect_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3], 10));
      CHECK_EQUAL(expect_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint16_t)
    {
      std::array<unsigned char, 4 * sizeof(uint16_t)> storage;
      std::array<uint16_t, 4> put_data = { uint16_t(0x0001), uint16_t(0xA55A), uint16_t(0x5AA5), uint16_t(0xFFFF) };
      std::array<uint16_t, 4> get_data = { uint16_t(0x0000), uint16_t(0x0000), uint16_t(0x0000), uint16_t(0x0000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint16_t_10bits)
    {
      std::array<unsigned char, 4 * sizeof(uint16_t)> storage;
      std::array<uint16_t, 4> put_data    = { uint16_t(0x0001), uint16_t(0xA55A), uint16_t(0x5AA5), uint16_t(0xFFFF) };
      std::array<uint16_t, 4> expect_data = { uint16_t(0x0001), uint16_t(0x015A), uint16_t(0x02A5), uint16_t(0x03FF) };
      std::array<uint16_t, 4> get_data    = { uint16_t(0x0000), uint16_t(0x0000), uint16_t(0x0000), uint16_t(0x0000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 10);
      bit_stream.put(put_data[1], 10);
      bit_stream.put(put_data[2], 10);
      bit_stream.put(put_data[3], 10);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 10));
      CHECK_EQUAL(expect_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1], 10));
      CHECK_EQUAL(expect_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2], 10));
      CHECK_EQUAL(expect_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3], 10));
      CHECK_EQUAL(expect_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_int32_t)
    {
      std::array<unsigned char, 4 * sizeof(int32_t)> storage;
      std::array<int32_t, 4> put_data = { int32_t(0x00000001), int32_t(0xA55AA55A), int32_t(0x5AA55AA5), int32_t(0xFFFFFFFF) };
      std::array<int32_t, 4> get_data = { int32_t(0x00000000), int32_t(0x00000000), int32_t(0x00000000), int32_t(0x00000000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_int32_t_22bits)
    {
      std::array<unsigned char, 4 * sizeof(int32_t)> storage;
      std::array<int32_t, 4> put_data    = { int32_t(0x00000001), int32_t(0xA55AA55A), int32_t(0x5AA55AA5), int32_t(0xFFFFFFFF) };
      std::array<int32_t, 4> expect_data = { int32_t(0x00000001), int32_t(0x001AA55A), int32_t(0xFFE55AA5), int32_t(0xFFFFFFFF) };
      std::array<int32_t, 4> get_data    = { int32_t(0x00000000), int32_t(0x00000000), int32_t(0x00000000), int32_t(0x00000000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 22);
      bit_stream.put(put_data[1], 22);
      bit_stream.put(put_data[2], 22);
      bit_stream.put(put_data[3], 22);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 22));
      CHECK_EQUAL(expect_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1], 22));
      CHECK_EQUAL(expect_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2], 22));
      CHECK_EQUAL(expect_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3], 22));
      CHECK_EQUAL(expect_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint32_t)
    {
      std::array<unsigned char, 4 * sizeof(uint32_t)> storage;
      std::array<uint32_t, 4> put_data = { uint32_t(0x00000001), uint32_t(0xA55AA55A), uint32_t(0x5AA55AA5), uint32_t(0xFFFFFFFF) };
      std::array<uint32_t, 4> get_data = { uint32_t(0x00000000), uint32_t(0x00000000), uint32_t(0x00000000), uint32_t(0x00000000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0]);
      bit_stream.put(put_data[1]);
      bit_stream.put(put_data[2]);
      bit_stream.put(put_data[3]);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0]));
      CHECK_EQUAL(put_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1]));
      CHECK_EQUAL(put_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2]));
      CHECK_EQUAL(put_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3]));
      CHECK_EQUAL(put_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_uint32_t_22bits)
    {
      std::array<unsigned char, 4 * sizeof(uint32_t)> storage;
      std::array<uint32_t, 4> put_data    = { uint32_t(0x00000001), uint32_t(0xA55AA55A), uint32_t(0x5AA55AA5), uint32_t(0xFFFFFFFF) };
      std::array<uint32_t, 4> expect_data = { uint32_t(0x00000001), uint32_t(0x001AA55A), uint32_t(0x00255AA5), uint32_t(0x003FFFFF) };
      std::array<uint32_t, 4> get_data    = { uint32_t(0x00000000), uint32_t(0x00000000), uint32_t(0x00000000), uint32_t(0x00000000) };

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream
      bit_stream.put(put_data[0], 22);
      bit_stream.put(put_data[1], 22);
      bit_stream.put(put_data[2], 22);
      bit_stream.put(put_data[3], 22);

      bit_stream.restart();

      CHECK(bit_stream.get(get_data[0], 22));
      CHECK_EQUAL(expect_data[0], get_data[0]);

      CHECK(bit_stream.get(get_data[1], 22));
      CHECK_EQUAL(expect_data[1], get_data[1]);

      CHECK(bit_stream.get(get_data[2], 22));
      CHECK_EQUAL(expect_data[2], get_data[2]);

      CHECK(bit_stream.get(get_data[3], 22));
      CHECK_EQUAL(expect_data[3], get_data[3]);
    }

    //*************************************************************************
    TEST(put_get_multiple_full_size)
    {
      char c1 = 90;
      char c2 = -91;
      unsigned short s1 = 23205;
      unsigned short s2 = 42330;
      int32_t i1 = 1520786085;   // 0x5AA55AA5
      int32_t i2 = -1520786086;  // 0xA55AA55A
      float f = 3.1415927f;
      double d = 3.1415927;
      long double ld = 3.1414927L;

      std::array<unsigned char, 100> storage;

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream.
      bit_stream.put(c1);
      bit_stream.put(s1);
      bit_stream.put(i1);
      bit_stream.put(f);
      bit_stream.put(i2);
      bit_stream.put(d);
      bit_stream.put(s2);
      bit_stream.put(ld);
      bit_stream.put(c2);

      bit_stream.restart();

      char rc1;
      char rc2;
      unsigned short rs1;
      unsigned short rs2;
      int32_t ri1;
      int32_t ri2;
      float rf;
      double rd;
      long double rld;

      // Read them all back.
      CHECK(bit_stream.get(rc1));
      CHECK_EQUAL(int(c1), int(rc1));

      CHECK(bit_stream.get(rs1));
      CHECK_EQUAL(s1, rs1);

      CHECK(bit_stream.get(ri1));
      CHECK_EQUAL(i1, ri1);

      CHECK(bit_stream.get(rf));
      CHECK_CLOSE(f, rf, 0.1f);

      CHECK(bit_stream.get(ri2));
      CHECK_EQUAL(i2, ri2);

      CHECK(bit_stream.get(rd));
      CHECK_CLOSE(d, rd, 0.1);

      CHECK(bit_stream.get(rs2));
      CHECK_EQUAL(s2, rs2);

      CHECK(bit_stream.get(rld));
      CHECK_CLOSE(ld, rld, 0.1);

      CHECK(bit_stream.get(rc2));
      CHECK_EQUAL(int(c2), int(rc2));
    }

    //*************************************************************************
    TEST(put_get_multiple_variable_size)
    {
      char c1 = 26;             // 6 bits
      char c2 = -10;            // 7 bits
      unsigned short s1 = 6742; // 13 bits
      unsigned short s2 = 1878; // 11 bits
      int32_t i1 = 2448037;     // 23 bits
      int32_t i2 = -10836646;   // 25 bits
      float f = 3.1415927f;
      double d = 3.1415927;
      long double ld = 3.1414927L;

      std::array<unsigned char, 100> storage;

      etl::bit_stream bit_stream(storage.data(), storage.size());

      // Insert into the stream.
      bit_stream.put(c1, 6);
      bit_stream.put(s1, 13);
      bit_stream.put(i1, 23);
      bit_stream.put(f);
      bit_stream.put(i2, 25);
      bit_stream.put(d);
      bit_stream.put(s2, 11);
      bit_stream.put(ld);
      bit_stream.put(c2, 7);

      bit_stream.restart();

      char rc1;
      char rc2;
      unsigned short rs1;
      unsigned short rs2;
      int32_t ri1;
      int32_t ri2;
      float rf;
      double rd;
      long double rld;

      // Read them all back.
      CHECK(bit_stream.get(rc1, 6));
      CHECK_EQUAL(int(c1), int(rc1));

      CHECK(bit_stream.get(rs1, 13));
      CHECK_EQUAL(s1, rs1);

      CHECK(bit_stream.get(ri1, 23));
      CHECK_EQUAL(i1, ri1);

      CHECK(bit_stream.get(rf));
      CHECK_CLOSE(f, rf, 0.1f);

      CHECK(bit_stream.get(ri2, 25));
      CHECK_EQUAL(i2, ri2);

      CHECK(bit_stream.get(rd));
      CHECK_CLOSE(d, rd, 0.1);

      CHECK(bit_stream.get(rs2, 11));
      CHECK_EQUAL(s2, rs2);

      CHECK(bit_stream.get(rld));
      CHECK_CLOSE(ld, rld, 0.1);

      CHECK(bit_stream.get(rc2, 7));
      CHECK_EQUAL(int(c2), int(rc2));
    }
  };
}
