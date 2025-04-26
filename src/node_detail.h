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
  if (n.t == node::type::constant) {
    return n.u.val;
  } else if (n.t == node::type::variable) {
    return data[(stride * n.u.fid) + idx];
  } else {
    const float inval = in[0];
    const float abs_inval = fabsf(inval);
    
    // note: keep the case statements in alphabetical order under each category
    // of operators, but prioritize common operations
    switch (n.t) {
    // binary operators - common ones first
    case node::type::add:
      return inval + in[1];
    case node::type::mul:
      return inval * in[1];
    case node::type::sub:
      return inval - in[1];
    case node::type::div: {
      const float abs_inval1 = fabsf(in[1]);
      return abs_inval1 < MIN_VAL ? 1.0f : (inval/in[1]);
    }
    case node::type::atan2:
      return atan2f(inval, in[1]);
    case node::type::fdim:
      return fdimf(inval, in[1]);
    case node::type::max:
      return fmaxf(inval, in[1]);
    case node::type::min:
      return fminf(inval, in[1]);
    case node::type::pow:
      return powf(inval, in[1]);
    
    // unary operators - common ones first
    case node::type::abs:
      return abs_inval;
    case node::type::sq:
      return inval * inval;
    case node::type::cube:
      return inval * inval * inval;
    case node::type::neg:
      return -inval;
    case node::type::inv:
      return abs_inval < MIN_VAL ? 0.f : 1.f / inval;
    case node::type::sqrt:
      return sqrtf(abs_inval);
    case node::type::sin:
      return sinf(inval);
    case node::type::cos:
      return cosf(inval);
    case node::type::tan:
      return tanf(inval);
    case node::type::log:
      return abs_inval < MIN_VAL ? 0.f : logf(abs_inval);
    case node::type::exp:
      return expf(inval);
    case node::type::acos:
      return acosf(inval);
    case node::type::acosh:
      return acoshf(inval);
    case node::type::asin:
      return asinf(inval);
    case node::type::asinh:
      return asinhf(inval);
    case node::type::atan:
      return atanf(inval);
    case node::type::atanh:
      return atanhf(inval);
    case node::type::cbrt:
      return cbrtf(inval);
    case node::type::cosh:
      return coshf(inval);
    case node::type::rcbrt:
      return 1.0f / cbrtf(inval);
    case node::type::rsqrt:
      return 1.0f / sqrtf(abs_inval);
    case node::type::sinh:
      return sinhf(inval);
    case node::type::tanh:
      return tanhf(inval);
    // shouldn't reach here!
    default:
      return 0.f;
    };
  }
}

} // namespace detail
} // namespace genetic
