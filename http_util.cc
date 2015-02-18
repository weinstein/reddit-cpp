#include "http_session.h"

#include <gflags/gflags.h>
#include <iostream>
using std::cout;
using std::cerr;
#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <vector>
using std::vector;

DEFINE_string(url, "", "URL to GET/POST");
DEFINE_string(post_data, "", "data for POST fields");

namespace reddit {

void PrintUsage(const string& exec) {
   cerr << "Usage: " << exec << " <get|post>\n";
}

int get_main(HttpSession* http);
int post_main(HttpSession* http);

int get_main(HttpSession* http) {
   if (!http) {
      return 1;
   }
   if (FLAGS_url.empty()) {
      cerr << "Need --url\n";
      return 1;
   }

   string hdrs, body;
   http->Get(FLAGS_url, &hdrs, &body);
   cout << hdrs << "\n" << body << "\n";
   return 0;
}

int post_main(HttpSession* http) {
   if (!http) {
      return 1;
   }
   if (FLAGS_url.empty()) {
      cerr << "Need --url\n";
      return 1;
   }

   string hdrs, body;
   if (!http->Post(FLAGS_url, FLAGS_post_data, &hdrs, &body)) {
      cerr << "Post failed.\n";
      return 2;
   }
   cout << hdrs << "\n" << body << "\n";
   return 0;
}

};  // namespace reddit

using namespace reddit;

int main(int argc, char** argv) {
   google::ParseCommandLineFlags(&argc, &argv, true);

   if (argc != 2) {
      PrintUsage(argv[0]);
      return 1;
   }

   unique_ptr<HttpSession> http(HttpSession::ConstructNew(""));
   if (argv[1] == string("get")) {
      return get_main(http.get());
   } else if (argv[1] == string("post")) {
      return post_main(http.get());
   } else {
      PrintUsage(argv[0]);
      return 1;
   }
}

