/* ================== Include ================== */
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "csv.h"


/* ============= Tests Declaration ============= */

// Demonstrate some basic assertions.
TEST(CsvTest, BasicCSV) {
  // Setup
  std::string filepath = "../../inputs/basic.csv";
  csv file = csv(filepath);
  
  std::vector<std::string> row_1;
  std::vector<std::string> row_2;
  std::vector<std::string> row_3;
  std::vector<std::string> row_4;

  // Execute
  ASSERT_TRUE(file.readRow(row_1));
  ASSERT_TRUE(file.readRow(row_2));
  ASSERT_TRUE(file.readRow(row_3));
  EXPECT_FALSE(file.readRow(row_4));

  // Validate
  EXPECT_EQ(row_1.size(), 2);
  EXPECT_EQ(row_2.size(), 2);
  EXPECT_EQ(row_3.size(), 2);
}
