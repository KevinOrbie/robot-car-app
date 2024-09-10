/**
 * @file test_logging.cpp
 * @author Kevin Orbie
 * 
 * @brief Unit tests for the logging functionality.
 */

/* ================== Include ================== */
/* Setup Google Testing Inferastructure */
#include <gtest/gtest.h>  // 

/* Standard C++ Libraries */
#include <string>
#include <vector>

/* Custom C++ Libraries */
#include "common/logger.h"


/* ============= Tests Declaration ============= */

// Demonstrate some basic assertions.
TEST(TestLogger, NoExceptions) {
  /* Setup */
  // None

  /* Execute */
  EXPECT_NO_THROW(LOGI("Test Info"));
  EXPECT_NO_THROW(LOGI("Test int: %d", 5));
  EXPECT_NO_THROW(LOGI("Test string: %s", "Hello World!"));

  EXPECT_NO_THROW(LOGW("Test Warning"));
  EXPECT_NO_THROW(LOGW("Test int: %d", 5));
  EXPECT_NO_THROW(LOGW("Test string: %s", "Hello World!"));

  EXPECT_NO_THROW(LOGE("Test Error"));
  EXPECT_NO_THROW(LOGE("Test int: %d", 5));
  EXPECT_NO_THROW(LOGE("Test string: %s", "Hello World!"));
}
