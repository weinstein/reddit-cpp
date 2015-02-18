#include "http_session.h"
#include "json_utils.h"
#include "reddit_agent.h"
#include "reddit_constants.h"
#include "reddit_types.pb.h"

#include <jsoncpp/json/json.h>
#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <vector>
using std::vector;

namespace reddit {

string BoolToString(bool val) {
   return val ? "true" : "false";
}

RedditAgent::RedditAgent() {
}

RedditAgent::~RedditAgent() {
}

// static
RedditAgent* RedditAgent::ConstructNew(const std::string& user_agent) {
   unique_ptr<RedditAgent> agent(new RedditAgent);
   if (agent->Init(user_agent)) {
      return agent.release();
   } else {
      return 0;
   }
}

bool RedditAgent::Init(const string& user_agent) {
   http_.reset(HttpSession::ConstructNew(user_agent));
   return http_.get();
}

string RedditAgent::user_agent() const {
   return http_->user_agent();
}

// =================== ACCOUNTS ======================

string RedditAgent::modhash() const {
   return modhash_;
}

void RedditAgent::clear_modhash() {
   modhash_.clear();
}

void RedditAgent::set_modhash(const string& modhash) {
   modhash_ = modhash;
}

bool RedditAgent::Login(const string& user, const string& pass) {
   LoginResponse response = AccountLogin(pass, false, user);
   return !response.errors_size();
}

void RedditAgent::AccountClearSessions(const string& curpass, const string& dest) {
   vector<HttpSession::PostField> fields = {
         {"api_type", "json"},
         {"curpass", curpass},
         {"dest", dest},
         {"uh", modhash_}};
   http_->Post(string(kRedditUrl) + "api/clear_sessions", fields, 0, 0);
}

void RedditAgent::AccountDeleteUser(bool confirm, const string& delete_message,
                                    const string& passwd, const string& user) {
   vector<HttpSession::PostField> fields = {
         {"api_type", "json"},
         {"confirm", BoolToString(confirm)},
         {"delete_message", delete_message},
         {"passwd", passwd},
         {"user", user},
         {"uh", modhash_}};
   http_->Post(string(kRedditUrl) + "api/delete_user", fields, 0, 0);
}

void RedditAgent::AccountRegister(const string& captcha, const string& email,
                                  const string& iden, const string& passwd,
                                  const string& user) {
   vector<HttpSession::PostField> fields = {
         {"api_type", "json"},
         {"captcha", captcha},
         {"email", email},
         {"iden", iden},
         {"passwd", passwd},
         {"passwd2", passwd},
         {"user", user}};
   http_->Post(string(kRedditUrl) + "api/register", fields, 0, 0);
}

void RedditAgent::AccountUpdate(const string& curpass, const string& dest,
                                const string& email, const string& newpass) {
   vector<HttpSession::PostField> fields = {
         {"api_type", "json"},
         {"curpass", curpass},
         {"dest", dest},
         {"email", email},
         {"newpass", newpass},
         {"verify", "true"},
         {"verpass", newpass},
         {"uh", modhash_}};
   http_->Post(string(kRedditUrl) + "api/update", fields, 0, 0);
}

LoginResponse RedditAgent::AccountLogin(const string& passwd, bool rem,
                                        const string& user) {
   LoginResponse result;
   vector<HttpSession::PostField> fields = {{"api_type", "json"},
                                            {"passwd", passwd},
                                            {"rem", BoolToString(rem)},
                                            {"user", user}};
   string body;
   if (http_->Post(string(kRedditUrl) + "api/login", fields, 0, &body)) {
      result = JsonToLoginResponse(StringToJson(body)["json"]);
      if (result.has_data() && result.data().has_modhash()) {
         modhash_ = result.data().modhash();
      }
   }
   return result;
}

Account RedditAgent::AccountMe() {
   Thing result;
   string body;
   if (http_->Get(string(kRedditUrl) + "api/me.json", 0, &body)) {
      result = JsonToThing(StringToJson(body));
   }
   return result.account_data();
}

Account RedditAgent::AccountV1Me() {
   Thing result;
   string body;
   if (http_->Get(string(kRedditUrl) + "api/v1/me", 0, &body)) {
      result = JsonToThing(StringToJson(body));
   }
   return result.account_data();
}

// ================== CAPTCHA =================

bool RedditAgent::NeedsCaptcha() {
   string body;
   if (http_->Get(string(kRedditUrl) + "api/needs_captcha.json", 0, &body)) {
      if (body == BoolToString(false)) {
         return false;
      }
   }
   return true;
}

string RedditAgent::NewCaptchaIden() {
   vector<HttpSession::PostField> fields = {{"api_type", "json"}};
   string body;
   if (http_->Post(string(kRedditUrl) + "api/new_captcha", fields, 0, &body)) {
      Json::Value json = StringToJson(body)["json"];
      if (json["data"].isObject()) {
         return json["data"]["iden"].asString();
      }
   }
   return string();
}

string RedditAgent::GetCaptcha(const string& iden) {
   string body;
   if (http_->Get(string(kRedditUrl) + "captcha/" + iden, 0, &body)) {
      return body;
   }
   return string();
}

// =============== subreddits ====================

Listing RedditAgent::SubredditHot(const string& sub, const string& after,
                                  const string& before, int count, int limit,
                                  bool show_all) {
   vector<HttpSession::PostField> fields;
   if (!after.empty()) {
      fields.push_back({"after", after});
   }
   if (!before.empty()) {
      fields.push_back({"before", before});
   }
   if (count > 0) {
      fields.push_back({"count", std::to_string(count)});
   }
   if (limit > 0) {
      fields.push_back({"limit", std::to_string(limit)});
   }
   if (show_all) {
      fields.push_back({"show", "all"});
   }

   string body;
   if (http_->Get(string(kRedditUrl) + "r/" + sub + "/hot.json", 0, &body)) {
      Thing response = JsonToThing(StringToJson(body));
      return response.listing_data();
   }
   return Listing();
}

Listing RedditAgent::SubredditHot(const string& sub) {
   string body;
   if (http_->Get(string(kRedditUrl) + "r/" + sub + "/hot.json", 0, &body)) {
      Thing response = JsonToThing(StringToJson(body));
      return response.listing_data();
   }
   return Listing();
}

Listing RedditAgent::SubredditNew(const string& sub, const string& after,
                                  const string& before, int count, int limit,
                                  bool show_all) {
   vector<HttpSession::PostField> fields;
   if (!after.empty()) {
      fields.push_back({"after", after});
   }
   if (!before.empty()) {
      fields.push_back({"before", before});
   }
   if (count > 0) {
      fields.push_back({"count", std::to_string(count)});
   }
   if (limit > 0) {
      fields.push_back({"limit", std::to_string(limit)});
   }
   if (show_all) {
      fields.push_back({"show", "all"});
   }

   string body;
   if (http_->Get(string(kRedditUrl) + "r/" + sub + "/new.json",
         fields, 0, &body)) {
      Thing response = JsonToThing(StringToJson(body));
      return response.listing_data();
   }
   return Listing();
}

Listing RedditAgent::SubredditNew(const string& sub) {
   string body;
   if (http_->Get(string(kRedditUrl) + "r/" + sub + "/new.json", 0, &body)) {
      Thing response = JsonToThing(StringToJson(body));
      return response.listing_data();
   }
   return Listing();
}

Listing RedditAgent::SubredditRandom(const string& sub) {
   string body;
   if (http_->Get(string(kRedditUrl) + "r/" + sub + "/random.json", 0, &body)) {
      Json::Value response = StringToJson(body);
      if (response.isArray()) {
         Thing response_thing = JsonToThing(response[0]);
         return response_thing.listing_data();
      }
   }
   return Listing();
}

};  // namespace reddit
