#include "json_utils.h"
#include "reddit_types.pb.h"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
using std::cout;
#include <jsoncpp/json/json.h>
#include <memory>
#include <string>
using std::string;
#include <vector>
using std::vector;

const char kListOfCommentsFile[] = "testing/test_post_comments.json";
const char kListOfLinksFile[] = "testing/uiuc_new.json";
const char kListOfSubsFile[] = "testing/subreddits_new.json";

string GetFileContents(const char* fname) {
   std::ifstream in(fname, std::ios::in | std::ios::binary);
   if (in) {
      string contents;
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
      return contents;
   } else {
      return string();
   }
}

TEST(JsonUtilsTest, TestParseComments) {
   const string comments_str = GetFileContents(kListOfCommentsFile);
   const Json::Value root_json = reddit::StringToJson(comments_str);
   vector<reddit::Thing> parsed;
   for (const Json::Value& list_item : root_json) {
      parsed.push_back(reddit::JsonToThing(list_item));
   }
   ASSERT_EQ(parsed.size(), 2);
   ASSERT_TRUE(parsed[0].has_listing_data());
   ASSERT_TRUE(parsed[1].has_listing_data());

   EXPECT_EQ(parsed[1].listing_data().children_size(), 58);
   for (const reddit::Thing& thing : parsed[1].listing_data().children()) {
      ASSERT_TRUE(thing.has_comment_data() || thing.has_more_data());
      if (thing.has_comment_data()) {
         EXPECT_TRUE(thing.comment_data().has_id());
         EXPECT_TRUE(thing.comment_data().has_name());
         EXPECT_EQ(thing.kind() + "_" + thing.comment_data().id(), thing.comment_data().name());
      }
   }
}

TEST(JsonUtilsTest, TestParseLinks) {
   const string links_str = GetFileContents(kListOfLinksFile);
   const Json::Value links_json = reddit::StringToJson(links_str);
   const reddit::Thing parsed = reddit::JsonToThing(links_json);
   ASSERT_TRUE(parsed.has_listing_data());
   EXPECT_EQ(parsed.listing_data().children_size(), 25);

   for (const reddit::Thing& thing : parsed.listing_data().children()) {
      ASSERT_TRUE(thing.has_link_data());
      EXPECT_TRUE(thing.link_data().has_id());
      EXPECT_TRUE(thing.link_data().has_name());
      EXPECT_EQ(thing.kind() + "_" + thing.link_data().id(), thing.link_data().name());
   }

//   cout << parsed.DebugString() << "\n";
//   cout << "Num children: " << parsed.listing_data().children_size() << "\n";
}

TEST(JsonUtilsTest, TestParseListingOfSubs) {
   const string subs_str = GetFileContents(kListOfSubsFile);
   const Json::Value subs_json = reddit::StringToJson(subs_str);
   const reddit::Thing parsed = reddit::JsonToThing(subs_json);
   ASSERT_TRUE(parsed.has_listing_data());
   EXPECT_EQ(parsed.listing_data().children_size(), 25);

   for (const reddit::Thing& thing : parsed.listing_data().children()) {
      ASSERT_TRUE(thing.has_subreddit_data());
      EXPECT_TRUE(thing.subreddit_data().has_id());
      EXPECT_TRUE(thing.subreddit_data().has_name());
      EXPECT_EQ(thing.kind() + "_" + thing.subreddit_data().id(), thing.subreddit_data().name());
   }

//   cout << parsed.DebugString() << "\n";
//   cout << "Num children: " << parsed.listing_data().children_size() << "\n";
}
