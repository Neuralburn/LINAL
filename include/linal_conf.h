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
 *    #define LINAL_MAX 1000000.0f
 *    #include "linal_conf.h"
 *    #include "linal.h"
 *    @endcode
 *
 */
#ifndef LINAL_CONF_H_
#define LINAL_CONF_H_

/* ================ CONFIGURATION =========================================== */

/* ---------------- Example Configuration ----------------------------------- */

#ifndef LINAL_MAX
/**
 * @def LINAL_MAX
 * @brief Maximum value for LINAL operations.
 *
 * @details
 *    This defines the maximum value that can be processed by LINAL.
 *    Adjust this based on your application requirements.
 *
 * @note
 *    The default value of 1000000 is suitable for most applications.
 *    Override this value before including this header if needed.
 */
#define LINAL_MAX (1000000)
#endif

#ifndef LINAL_MIN
/**
 * @def LINAL_MIN
 * @brief Minimum value for LINAL operations.
 *
 * @details
 *    This defines the minimum value that can be processed by LINAL.
 *
 * @note
 *    The default value of -1000000 complements the maximum value.
 */
#define LINAL_MIN (-1000000)
#endif

#endif /* LINAL_CONF_H_ */