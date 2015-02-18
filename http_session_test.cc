#include "http_session.h"
using reddit::HttpSession;
#include <gtest/gtest.h>
#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <iostream>
using std::cout;
#include <jsoncpp/json/json.h>
#include <vector>
using std::vector;

const char kGetUrl[] = "http://httpbin.org/get";

const char kPostUrl[] = "http://httpbin.org/post";
const char kPostKey[] = "awesome";
const char kPostValue[] = "very-yes";

const char kSetCookiesUrl[] = "http://httpbin.org/cookies/set?";
const char kGetCookiesUrl[] = "http://httpbin.org/cookies";

const char kKey1[] = "Virus";
const char kVal1[] = "very-yes";
const char kKey2[] = "Creative";
const char kVal2[] = "not-very";

const char kTestUserAgent[] = "test-user-agent";
const char kUserAgentUrl[] = "http://httpbin.org/user-agent";

Json::Value StringToJson(const string& str) {
   Json::Value val;
   Json::Reader reader;
   reader.parse(str, val);
   return val;
}

TEST(HttpSessionTest, UserAgent) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());
   
   string body;
   EXPECT_TRUE(http->Get(kUserAgentUrl, 0, &body));
   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["user-agent"].isString());
   EXPECT_EQ(val["user-agent"].asString(), kTestUserAgent);
   EXPECT_EQ(val["user-agent"].asString(), http->user_agent());
}

TEST(HttpSessionTest, GetWithoutReply) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   EXPECT_TRUE(http->Get(kGetUrl, 0, 0));
}

TEST(HttpSessionTest, PostWithoutReply) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   EXPECT_TRUE(http->Post(kPostUrl, "", 0, 0));
}

TEST(HttpSessionTest, Get) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   string hdrs, body;
   EXPECT_TRUE(http->Get(kGetUrl, &hdrs, &body));

   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["url"].isString());
   EXPECT_EQ(val["url"].asString(), kGetUrl);
}

TEST(HttpSessionTest, Post) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   string hdrs, body;
   vector<HttpSession::PostField> post_fields = {{kPostKey, kPostValue}};
   EXPECT_TRUE(http->Post(kPostUrl, post_fields, &hdrs, &body));

   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["url"].isString());
   EXPECT_EQ(val["url"].asString(), kPostUrl);
   EXPECT_FALSE(val["form"].isNull());
   ASSERT_TRUE(val["form"][kPostKey].isString());
   EXPECT_EQ(val["form"][kPostKey].asString(), kPostValue);
}

TEST(HttpSessionTest, PostEmptyFields) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   string hdrs, body;
   vector<HttpSession::PostField> post_fields;
   EXPECT_TRUE(http->Post(kPostUrl, post_fields, &hdrs, &body));

   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["url"].isString()) << body;
   EXPECT_EQ(val["url"].asString(), kPostUrl) << body;
}

TEST(HttpSessionTest, Cookies) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   string hdrs, body;
   vector<HttpSession::PostField> fields = {
      {kKey1, kVal1},
      {kKey2, kVal2}};
   EXPECT_TRUE(http->Get(kSetCookiesUrl, fields, 0, 0));
   EXPECT_TRUE(http->Get(kGetCookiesUrl, &hdrs, &body));

   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["cookies"].isObject());
   ASSERT_TRUE(val["cookies"][kKey1].isString());
   EXPECT_EQ(val["cookies"][kKey1].asString(), kVal1);
   ASSERT_TRUE(val["cookies"][kKey2].isString());
   EXPECT_EQ(val["cookies"][kKey2].asString(), kVal2);
}

TEST(HttpSessionTest, CustomHeaders) {
   unique_ptr<HttpSession> http(HttpSession::ConstructNew(kTestUserAgent));
   ASSERT_TRUE(http.get());

   http->AddCustomHeader(kKey1, kVal1);
   http->AddCustomHeader(kKey2, kVal2);

   string body;
   EXPECT_TRUE(http->Get(kGetUrl, 0, &body));
   Json::Value val = StringToJson(body);
   ASSERT_TRUE(val["headers"].isObject());
   ASSERT_TRUE(val["headers"][kKey1].isString());
   EXPECT_EQ(val["headers"][kKey1].asString(), kVal1);
   ASSERT_TRUE(val["headers"][kKey2].isString());
   EXPECT_EQ(val["headers"][kKey2].asString(), kVal2);

   http->ClearCustomHeaders();
   body.clear();
   EXPECT_TRUE(http->Get(kGetUrl, 0, &body));
   val = StringToJson(body);
   ASSERT_TRUE(val["headers"].isObject());
   EXPECT_TRUE(val["headers"][kKey1].isNull());
   EXPECT_TRUE(val["headers"][kKey2].isNull());
}
