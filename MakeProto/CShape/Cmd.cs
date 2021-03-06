// Generated by ProtoGen, Version=2.3.0.277, Culture=neutral, PublicKeyToken=8fd7408b07f8d2cd.  DO NOT EDIT!

using pb = global::Google.ProtocolBuffers;
using pbc = global::Google.ProtocolBuffers.Collections;
using pbd = global::Google.ProtocolBuffers.Descriptors;
using scg = global::System.Collections.Generic;
public static partial class Cmd {

  #region Extension registration
  public static void RegisterAllExtensions(pb::ExtensionRegistry registry) {
  }
  #endregion
  #region Static variables
  #endregion
  #region Descriptor
  public static pbd::FileDescriptor Descriptor {
    get { return descriptor; }
  }
  private static pbd::FileDescriptor descriptor;
  
  static Cmd() {
    byte[] descriptorData = global::System.Convert.FromBase64String(
        "CgljbWQucHJvdG8qGgoHQ21kVHlwZRIPCgtJTlZBTElEX0NNRBAAKjIKB01h" + 
        "aW5DbWQSDAoIR01fTE9HSU4QARIMCghHTV9XT1JMRBACEgsKB0dNX0dBTUUQ" + 
        "AypACgtTdWJDbWRMb2dpbhINCglTVUJfTE9HSU4QARIPCgtTVUJfTE9HSU5P" + 
        "SxACEhEKDVNVQl9MT0dJTkZBSUQQAipJCgtTdWJDbWRXb3JsZBIQCgxTVUJf" + 
        "VVNFUklORk8QARISCg5TVUJfVVNFUklORk9PSxACEhQKEFNVQl9VU0VSSU5G" + 
        "T0ZBSUQQAyobCgpTdWJDbWRHYW1lEg0KCVNVQl9TSE9PVBAB");
    pbd::FileDescriptor.InternalDescriptorAssigner assigner = delegate(pbd::FileDescriptor root) {
      descriptor = root;
      return null;
    };
    pbd::FileDescriptor.InternalBuildGeneratedFileFrom(descriptorData,
        new pbd::FileDescriptor[] {
        }, assigner);
  }
  #endregion
  
}
#region Enums
public enum CmdType {
  INVALID_CMD = 0,
}

public enum MainCmd {
  GM_LOGIN = 1,
  GM_WORLD = 2,
  GM_GAME = 3,
}

public enum SubCmdLogin {
  SUB_LOGIN = 1,
  SUB_LOGINOK = 2,
  SUB_LOGINFAID = 2,
}

public enum SubCmdWorld {
  SUB_USERINFO = 1,
  SUB_USERINFOOK = 2,
  SUB_USERINFOFAID = 3,
}

public enum SubCmdGame {
  SUB_SHOOT = 1,
}

#endregion

