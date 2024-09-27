//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_DEFINEMACROS_HPP
#define C_ENGINE_DEFINEMACROS_HPP

/**
 * \file defineMacros.hpp
 * \brief Contains macros that can be defined to enable or disable certain features in the engine.
 *
 * This file contains a list of macros that can be defined to enable or disable certain features in the engine.
 */

/**
 * \def C_ENGINE_USE_VULKAN
 * \brief Enables the Vulkan rendering backend and Vulkan-specific features.
 *
 * If this macro is not defined, no Vulkan-specific features will be available.
 *
 * \see C_ENGINE_USE_VKLOG
 */
#define C_ENGINE_USE_VULKAN

/**
 * \def C_ENGINE_USE_VKLOG
 * \brief Enables the Vulkan logging feature.
 *
 * If this macro is not defined, no Vulkan logging will be available.
 * This will only work if \c C_ENGINE_USE_VULKAN is defined.
 * This will create a file called \c vklog.log in the working directory.
 * A filter must be set for this feature to work.
 *
 * \see C_ENGINE_VKLOG_FILTER_ALL, C_ENGINE_VKLOG_FILTER_IMPORTANT, C_ENGINE_VKLOG_FILTER_ERROR
 */
#define C_ENGINE_USE_VKLOG

/**
 * \def C_ENGINE_VKLOG_FILTER_ALL
 * \brief Logs all Vulkan messages.
 *
 * \see C_ENGINE_USE_VKLOG, C_ENGINE_VKLOG_FILTER_IMPORTANT, C_ENGINE_VKLOG_FILTER_ERROR
 */
#define C_ENGINE_VKLOG_FILTER_ALL

/**
 * \def C_ENGINE_VKLOG_FILTER_IMPORTANT
 * \brief Logs only important Vulkan messages, such as errors and warnings.
 *
 * \see C_ENGINE_USE_VKLOG, C_ENGINE_VKLOG_FILTER_ALL, C_ENGINE_VKLOG_FILTER_ERROR
 */
#define C_ENGINE_VKLOG_FILTER_IMPORTANT

/**
 * \def C_ENGINE_VKLOG_FILTER_ERROR
 * \brief Logs only Vulkan errors.
 *
 * \see C_ENGINE_USE_VKLOG, C_ENGINE_VKLOG_FILTER_ALL, C_ENGINE_VKLOG_FILTER_IMPORTANT
 */
#define C_ENGINE_VKLOG_FILTER_ERROR

/**
 * \def C_ENGINE_USE_THREADS
 * \brief Enables multi-threading support in the engine.
 */
#define C_ENGINE_USE_THREADS

#endif //C_ENGINE_DEFINEMACROS_HPP
