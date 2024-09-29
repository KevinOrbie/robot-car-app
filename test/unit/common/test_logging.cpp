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

/* Verify it works without any argument given. */
TEST(TestLogger, LogsWithNoArgs) {
  EXPECT_NO_THROW(LOGI("Test Info"));
  EXPECT_NO_THROW(LOGW("Test Warning"));
  EXPECT_NO_THROW(LOGE("Test Error"));
}

/* Verify it works without one argument given. */
TEST(TestLogger, LogsWithOneArg) {
  /* Strings */
  EXPECT_NO_THROW(LOGI("Test %s", "Info"));
  EXPECT_NO_THROW(LOGW("Test %s", "Warning"));
  EXPECT_NO_THROW(LOGE("Test %s", "Error"));

  /* Ints */
  EXPECT_NO_THROW(LOGI("Test %d", 0));
  EXPECT_NO_THROW(LOGW("Test %d", 0));
  EXPECT_NO_THROW(LOGE("Test %d", 0));

  /* Float */
  EXPECT_NO_THROW(LOGI("Test %f", 0.0f));
  EXPECT_NO_THROW(LOGW("Test %f", 0.0f));
  EXPECT_NO_THROW(LOGE("Test %f", 0.0f));

  /* Double */
  EXPECT_NO_THROW(LOGI("Test %f", 0.0));
  EXPECT_NO_THROW(LOGW("Test %f", 0.0));
  EXPECT_NO_THROW(LOGE("Test %f", 0.0));
}

/* Verify it works without multiple arguments given. */
TEST(TestLogger, LogsWithMultipleArgs) {
  /* Three Arguments. */
  EXPECT_NO_THROW(LOGI("Test %s, %d, %f", "Info", 0, 0.0f));
  EXPECT_NO_THROW(LOGW("Test %s, %d, %f", "Warning", 0, 0.0f));
  EXPECT_NO_THROW(LOGE("Test %s, %d, %f", "Error", 0, 0.0f));
}