#pragma once

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <node.h>

namespace genetic {
namespace detail {

static constexpr float MIN_VAL = 0.001f;

inline bool is_terminal(node::type t) {
  return t == node::type::variable || t == node::type::constant;
}

inline bool is_nonterminal(node::type t) { return !is_terminal(t); }

inline int arity(node::type t) {
  if (node::type::unary_begin <= t && t <= node::type::unary_end) {
    return 1;
  }
  if (node::type::binary_begin <= t && t <= node::type::binary_end) {
    return 2;
  }
  return 0;
}

// `data` assumed to be stored in col-major format
inline float evaluate_node(const node &n, const float *data, const uint64_t stride,
                    const uint64_t idx, const float *in) {
  // Early returns for terminal nodes to reduce register pressure
  if (n.t == node::type::constant) {
    return n.u.val;
  }
  if (n.t == node::type::variable) {
    return data[(stride * n.u.fid) + idx];
  }
  
  // Pre-compute common values to reduce redundant calculations
  const float abs_inval = fabsf(in[0]);
  
  // Group similar operations together for better branch prediction
  switch (n.t) {
    // Simple arithmetic operations first (no function calls)
    case node::type::add:
      return in[0] + in[1];
    case node::type::sub:
      return in[0] - in[1];
    case node::type::mul:
      return in[0] * in[1];
    case node::type::neg:
      return -in[0];
    case node::type::sq:
      return in[0] * in[0];
    case node::type::cube:
      return in[0] * in[0] * in[0];
    case node::type::abs:
      return abs_inval;
      
    // Division operations
    case node::type::div: {
      const float abs_inval1 = fabsf(in[1]);
      return abs_inval1 < MIN_VAL ? 1.0f : (in[0]/in[1]);
    }
    case node::type::inv:
      return abs_inval < MIN_VAL ? 0.f : 1.f / in[0];
      
    // Min/max operations
    case node::type::max:
      return fmaxf(in[0], in[1]);
    case node::type::min:
      return fminf(in[0], in[1]);
    case node::type::fdim:
      return fdimf(in[0], in[1]);
      
    // Square root family
    case node::type::sqrt:
      return sqrtf(abs_inval);
    case node::type::rsqrt:
      return static_cast<float>(1.0) / sqrtf(abs_inval);
    case node::type::cbrt:
      return cbrtf(in[0]);
    case node::type::rcbrt:
      return static_cast<float>(1.0) / cbrtf(in[0]);
      
    // Trigonometric functions
    case node::type::sin:
      return sinf(in[0]);
    case node::type::cos:
      return cosf(in[0]);
    case node::type::tan:
      return tanf(in[0]);
    case node::type::asin:
      return asinf(in[0]);
    case node::type::acos:
      return acosf(in[0]);
    case node::type::atan:
      return atanf(in[0]);
    case node::type::atan2:
      return atan2f(in[0], in[1]);
      
    // Hyperbolic functions
    case node::type::sinh:
      return sinhf(in[0]);
    case node::type::cosh:
      return coshf(in[0]);
    case node::type::tanh:
      return tanhf(in[0]);
    case node::type::asinh:
      return asinhf(in[0]);
    case node::type::acosh:
      return acoshf(in[0]);
    case node::type::atanh:
      return atanhf(in[0]);
      
    // Other math functions
    case node::type::exp:
      return expf(in[0]);
    case node::type::log:
      return abs_inval < MIN_VAL ? 0.f : logf(abs_inval);
    case node::type::pow:
      return powf(in[0], in[1]);
      
    // Default case
    default:
      return 0.f;
  }
}

} // namespace detail
} // namespace genetic
