#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "reddit_types.pb.h"

#include <string>

namespace google {
namespace protobuf {

class Message;
class FieldDescriptor;

};  // namespace google::protobuf
};  // namespace google


namespace Json {

class Value;

};  // namespace Json


namespace reddit {

Json::Value StringToJson(const std::string& str);
bool JsonHasErrors(const Json::Value& json);

Comment JsonToComment(const Json::Value& json);
Account JsonToAccount(const Json::Value& json);
Link JsonToLink(const Json::Value& json);
Message JsonToMessage(const Json::Value& json);
Subreddit JsonToSubreddit(const Json::Value& json);
Listing JsonToListing(const Json::Value& json);
More JsonToMore(const Json::Value& json);
Thing JsonToThing(const Json::Value& json);

LoginResponse JsonToLoginResponse(const Json::Value& json);

void SetMessageFromJson(google::protobuf::Message* msg,
                        const Json::Value& json);
void MaybeSetField(google::protobuf::Message* msg,
                   const google::protobuf::FieldDescriptor* field,
                   const Json::Value& json_val);
void MaybeAddField(google::protobuf::Message* msg,
                   const google::protobuf::FieldDescriptor* field,
                   const Json::Value& json_val);

};  // namespace reddit

#endif
