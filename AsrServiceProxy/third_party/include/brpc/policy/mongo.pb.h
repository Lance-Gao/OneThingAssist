// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: brpc/policy/mongo.proto

#ifndef PROTOBUF_brpc_2fpolicy_2fmongo_2eproto__INCLUDED
#define PROTOBUF_brpc_2fpolicy_2fmongo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/service.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace brpc {
namespace policy {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_brpc_2fpolicy_2fmongo_2eproto();
void protobuf_AssignDesc_brpc_2fpolicy_2fmongo_2eproto();
void protobuf_ShutdownFile_brpc_2fpolicy_2fmongo_2eproto();

class MongoHeader;
class MongoRequest;
class MongoResponse;

enum MongoOp {
  OPREPLY = 1,
  DBMSG = 1000,
  DB_UPDATE = 2001,
  DB_INSERT = 2002,
  DB_QUERY = 2004,
  DB_GETMORE = 2005,
  DB_DELETE = 2006,
  DB_KILLCURSORS = 2007,
  DB_COMMAND = 2008,
  DB_COMMANDREPLY = 2009
};
bool MongoOp_IsValid(int value);
const MongoOp MongoOp_MIN = OPREPLY;
const MongoOp MongoOp_MAX = DB_COMMANDREPLY;
const int MongoOp_ARRAYSIZE = MongoOp_MAX + 1;

const ::google::protobuf::EnumDescriptor* MongoOp_descriptor();
inline const ::std::string& MongoOp_Name(MongoOp value) {
  return ::google::protobuf::internal::NameOfEnum(
    MongoOp_descriptor(), value);
}
inline bool MongoOp_Parse(
    const ::std::string& name, MongoOp* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MongoOp>(
    MongoOp_descriptor(), name, value);
}
// ===================================================================

class MongoHeader : public ::google::protobuf::Message {
 public:
  MongoHeader();
  virtual ~MongoHeader();

  MongoHeader(const MongoHeader& from);

  inline MongoHeader& operator=(const MongoHeader& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MongoHeader& default_instance();

  void Swap(MongoHeader* other);

  // implements Message ----------------------------------------------

  MongoHeader* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MongoHeader& from);
  void MergeFrom(const MongoHeader& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 message_length = 1;
  inline bool has_message_length() const;
  inline void clear_message_length();
  static const int kMessageLengthFieldNumber = 1;
  inline ::google::protobuf::int32 message_length() const;
  inline void set_message_length(::google::protobuf::int32 value);

  // required int32 request_id = 2;
  inline bool has_request_id() const;
  inline void clear_request_id();
  static const int kRequestIdFieldNumber = 2;
  inline ::google::protobuf::int32 request_id() const;
  inline void set_request_id(::google::protobuf::int32 value);

  // required int32 response_to = 3;
  inline bool has_response_to() const;
  inline void clear_response_to();
  static const int kResponseToFieldNumber = 3;
  inline ::google::protobuf::int32 response_to() const;
  inline void set_response_to(::google::protobuf::int32 value);

  // required .brpc.policy.MongoOp op_code = 4;
  inline bool has_op_code() const;
  inline void clear_op_code();
  static const int kOpCodeFieldNumber = 4;
  inline ::brpc::policy::MongoOp op_code() const;
  inline void set_op_code(::brpc::policy::MongoOp value);

  // @@protoc_insertion_point(class_scope:brpc.policy.MongoHeader)
 private:
  inline void set_has_message_length();
  inline void clear_has_message_length();
  inline void set_has_request_id();
  inline void clear_has_request_id();
  inline void set_has_response_to();
  inline void clear_has_response_to();
  inline void set_has_op_code();
  inline void clear_has_op_code();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 message_length_;
  ::google::protobuf::int32 request_id_;
  ::google::protobuf::int32 response_to_;
  int op_code_;
  friend void  protobuf_AddDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_AssignDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_ShutdownFile_brpc_2fpolicy_2fmongo_2eproto();

  void InitAsDefaultInstance();
  static MongoHeader* default_instance_;
};
// -------------------------------------------------------------------

class MongoRequest : public ::google::protobuf::Message {
 public:
  MongoRequest();
  virtual ~MongoRequest();

  MongoRequest(const MongoRequest& from);

  inline MongoRequest& operator=(const MongoRequest& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MongoRequest& default_instance();

  void Swap(MongoRequest* other);

  // implements Message ----------------------------------------------

  MongoRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MongoRequest& from);
  void MergeFrom(const MongoRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .brpc.policy.MongoHeader header = 1;
  inline bool has_header() const;
  inline void clear_header();
  static const int kHeaderFieldNumber = 1;
  inline const ::brpc::policy::MongoHeader& header() const;
  inline ::brpc::policy::MongoHeader* mutable_header();
  inline ::brpc::policy::MongoHeader* release_header();
  inline void set_allocated_header(::brpc::policy::MongoHeader* header);

  // required string message = 2;
  inline bool has_message() const;
  inline void clear_message();
  static const int kMessageFieldNumber = 2;
  inline const ::std::string& message() const;
  inline void set_message(const ::std::string& value);
  inline void set_message(const char* value);
  inline void set_message(const char* value, size_t size);
  inline ::std::string* mutable_message();
  inline ::std::string* release_message();
  inline void set_allocated_message(::std::string* message);

  // @@protoc_insertion_point(class_scope:brpc.policy.MongoRequest)
 private:
  inline void set_has_header();
  inline void clear_has_header();
  inline void set_has_message();
  inline void clear_has_message();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::brpc::policy::MongoHeader* header_;
  ::std::string* message_;
  friend void  protobuf_AddDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_AssignDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_ShutdownFile_brpc_2fpolicy_2fmongo_2eproto();

  void InitAsDefaultInstance();
  static MongoRequest* default_instance_;
};
// -------------------------------------------------------------------

class MongoResponse : public ::google::protobuf::Message {
 public:
  MongoResponse();
  virtual ~MongoResponse();

  MongoResponse(const MongoResponse& from);

  inline MongoResponse& operator=(const MongoResponse& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MongoResponse& default_instance();

  void Swap(MongoResponse* other);

  // implements Message ----------------------------------------------

  MongoResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MongoResponse& from);
  void MergeFrom(const MongoResponse& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .brpc.policy.MongoHeader header = 1;
  inline bool has_header() const;
  inline void clear_header();
  static const int kHeaderFieldNumber = 1;
  inline const ::brpc::policy::MongoHeader& header() const;
  inline ::brpc::policy::MongoHeader* mutable_header();
  inline ::brpc::policy::MongoHeader* release_header();
  inline void set_allocated_header(::brpc::policy::MongoHeader* header);

  // required int32 response_flags = 2;
  inline bool has_response_flags() const;
  inline void clear_response_flags();
  static const int kResponseFlagsFieldNumber = 2;
  inline ::google::protobuf::int32 response_flags() const;
  inline void set_response_flags(::google::protobuf::int32 value);

  // required int64 cursor_id = 3;
  inline bool has_cursor_id() const;
  inline void clear_cursor_id();
  static const int kCursorIdFieldNumber = 3;
  inline ::google::protobuf::int64 cursor_id() const;
  inline void set_cursor_id(::google::protobuf::int64 value);

  // required int32 starting_from = 4;
  inline bool has_starting_from() const;
  inline void clear_starting_from();
  static const int kStartingFromFieldNumber = 4;
  inline ::google::protobuf::int32 starting_from() const;
  inline void set_starting_from(::google::protobuf::int32 value);

  // required int32 number_returned = 5;
  inline bool has_number_returned() const;
  inline void clear_number_returned();
  static const int kNumberReturnedFieldNumber = 5;
  inline ::google::protobuf::int32 number_returned() const;
  inline void set_number_returned(::google::protobuf::int32 value);

  // required string message = 6;
  inline bool has_message() const;
  inline void clear_message();
  static const int kMessageFieldNumber = 6;
  inline const ::std::string& message() const;
  inline void set_message(const ::std::string& value);
  inline void set_message(const char* value);
  inline void set_message(const char* value, size_t size);
  inline ::std::string* mutable_message();
  inline ::std::string* release_message();
  inline void set_allocated_message(::std::string* message);

  // @@protoc_insertion_point(class_scope:brpc.policy.MongoResponse)
 private:
  inline void set_has_header();
  inline void clear_has_header();
  inline void set_has_response_flags();
  inline void clear_has_response_flags();
  inline void set_has_cursor_id();
  inline void clear_has_cursor_id();
  inline void set_has_starting_from();
  inline void clear_has_starting_from();
  inline void set_has_number_returned();
  inline void clear_has_number_returned();
  inline void set_has_message();
  inline void clear_has_message();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::brpc::policy::MongoHeader* header_;
  ::google::protobuf::int64 cursor_id_;
  ::google::protobuf::int32 response_flags_;
  ::google::protobuf::int32 starting_from_;
  ::std::string* message_;
  ::google::protobuf::int32 number_returned_;
  friend void  protobuf_AddDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_AssignDesc_brpc_2fpolicy_2fmongo_2eproto();
  friend void protobuf_ShutdownFile_brpc_2fpolicy_2fmongo_2eproto();

  void InitAsDefaultInstance();
  static MongoResponse* default_instance_;
};
// ===================================================================

class MongoService_Stub;

class MongoService : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline MongoService() {};
 public:
  virtual ~MongoService();

  typedef MongoService_Stub Stub;

  static const ::google::protobuf::ServiceDescriptor* descriptor();

  virtual void default_method(::google::protobuf::RpcController* controller,
                       const ::brpc::policy::MongoRequest* request,
                       ::brpc::policy::MongoResponse* response,
                       ::google::protobuf::Closure* done);

  // implements Service ----------------------------------------------

  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MongoService);
};

class MongoService_Stub : public MongoService {
 public:
  MongoService_Stub(::google::protobuf::RpcChannel* channel);
  MongoService_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~MongoService_Stub();

  inline ::google::protobuf::RpcChannel* channel() { return channel_; }

  // implements MongoService ------------------------------------------

  void default_method(::google::protobuf::RpcController* controller,
                       const ::brpc::policy::MongoRequest* request,
                       ::brpc::policy::MongoResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MongoService_Stub);
};


// ===================================================================


// ===================================================================

// MongoHeader

// required int32 message_length = 1;
inline bool MongoHeader::has_message_length() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MongoHeader::set_has_message_length() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MongoHeader::clear_has_message_length() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MongoHeader::clear_message_length() {
  message_length_ = 0;
  clear_has_message_length();
}
inline ::google::protobuf::int32 MongoHeader::message_length() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoHeader.message_length)
  return message_length_;
}
inline void MongoHeader::set_message_length(::google::protobuf::int32 value) {
  set_has_message_length();
  message_length_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoHeader.message_length)
}

// required int32 request_id = 2;
inline bool MongoHeader::has_request_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MongoHeader::set_has_request_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MongoHeader::clear_has_request_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MongoHeader::clear_request_id() {
  request_id_ = 0;
  clear_has_request_id();
}
inline ::google::protobuf::int32 MongoHeader::request_id() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoHeader.request_id)
  return request_id_;
}
inline void MongoHeader::set_request_id(::google::protobuf::int32 value) {
  set_has_request_id();
  request_id_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoHeader.request_id)
}

// required int32 response_to = 3;
inline bool MongoHeader::has_response_to() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MongoHeader::set_has_response_to() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MongoHeader::clear_has_response_to() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MongoHeader::clear_response_to() {
  response_to_ = 0;
  clear_has_response_to();
}
inline ::google::protobuf::int32 MongoHeader::response_to() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoHeader.response_to)
  return response_to_;
}
inline void MongoHeader::set_response_to(::google::protobuf::int32 value) {
  set_has_response_to();
  response_to_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoHeader.response_to)
}

// required .brpc.policy.MongoOp op_code = 4;
inline bool MongoHeader::has_op_code() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MongoHeader::set_has_op_code() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MongoHeader::clear_has_op_code() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MongoHeader::clear_op_code() {
  op_code_ = 1;
  clear_has_op_code();
}
inline ::brpc::policy::MongoOp MongoHeader::op_code() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoHeader.op_code)
  return static_cast< ::brpc::policy::MongoOp >(op_code_);
}
inline void MongoHeader::set_op_code(::brpc::policy::MongoOp value) {
  assert(::brpc::policy::MongoOp_IsValid(value));
  set_has_op_code();
  op_code_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoHeader.op_code)
}

// -------------------------------------------------------------------

// MongoRequest

// required .brpc.policy.MongoHeader header = 1;
inline bool MongoRequest::has_header() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MongoRequest::set_has_header() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MongoRequest::clear_has_header() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MongoRequest::clear_header() {
  if (header_ != NULL) header_->::brpc::policy::MongoHeader::Clear();
  clear_has_header();
}
inline const ::brpc::policy::MongoHeader& MongoRequest::header() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoRequest.header)
  return header_ != NULL ? *header_ : *default_instance_->header_;
}
inline ::brpc::policy::MongoHeader* MongoRequest::mutable_header() {
  set_has_header();
  if (header_ == NULL) header_ = new ::brpc::policy::MongoHeader;
  // @@protoc_insertion_point(field_mutable:brpc.policy.MongoRequest.header)
  return header_;
}
inline ::brpc::policy::MongoHeader* MongoRequest::release_header() {
  clear_has_header();
  ::brpc::policy::MongoHeader* temp = header_;
  header_ = NULL;
  return temp;
}
inline void MongoRequest::set_allocated_header(::brpc::policy::MongoHeader* header) {
  delete header_;
  header_ = header;
  if (header) {
    set_has_header();
  } else {
    clear_has_header();
  }
  // @@protoc_insertion_point(field_set_allocated:brpc.policy.MongoRequest.header)
}

// required string message = 2;
inline bool MongoRequest::has_message() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MongoRequest::set_has_message() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MongoRequest::clear_has_message() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MongoRequest::clear_message() {
  if (message_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_->clear();
  }
  clear_has_message();
}
inline const ::std::string& MongoRequest::message() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoRequest.message)
  return *message_;
}
inline void MongoRequest::set_message(const ::std::string& value) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(value);
  // @@protoc_insertion_point(field_set:brpc.policy.MongoRequest.message)
}
inline void MongoRequest::set_message(const char* value) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(value);
  // @@protoc_insertion_point(field_set_char:brpc.policy.MongoRequest.message)
}
inline void MongoRequest::set_message(const char* value, size_t size) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:brpc.policy.MongoRequest.message)
}
inline ::std::string* MongoRequest::mutable_message() {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:brpc.policy.MongoRequest.message)
  return message_;
}
inline ::std::string* MongoRequest::release_message() {
  clear_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = message_;
    message_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MongoRequest::set_allocated_message(::std::string* message) {
  if (message_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete message_;
  }
  if (message) {
    set_has_message();
    message_ = message;
  } else {
    clear_has_message();
    message_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:brpc.policy.MongoRequest.message)
}

// -------------------------------------------------------------------

// MongoResponse

// required .brpc.policy.MongoHeader header = 1;
inline bool MongoResponse::has_header() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MongoResponse::set_has_header() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MongoResponse::clear_has_header() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MongoResponse::clear_header() {
  if (header_ != NULL) header_->::brpc::policy::MongoHeader::Clear();
  clear_has_header();
}
inline const ::brpc::policy::MongoHeader& MongoResponse::header() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.header)
  return header_ != NULL ? *header_ : *default_instance_->header_;
}
inline ::brpc::policy::MongoHeader* MongoResponse::mutable_header() {
  set_has_header();
  if (header_ == NULL) header_ = new ::brpc::policy::MongoHeader;
  // @@protoc_insertion_point(field_mutable:brpc.policy.MongoResponse.header)
  return header_;
}
inline ::brpc::policy::MongoHeader* MongoResponse::release_header() {
  clear_has_header();
  ::brpc::policy::MongoHeader* temp = header_;
  header_ = NULL;
  return temp;
}
inline void MongoResponse::set_allocated_header(::brpc::policy::MongoHeader* header) {
  delete header_;
  header_ = header;
  if (header) {
    set_has_header();
  } else {
    clear_has_header();
  }
  // @@protoc_insertion_point(field_set_allocated:brpc.policy.MongoResponse.header)
}

// required int32 response_flags = 2;
inline bool MongoResponse::has_response_flags() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MongoResponse::set_has_response_flags() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MongoResponse::clear_has_response_flags() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MongoResponse::clear_response_flags() {
  response_flags_ = 0;
  clear_has_response_flags();
}
inline ::google::protobuf::int32 MongoResponse::response_flags() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.response_flags)
  return response_flags_;
}
inline void MongoResponse::set_response_flags(::google::protobuf::int32 value) {
  set_has_response_flags();
  response_flags_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoResponse.response_flags)
}

// required int64 cursor_id = 3;
inline bool MongoResponse::has_cursor_id() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MongoResponse::set_has_cursor_id() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MongoResponse::clear_has_cursor_id() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MongoResponse::clear_cursor_id() {
  cursor_id_ = GOOGLE_LONGLONG(0);
  clear_has_cursor_id();
}
inline ::google::protobuf::int64 MongoResponse::cursor_id() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.cursor_id)
  return cursor_id_;
}
inline void MongoResponse::set_cursor_id(::google::protobuf::int64 value) {
  set_has_cursor_id();
  cursor_id_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoResponse.cursor_id)
}

// required int32 starting_from = 4;
inline bool MongoResponse::has_starting_from() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MongoResponse::set_has_starting_from() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MongoResponse::clear_has_starting_from() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MongoResponse::clear_starting_from() {
  starting_from_ = 0;
  clear_has_starting_from();
}
inline ::google::protobuf::int32 MongoResponse::starting_from() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.starting_from)
  return starting_from_;
}
inline void MongoResponse::set_starting_from(::google::protobuf::int32 value) {
  set_has_starting_from();
  starting_from_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoResponse.starting_from)
}

// required int32 number_returned = 5;
inline bool MongoResponse::has_number_returned() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void MongoResponse::set_has_number_returned() {
  _has_bits_[0] |= 0x00000010u;
}
inline void MongoResponse::clear_has_number_returned() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void MongoResponse::clear_number_returned() {
  number_returned_ = 0;
  clear_has_number_returned();
}
inline ::google::protobuf::int32 MongoResponse::number_returned() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.number_returned)
  return number_returned_;
}
inline void MongoResponse::set_number_returned(::google::protobuf::int32 value) {
  set_has_number_returned();
  number_returned_ = value;
  // @@protoc_insertion_point(field_set:brpc.policy.MongoResponse.number_returned)
}

// required string message = 6;
inline bool MongoResponse::has_message() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void MongoResponse::set_has_message() {
  _has_bits_[0] |= 0x00000020u;
}
inline void MongoResponse::clear_has_message() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void MongoResponse::clear_message() {
  if (message_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_->clear();
  }
  clear_has_message();
}
inline const ::std::string& MongoResponse::message() const {
  // @@protoc_insertion_point(field_get:brpc.policy.MongoResponse.message)
  return *message_;
}
inline void MongoResponse::set_message(const ::std::string& value) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(value);
  // @@protoc_insertion_point(field_set:brpc.policy.MongoResponse.message)
}
inline void MongoResponse::set_message(const char* value) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(value);
  // @@protoc_insertion_point(field_set_char:brpc.policy.MongoResponse.message)
}
inline void MongoResponse::set_message(const char* value, size_t size) {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  message_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:brpc.policy.MongoResponse.message)
}
inline ::std::string* MongoResponse::mutable_message() {
  set_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    message_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:brpc.policy.MongoResponse.message)
  return message_;
}
inline ::std::string* MongoResponse::release_message() {
  clear_has_message();
  if (message_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = message_;
    message_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MongoResponse::set_allocated_message(::std::string* message) {
  if (message_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete message_;
  }
  if (message) {
    set_has_message();
    message_ = message;
  } else {
    clear_has_message();
    message_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:brpc.policy.MongoResponse.message)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace policy
}  // namespace brpc

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::brpc::policy::MongoOp> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::policy::MongoOp>() {
  return ::brpc::policy::MongoOp_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_brpc_2fpolicy_2fmongo_2eproto__INCLUDED
