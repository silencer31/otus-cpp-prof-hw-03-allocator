#include "lib.h"

#include <gtest/gtest.h>

// 
TEST(SomeGtests, TestVersionA){

    // Assert
    ASSERT_TRUE(version() > 0);
}

