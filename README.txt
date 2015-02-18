Dependencies:

libcurl-dev
   for handling HTTP sessions and cookies
libgflags-dev
   not strictly required for library classes, but provides convenient
   command line flag features for CLI executables
libgtest-dev
   required for running test executables
libprotobuf-dev
   for reddit data types
libjsoncpp-dev
   for parsing JSON responses from reddit



Most of the interesting stuff is handled by the reddit_agent class.
* See reddit_agent.h

Google protobuffers are used for encapsulating Reddit datatypes.
* See reddit_types.proto for protobuf definitions
* See https://github.com/reddit/reddit/wiki/JSON for more detailed docs
  - but I've found that some documentation is out of date
