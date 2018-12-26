///\file

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

#ifndef ETL_SCALED_ROUNDING_INCLUDED
#define ETL_SCALED_ROUNDING_INCLUDED

#include <stdlib.h>

#include "static_assert.h"
#include "type_traits.h"

namespace etl
{
  //*****************************************************************************
  /// A set of rounding algorithms for scaled integrals.
  /// \tparam T       The integral type.
  /// \tparam SCALING The scaling factor.
  ///
  /// \example For emulating fixed point of two decimal places we could use a
  /// scaling factor of '100'. To round the result of scaled int calculations
  /// using 'Banker's Rounding' we would define this.
  /// \code
  /// typedef etl::scaled_rounding<int, 100> Rounding;
  /// int final_result = Rounding::round_half_even_unscaled(accumulated_result);
  /// \endcode
  /// \link http://www.clivemaxfield.com/diycalculator/sp-round.shtml
  //*****************************************************************************
  template <typename T, const T SCALING_>
  class scaled_rounding
  {
  private:

    //***************************************************************************
    // For signed integrals.
    //***************************************************************************
    template <typename U>
    static
    typename etl::enable_if<etl::is_signed<U>::value, U>::type
      absolute(U value)
    {
      return (value < 0 ? -value : value);
    }

    //***************************************************************************
    // For unsigned integrals.
    //***************************************************************************
    template <typename U>
    static
      typename etl::enable_if<etl::is_unsigned<U>::value, U>::type
      absolute(U value)
    {
      return value;
    }

  public:

    static const T SCALING      = SCALING_;
    static const T HALF_SCALING = SCALING_ / T(2);

    ETL_STATIC_ASSERT(etl::is_integral<T>::value,         "Type must be an integral");
    ETL_STATIC_ASSERT(((HALF_SCALING * T(2)) == SCALING), "Scaling must be divisible by 2");

    //***************************************************************************
    /// Round to more positive integer.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_ceiling_unscaled(T value)
    {
      if (value >= 0)
      {
        return (value + SCALING) / SCALING;
      }
      else
      {
        return value / SCALING;
      }
    }

    //***************************************************************************
    /// Round to more positive integer.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_ceiling_scaled(T value)
    {
      return round_ceiling_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round to more negative integer.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_floor_unscaled(T value)
    {
      if (value >= 0)
      {
        return value / SCALING;
      }
      else
      {
        return (value - SCALING) / SCALING;
      }
    }

    //***************************************************************************
    /// Round to more negative integer.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_floor_scaled(T value)
    {
      return round_floor_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded up (to infinity).
    /// Uses 'symmetric up' rounding.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_half_up_unscaled(T value)
    {
      if (value >= 0)
      {
        return (value + HALF_SCALING) / SCALING;
      }
      else
      {
        return (value - HALF_SCALING) / SCALING;
      }
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded up (to infinity).
    /// Uses 'symmetric up' rounding.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_half_up_scaled(T value)
    {
      return round_half_up_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded down (to zero).
    /// Uses 'symmetric down' rounding.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_half_down_unscaled(T value)
    {
      if (value >= 0)
      {
        return (value + (HALF_SCALING - 1)) / SCALING;
      }
      else
      {
        return (value - (HALF_SCALING - 1)) / SCALING;
      }
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded down (to zero).
    /// Uses 'symmetric down' rounding.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_half_down_scaled(T value)
    {
      return round_half_down_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round toward zero.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_zero_unscaled(T value)
    {
      return value / SCALING;
    }

    //***************************************************************************
    /// Round toward zero.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_zero_scaled(T value)
    {
      return round_zero_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round toward infinity.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_infinity_unscaled(T value)
    {
      if (value >= 0)
      {
        return (value + SCALING) / SCALING;
      }
      else
      {
        return (value - SCALING) / SCALING;
      }
    }

    //***************************************************************************
    /// Round toward infinity.
    /// \param value Scaled integral.
    /// \return Ccaled, rounded integral.
    //***************************************************************************
    static T round_infinity_scaled(T value)
    {
      return round_infinity_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded to even integral.
    /// Also known as 'Banker's Rounding'.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_half_even_unscaled(T value)
    {
      if (absolute(value) % SCALING == HALF_SCALING)
      {
        // Odd?
        if ((value / SCALING) & 1U)
        {
          return round_half_up_unscaled(value);
        }
        else
        {
          return round_half_down_unscaled(value);
        }
      }
      else
      {
        return round_half_up_unscaled(value);
      }
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded to even integral.
    /// Also known as 'Banker's Rounding'.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_half_even_scaled(T value)
    {
      return round_half_even_unscaled(value) * SCALING;
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded to odd integral.
    /// Also known as 'Banker's Rounding'.
    /// \param value Scaled integral.
    /// \return Unscaled, rounded integral.
    //***************************************************************************
    static T round_half_odd_unscaled(T value)
    {
      if (absolute(value) % SCALING == HALF_SCALING)
      {
        // Odd?
        if ((value / SCALING) & 1U)
        {
          return round_half_down_unscaled(value);
        }
        else
        {
          return round_half_up_unscaled(value);
        }
      }
      else
      {
        return round_half_up_unscaled(value);
      }
    }

    //***************************************************************************
    /// Round to nearest integer. 'Half' value is rounded to odd integral.
    /// Also known as 'Banker's Rounding'.
    /// \param value Scaled integral.
    /// \return Scaled, rounded integral.
    //***************************************************************************
    static T round_half_odd_scaled(T value)
    {
      return round_half_odd_unscaled(value) * SCALING;
    }
  };
}

#endif
