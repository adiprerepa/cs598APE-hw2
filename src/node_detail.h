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
  // Handle terminal nodes first (likely the most common case)
  if (n.t == node::type::constant) {
    return n.u.val;
  } 
  if (n.t == node::type::variable) {
    return data[(stride * n.u.fid) + idx];
  }
  
  // For non-terminal nodes
  const float abs_inval = fabsf(in[0]);
  
  // Handle unary operators first (more common than binary)
  if (n.t >= node::type::unary_begin && n.t <= node::type::unary_end) {
    switch (n.t) {
    case node::type::abs:
      return abs_inval;
    case node::type::cube:
      return in[0] * in[0] * in[0];
    case node::type::sq:
      return in[0] * in[0];
    case node::type::neg:
      return -in[0];
    case node::type::inv:
      return abs_inval < MIN_VAL ? 0.f : 1.f / in[0];
    case node::type::sqrt:
      return sqrtf(abs_inval);
    case node::type::log:
      return abs_inval < MIN_VAL ? 0.f : logf(abs_inval);
    case node::type::cos:
      return cosf(in[0]);
    case node::type::sin:
      return sinf(in[0]);
    case node::type::tan:
      return tanf(in[0]);
    case node::type::acos:
      return acosf(in[0]);
    case node::type::asin:
      return asinf(in[0]);
    case node::type::atan:
      return atanf(in[0]);
    case node::type::cosh:
      return coshf(in[0]);
    case node::type::sinh:
      return sinhf(in[0]);
    case node::type::tanh:
      return tanhf(in[0]);
    case node::type::acosh:
      return acoshf(in[0]);
    case node::type::asinh:
      return asinhf(in[0]);
    case node::type::atanh:
      return atanhf(in[0]);
    case node::type::cbrt:
      return cbrtf(in[0]);
    case node::type::rcbrt:
      return static_cast<float>(1.0) / cbrtf(in[0]);
    case node::type::rsqrt:
      return static_cast<float>(1.0) / sqrtf(abs_inval);
    case node::type::exp:
      return expf(in[0]);
    default:
      return 0.f;
    }
  }
  
  // Handle binary operators
  if (n.t >= node::type::binary_begin && n.t <= node::type::binary_end) {
    const float abs_inval1 = fabsf(in[1]);
    
    switch (n.t) {
    case node::type::add:
      return in[0] + in[1];
    case node::type::sub:
      return in[0] - in[1];
    case node::type::mul:
      return in[0] * in[1];
    case node::type::div:
      return abs_inval1 < MIN_VAL ? 1.0f : (in[0]/in[1]);
    case node::type::min:
      return fminf(in[0], in[1]);
    case node::type::max:
      return fmaxf(in[0], in[1]);
    case node::type::atan2:
      return atan2f(in[0], in[1]);
    case node::type::fdim:
      return fdimf(in[0], in[1]);
    case node::type::pow:
      return powf(in[0], in[1]);
    default:
      return 0.f;
    }
  }
  
  // Shouldn't reach here
  return 0.f;
}

} // namespace detail
} // namespace genetic
