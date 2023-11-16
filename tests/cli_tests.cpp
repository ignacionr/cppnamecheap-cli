#include "gtest/gtest.h"
#include "namecheap-cli.hpp"

TEST(CliTest, Constructor) {
    ignacionr::namecheap::cli c("user", "key", "ip");
    EXPECT_EQ(c.getApiUser(), "user");
    // Add more assertions here...
}
