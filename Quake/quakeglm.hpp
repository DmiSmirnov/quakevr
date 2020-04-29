#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wmacro-redefined"
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#define GLM_FORCE_INLINE
#define GLM_CONFIG_SIMD GLM_ENABLE
#define GLM_CONFIG_SWIZZLE GLM_SWIZZLE_OPERATOR
#define GLM_CONFIG_ALIGNED_GENTYPES GLM_ENABLE
#define GLM_CONFIG_ANONYMOUS_STRUCT GLM_ENABLE
#define GLM_FORCE_SWIZZLE

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/io.hpp>
#include <gtx/vec_swizzle.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

extern template struct glm::vec<4, signed char, glm::packed_highp>;
extern template struct glm::vec<4, unsigned char, glm::packed_highp>;
extern template struct glm::vec<4, float, glm::packed_highp>;
extern template struct glm::vec<3, float, glm::packed_highp>;
extern template struct glm::vec<2, float, glm::packed_highp>;
