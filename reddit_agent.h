#ifndef REDDIT_AGENT_H
#define REDDIT_AGENT_H

#include "reddit_types.pb.h"

#include <memory>
#include <jsoncpp/json/json.h>

namespace reddit {

class HttpSession;

class RedditAgent {
 public:
   ~RedditAgent();
   static RedditAgent* ConstructNew(const std::string& user_agent);

   std::string user_agent() const;
   std::string modhash() const;
   void clear_modhash();
   void set_modhash(const std::string& modhash);

   // ====== account stuff ==========
   
   bool Login(const std::string& user, const std::string& pass);

   void AccountClearSessions(const std::string& curpass,
                             const std::string& dest);
   void AccountDeleteUser(bool confirm, const std::string& delete_message,
                          const std::string& passwd, const std::string& user);
   LoginResponse AccountLogin(const std::string& passwd, bool rem,
                              const std::string& user);
   Account AccountMe();
   void AccountRegister(const std::string& captcha, const std::string& email,
                        const std::string& iden, const std::string& passwd,
                        const std::string& user);
   void AccountUpdate(const std::string& curpass, const std::string& dest,
                      const std::string& email, const std::string& newpass);
   Account AccountV1Me();

   // ====== captcha =========
   bool NeedsCaptcha();
   std::string NewCaptchaIden();
   std::string GetCaptcha(const std::string& iden);

   // ====== subreddit =======
   
   Listing SubredditHot(const std::string& subreddit, const std::string& after,
                        const std::string& before, int count, int limit,
                        bool show_all);
   Listing SubredditNew(const std::string& subreddit, const std::string& after,
                        const std::string& before, int count, int limit,
                        bool show_all);

   Listing SubredditHot(const std::string& subreddit);
   Listing SubredditNew(const std::string& subreddit);
   Listing SubredditRandom(const std::string& subreddit);
   
 private:
   std::unique_ptr<HttpSession> http_;
   std::string modhash_;

   RedditAgent();
   bool Init(const std::string& user_agent);
};

};  // namespace reddit

#endif
