// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "cmd.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::EnumDescriptor* CmdType_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* MainCmd_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* SubCmdLogin_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* SubCmdWorld_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* SubCmdGame_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_cmd_2eproto() {
  protobuf_AddDesc_cmd_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "cmd.proto");
  GOOGLE_CHECK(file != NULL);
  CmdType_descriptor_ = file->enum_type(0);
  MainCmd_descriptor_ = file->enum_type(1);
  SubCmdLogin_descriptor_ = file->enum_type(2);
  SubCmdWorld_descriptor_ = file->enum_type(3);
  SubCmdGame_descriptor_ = file->enum_type(4);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_cmd_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_cmd_2eproto() {
}

void protobuf_AddDesc_cmd_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\tcmd.proto*\032\n\007CmdType\022\017\n\013INVALID_CMD\020\000*"
    "2\n\007MainCmd\022\014\n\010GM_LOGIN\020\001\022\014\n\010GM_WORLD\020\002\022\013"
    "\n\007GM_GAME\020\003*@\n\013SubCmdLogin\022\r\n\tSUB_LOGIN\020"
    "\001\022\017\n\013SUB_LOGINOK\020\002\022\021\n\rSUB_LOGINFAID\020\002*I\n"
    "\013SubCmdWorld\022\020\n\014SUB_USERINFO\020\001\022\022\n\016SUB_US"
    "ERINFOOK\020\002\022\024\n\020SUB_USERINFOFAID\020\003*\033\n\nSubC"
    "mdGame\022\r\n\tSUB_SHOOT\020\001", 261);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "cmd.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_cmd_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_cmd_2eproto {
  StaticDescriptorInitializer_cmd_2eproto() {
    protobuf_AddDesc_cmd_2eproto();
  }
} static_descriptor_initializer_cmd_2eproto_;

const ::google::protobuf::EnumDescriptor* CmdType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CmdType_descriptor_;
}
bool CmdType_IsValid(int value) {
  switch(value) {
    case 0:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* MainCmd_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MainCmd_descriptor_;
}
bool MainCmd_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* SubCmdLogin_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SubCmdLogin_descriptor_;
}
bool SubCmdLogin_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* SubCmdWorld_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SubCmdWorld_descriptor_;
}
bool SubCmdWorld_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* SubCmdGame_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SubCmdGame_descriptor_;
}
bool SubCmdGame_IsValid(int value) {
  switch(value) {
    case 1:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)