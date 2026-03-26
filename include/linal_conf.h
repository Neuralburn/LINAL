/**
 * @file linal_conf.h
 * @brief Public configuration header for LINAL.
 *
 * @details
 *    This header provides configuration options for the linal module.
 *    It is included automatically by linal.h, but applications may
 *    also include it directly before linal.h to keep configuration in
 *    one place. Default values are provided if not overridden.
 *
 * @note
 *    Override any configuration option by defining it before including
 *    this file. Example:
 *    @code
 *    #define LINAL_USE_RESTRICT 1
 *    #include "linal_conf.h"
 *    #include "linal.h"
 *    @endcode
 *
 */
#ifndef LINAL_CONF_H_
#define LINAL_CONF_H_

/* ================ CONFIGURATION =========================================== */

#ifndef LINAL_USE_RESTRICT
/**
 * @def LINAL_USE_RESTRICT
 * @brief Enable restrict keyword for pointer aliasing hints.
 *
 * @details
 *    When enabled, function parameters use the restrict keyword to hint
 *    to the compiler that pointers do not alias, enabling better
 *    optimization. Set to 0 for maximum portability.
 *
 * @note
 *    Default is 1 (enabled). C11 compliant compilers support restrict.
 */
#define LINAL_USE_RESTRICT 1
#endif

#ifndef LINAL_ENABLE_DEBUG_PRINT
/**
 * @def LINAL_ENABLE_DEBUG_PRINT
 * @brief Enable debug print functions in production builds.
 *
 * @details
 *    When set to 0, debug print functions like mat_print are conditionally
 *    compiled out to reduce binary size and runtime overhead.
 *
 * @note
 *    Default is 1 (enabled). Set to 0 for production builds.
 */
#define LINAL_ENABLE_DEBUG_PRINT 1
#endif

#ifndef LINAL_EPSILON
/**
 * @def LINAL_EPSILON
 * @brief Default tolerance for floating-point comparisons.
 *
 * @details
 *    Used internally for numerical comparisons where exact equality
 *    checks may fail due to floating-point precision issues.
 *
 * @note
 *    Default is 1e-10, suitable for most scientific computing tasks.
 */
#define LINAL_EPSILON (1e-10)
#endif

#ifndef LINAL_MAX_DIM
/**
 * @def LINAL_MAX_DIM
 * @brief Maximum matrix dimension for stack allocation optimizations.
 *
 * @details
 *    Matrices with dimensions <= this value may use stack allocation
 *    in future optimizations. Currently reserved for API compatibility.
 *
 * @note
 *    Default is 1024, suitable for most use cases.
 */
#define LINAL_MAX_DIM (1024)
#endif

/**
 * @def LINAL_RESTRICT
 * @brief Conditional restrict keyword based on configuration.
 *
 * @details
 *    Expands to 'restrict' if LINAL_USE_RESTRICT is enabled, otherwise
 *    expands to nothing. Use this in function declarations.
 */
#if LINAL_USE_RESTRICT
#define LINAL_RESTRICT restrict
#else
#define LINAL_RESTRICT
#endif

#endif /* LINAL_CONF_H_ */
