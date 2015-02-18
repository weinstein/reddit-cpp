#include "json_utils.h"
#include "file_utils.h"
#include "reddit_types.pb.h"
#include "reddit_agent.h"
#include "reddit_constants.h"
using namespace reddit;

#include <gtest/gtest.h>
#include <iostream>
using std::cout;
#include <jsoncpp/json/json.h>
#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <chrono>
#include <thread>

const char kTestUserAgent[] = "test-user-agent";
const char kCaptchaPngFile[] = "testing/captcha.png";

class RedditAgentTest : public ::testing::Test {
 protected:
   virtual void SetUp() {
      agent_.reset(RedditAgent::ConstructNew(kTestUserAgent));
      ASSERT_TRUE(agent_.get());
   }

   void SleepForMillis(int millis) {
      std::this_thread::sleep_for(std::chrono::milliseconds(millis));
   }

   unique_ptr<RedditAgent> agent_;
};

TEST_F(RedditAgentTest, TestUserAgent) {
   EXPECT_EQ(agent_->user_agent(), string(kTestUserAgent));
}

TEST_F(RedditAgentTest, TestSubredditListing) {
   Listing uiuc_listing = agent_->SubredditHot("uiuc");
   EXPECT_GT(uiuc_listing.children_size(), 0);
   for (const Thing& child : uiuc_listing.children()) {
      EXPECT_TRUE(child.has_link_data());
   }

   SleepForMillis(1000);
}

TEST_F(RedditAgentTest, TestSubredditNew) {
   Listing uiuc_listing = agent_->SubredditNew("uiuc", "", "", 0, 100, false);
   EXPECT_EQ(uiuc_listing.children_size(), 100);
   for (const Thing& child : uiuc_listing.children()) {
      EXPECT_TRUE(child.has_link_data());
   }

   SleepForMillis(1000);
}

TEST_F(RedditAgentTest, TestRandom) {
   Listing random = agent_->SubredditRandom("uiuc");
   ASSERT_EQ(random.children_size(), 1);
   ASSERT_TRUE(random.children(0).has_link_data());
   Link random_link = random.children(0).link_data();
   EXPECT_TRUE(random_link.has_id());

   SleepForMillis(1000);
}

TEST_F(RedditAgentTest, TestCaptcha) {
   EXPECT_TRUE(agent_->NeedsCaptcha());
   string iden = agent_->NewCaptchaIden();
   EXPECT_FALSE(iden.empty());
   string png_data = agent_->GetCaptcha(iden);
   EXPECT_FALSE(png_data.empty());
   
   string fname = string("testing/") + iden + string(".png");
   WriteStringToFile(fname, png_data);
}
