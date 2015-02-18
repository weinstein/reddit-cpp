#include "http_session.h"

#include <curl/curl.h>
#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <vector>
using std::vector;

namespace reddit {

HttpSession::HttpSession(const string& user_agent)
      : curl_(0),
        custom_headers_(0),
        user_agent_(user_agent) {
}

HttpSession::~HttpSession() {
   if (curl_) {
      curl_easy_cleanup(curl_);
   }
   if (custom_headers_) {
      curl_slist_free_all(custom_headers_);
   }
}

bool HttpSession::Init() {
   if (!curl_) {
      curl_ = curl_easy_init();
      curl_easy_setopt(curl_, CURLOPT_COOKIEFILE, "");
      curl_easy_setopt(curl_, CURLOPT_USERAGENT, user_agent_.c_str());
   }
   return curl_;
}

string HttpSession::user_agent() const {
   return user_agent_;
}

// static
HttpSession* HttpSession::ConstructNew(const string& user_agent) {
   unique_ptr<HttpSession> http(new HttpSession(user_agent));
   if (http->Init()) {
      return http.release();
   } else {
      return 0;
   }
}

// static
size_t HttpSession::WriteData(void* ptr, size_t size, size_t nmemb, void* arg) {
   string* out_str = (string*)arg;
   size_t n_wr = size * nmemb;
   out_str->append((char*)ptr, n_wr);
   return n_wr;
}

bool HttpSession::Get(const string& url, const vector<PostField>& fields, string* hdrs, string* body) {
   string fields_str = PostFieldsToString(fields);
   string url_with_fields = url;
   if (url.find("?") != string::npos) {
      url_with_fields.append(fields_str);
   } else {
      url_with_fields.append("?" + fields_str);
   }
   return Get(url_with_fields, hdrs, body);
}

bool HttpSession::Get(const string& url, string* hdrs, string* body) {
   if (!curl_) {
      return false;
   }
   string tmp_hdrs, tmp_body;

   curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
   curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
   curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, HttpSession::WriteData);
   if (!hdrs) {
      hdrs = &tmp_hdrs;
   }
   if (!body) {
      body = &tmp_body;
   }
   curl_easy_setopt(curl_, CURLOPT_WRITEHEADER, hdrs);
   curl_easy_setopt(curl_, CURLOPT_WRITEDATA, body);
   curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, custom_headers_);
   curl_easy_perform(curl_);

   return true;
}

// static
string HttpSession::PostFieldToString(const PostField& field) {
   return field.key + "=" + field.value;
}

// static
string HttpSession::PostFieldsToString(const vector<PostField>& fields) {
   string str;
   for (int i = 0; i < fields.size(); ++i) {
      if (i > 0) {
         str.append("&");
      }
      str.append(PostFieldToString(fields[i]));
   }
   return str;
}

bool HttpSession::Post(const string& url, const vector<PostField>& post_data, string* hdrs, string* body) {
   return Post(url, PostFieldsToString(post_data), hdrs, body);
}

bool HttpSession::Post(const string& url, const string& post_data, string* hdrs, string* body) {
   if (!curl_) {
      return false;
   }
   string tmp_hdrs, tmp_body;

   curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
   curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

   curl_easy_setopt(curl_, CURLOPT_POST, 1L);
   curl_easy_setopt(curl_, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
   curl_easy_setopt(curl_, CURLOPT_COPYPOSTFIELDS, post_data.c_str());

   curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, HttpSession::WriteData);
   if (!hdrs) {
      hdrs = &tmp_hdrs;
   }
   if (!body) {
      body = &tmp_body;
   }
   curl_easy_setopt(curl_, CURLOPT_WRITEHEADER, hdrs);
   curl_easy_setopt(curl_, CURLOPT_WRITEDATA, body);
   curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, custom_headers_);
   curl_easy_perform(curl_);

   return true;
}

void HttpSession::AddCustomHeader(const std::string& key, const std::string& value) {
   string header_line;
   if (value.empty()) {
      header_line = key + ";";
   } else {
      header_line = key + ": " + value;
   }
   custom_headers_ = curl_slist_append(custom_headers_, header_line.c_str());
}

void HttpSession::ClearCustomHeaders() {
   curl_slist_free_all(custom_headers_);
   custom_headers_ = 0;
}

};  // namespace reddit
