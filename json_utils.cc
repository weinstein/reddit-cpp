#include "json_utils.h"
#include "reddit_constants.h"
#include "reddit_types.pb.h"

#include <jsoncpp/json/json.h>
#include <string>
using std::string;
#include <google/protobuf/message.h>
using google::protobuf::Message;
using google::protobuf::Reflection;
#include <google/protobuf/descriptor.h>
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;

namespace reddit {

Json::Value StringToJson(const std::string& str) {
   Json::Reader reader;
   Json::Value json;
   reader.parse(str, json);
   return json;
}

bool JsonHasErrors(const Json::Value& json) {
   return !json["errors"].empty();
}

Comment JsonToComment(const Json::Value& json) {
   Comment comment;
   if (!json["replies"].isNull()) {
      *comment.mutable_replies() = JsonToThing(json["replies"]);
   }
   SetMessageFromJson(&comment, json);
   return comment;
}

Account JsonToAccount(const Json::Value& json) {
   Account account;
   SetMessageFromJson(&account, json);
   return account;
}

Link JsonToLink(const Json::Value& json) {
   Link link;
   if (!json["media"].isNull()) {
      link.set_media(json["media"].toStyledString());
   }
   if (!json["media_embed"].isNull()) {
      link.set_media_embed(json["media_embed"].toStyledString());
   }
   SetMessageFromJson(&link, json);
   return link;
}

reddit::Message JsonToMessage(const Json::Value& json) {
   reddit::Message message;
   if (json["new"].isBool()) {
      message.set_new_(json["new"].asBool());
   }
   SetMessageFromJson(&message, json);
   return message;
}

Subreddit JsonToSubreddit(const Json::Value& json) {
   Subreddit subreddit;
   SetMessageFromJson(&subreddit, json);
   return subreddit;
}

Listing JsonToListing(const Json::Value& json) {
   Listing list;
   const Json::Value& json_children = json["children"];
   for (const Json::Value& child : json_children) {
      *list.add_children() = JsonToThing(child);
   }
   SetMessageFromJson(&list, json);
   return list;
}

More JsonToMore(const Json::Value& json) {
   More more;
   SetMessageFromJson(&more, json);
   return more;
}

Thing JsonToThing(const Json::Value& json) {
   Thing thing;
   if (!json.isNull() && !json.isObject()) {
      return thing;
   }
   if (json["kind"].isString()) {
      thing.set_kind(json["kind"].asString());
   }

   const Json::Value& data = json["data"];
   if (thing.kind() == kCommentPrefix) {
      *thing.mutable_comment_data() = JsonToComment(data);
   } else if (thing.kind() == kAccountPrefix) {
      *thing.mutable_account_data() = JsonToAccount(data);
   } else if (thing.kind() == kLinkPrefix) {
      *thing.mutable_link_data() = JsonToLink(data);
   } else if (thing.kind() == kMessagePrefix) {
      *thing.mutable_message_data() = JsonToMessage(data);
   } else if (thing.kind() == kSubredditPrefix) {
      *thing.mutable_subreddit_data() = JsonToSubreddit(data);
   } else if (thing.kind() == kListingPrefix) {
      *thing.mutable_listing_data() = JsonToListing(data);
   } else if (thing.kind() == kMorePrefix) {
      *thing.mutable_more_data() = JsonToMore(data);
   }

   return thing;
}

LoginResponse JsonToLoginResponse(const Json::Value& json) {
   LoginResponse response;
   for (const Json::Value& json_err : json["errors"]) {
      Error* error = response.add_errors();
      for (const Json::Value& json_err_info : json_err) {
         if (json_err_info.isString()) {
            error->add_info(json_err_info.asString());
         }
      }
   }
   SetMessageFromJson(&response, json);
   return response;
}

// Iterate through unset fields in the protobuf, and set them according to the
// data in the json value
void SetMessageFromJson(google::protobuf::Message* msg, const Json::Value& json) {
   const Descriptor* msg_desc = msg->GetDescriptor();
   const Reflection* msg_refl = msg->GetReflection();

   for (int i = 0; i < msg_desc->field_count(); ++i) {
      const FieldDescriptor* field = msg_desc->field(i);
      const Json::Value& json_val = json[field->name()];
      if (json_val.empty()) {
         continue;
      }

      if (field->is_repeated()) {
         if (msg_refl->FieldSize(*msg, field)) {
            continue;
         }
         for (const Json::Value& arr_item : json_val) {
            MaybeAddField(msg, field, arr_item);
         }
      } else {  // not repeated
         if (msg_refl->HasField(*msg, field)) {
            continue;
         }
         MaybeSetField(msg, field, json_val);
      }
   }
}

void MaybeSetField(google::protobuf::Message* msg, const FieldDescriptor* field,
                   const Json::Value& json_val) {
   const Reflection* msg_refl = msg->GetReflection();

   if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT32) {
      if (json_val.isNumeric()) {
         msg_refl->SetInt32(msg, field, json_val.asInt());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT32) {
      if (json_val.isNumeric()) {
         msg_refl->SetUInt32(msg, field, json_val.asUInt());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT64) {
      if (json_val.isNumeric()) {
         msg_refl->SetInt64(msg, field, json_val.asInt64());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT64) {
      if (json_val.isNumeric()) {
         msg_refl->SetUInt64(msg, field, json_val.asUInt64());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_FLOAT) {
      if (json_val.isNumeric()) {
         msg_refl->SetFloat(msg, field, json_val.asFloat());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_DOUBLE) {
      if (json_val.isNumeric()) {
         msg_refl->SetDouble(msg, field, json_val.asDouble());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_BOOL) {
      if (json_val.isBool()) {
         msg_refl->SetBool(msg, field, json_val.asBool());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_STRING) {
      if (json_val.isString()) {
         msg_refl->SetString(msg, field, json_val.asString());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      google::protobuf::Message* embedded_msg = msg_refl->MutableMessage(msg, field);
      SetMessageFromJson(embedded_msg, json_val);
   }
}

void MaybeAddField(google::protobuf::Message* msg, const FieldDescriptor* field,
                   const Json::Value& json_val) {
   const Reflection* msg_refl = msg->GetReflection();

   if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT32) {
      if (json_val.isNumeric()) {
         msg_refl->AddInt32(msg, field, json_val.asInt());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT32) {
      if (json_val.isNumeric()) {
         msg_refl->AddUInt32(msg, field, json_val.asUInt());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT64) {
      if (json_val.isNumeric()) {
         msg_refl->AddInt64(msg, field, json_val.asInt64());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT64) {
      if (json_val.isNumeric()) {
         msg_refl->AddUInt64(msg, field, json_val.asUInt64());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_FLOAT) {
      if (json_val.isNumeric()) {
         msg_refl->AddFloat(msg, field, json_val.asFloat());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_DOUBLE) {
      if (json_val.isNumeric()) {
         msg_refl->AddDouble(msg, field, json_val.asDouble());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_BOOL) {
      if (json_val.isBool()) {
         msg_refl->AddBool(msg, field, json_val.asBool());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_STRING) {
      if (json_val.isString()) {
         msg_refl->AddString(msg, field, json_val.asString());
      }
   } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      google::protobuf::Message* embedded_msg = msg_refl->AddMessage(msg, field);
      SetMessageFromJson(embedded_msg, json_val);
   }
}

};  // namespace reddit
