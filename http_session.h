#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <string>
#include <vector>
#include <curl/curl.h>

namespace reddit {

class HttpSession {
 public:
   ~HttpSession();
   static HttpSession* ConstructNew(const std::string& user_agent);

   struct PostField {
      std::string key;
      std::string value;
   };

   std::string user_agent() const;

   static std::string PostFieldToString(const PostField& field);
   static std::string PostFieldsToString(const std::vector<PostField>& fields);

   bool Get(const std::string& url, std::string* hdrs, std::string* body);
   bool Get(const std::string& url, const std::vector<PostField>& fields, std::string* hdrs, std::string* body);
   bool Post(const std::string& url, const std::vector<PostField>& post_data, std::string* hdrs, std::string* body);
   bool Post(const std::string& url, const std::string& post_data, std::string* hdrs, std::string* body);

   void AddCustomHeader(const std::string& key, const std::string& value);
   void ClearCustomHeaders();

 private:
   CURL* curl_;
   curl_slist* custom_headers_;
   const std::string user_agent_;

   HttpSession(const std::string& user_agent);
   bool Init();

   static size_t WriteData(void* ptr, size_t size, size_t nmemb, void* arg);
};

};  // namespace 

#endif
