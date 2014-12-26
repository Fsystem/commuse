using System;
using System.Text;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;


/// <summary>
/// 
/// </summary>
///  

class ManagerSocket
{
    class Sendinfo
    {
        public ManagerSocket manager = null;
        public string  sockinfo = null;
        public byte[] arr = null;
        public int nBytes = 0;
    }

    private Dictionary<string,TcpSocketAny> Sock_List = new Dictionary<string,TcpSocketAny>();

    public bool NewConnect(string sockInfo , string strIp , int nPort )
    {
        bool bRes =false;
        if (null==sockInfo)
        {
            return bRes;
        }

        TcpSocketAny any = new TcpSocketAny();
        any.Connect(strIp,nPort);
        while(!any.isConnected){};
        if( !Sock_List.ContainsKey(sockInfo) )
        {
            Sock_List.Add(sockInfo,any);
            ThreadPool.QueueUserWorkItem(RecvFunc, sockInfo);//启动接受检测线程
            bRes = true;
        }
        
        return bRes;
        
    }

    public void CloseConnect(string sockInfo)
    {
        
        if (Sock_List.ContainsKey(sockInfo))
        {
            TcpSocketAny any  = null;
            Sock_List.TryGetValue(sockInfo,out any);
            if ( null != any)
            {
                any.Close();
            }
            
            Sock_List.Remove(sockInfo);
        }
    }

    public bool IsConnected(string sockInfo)
    {
        bool bRes = false;
        TcpSocketAny any = null;
        Sock_List.TryGetValue(sockInfo, out any);
        if ( null != any)
        {
            bRes = any.isConnected;
        }
        return bRes;
    }

    public void Send(string sockInfo,byte[] datagram, int nSize)
    {
        Sendinfo sinfo = new Sendinfo();
        sinfo.manager = this;
        sinfo.sockinfo = sockInfo;
        sinfo.arr = datagram;
        sinfo.nBytes = nSize;

        ThreadPool.QueueUserWorkItem(SendFunc, sinfo);
    }

    private static void SendFunc(object obj)
    {
        Sendinfo send = (Sendinfo)obj;
        if (null != send.manager && null != send.arr && 0 != send.nBytes )
        {
            TcpSocketAny any = null;
            send.manager.Sock_List.TryGetValue(send.sockinfo, out any);
            if (null != any)
            {
                any.Send(send.arr, send.nBytes);
            }
        }
        
    }

    private static void RecvFunc(object obj)
    {
        string sockinfo = (string)obj;
        protoPacket pkRecv = null;
        while(true)
        {
            //通过obj可以知道是哪个sock接收到消息了

            pkRecv = GloabVariable.ProtoPacket.PopPk();
            if ( null != pkRecv)
            {
                GloabVariable.DisMessage.OnMessage(pkRecv.arr, pkRecv.nBytes);
            }

            Thread.Sleep(1);
        }
        
    }
}

class TcpSocketAny
{
    public enum EncodingType
    {
        Default = 0,
        Unicode,
        UTF8,
        ASCII,
    }

    public EncodingType encType = EncodingType.Default;

    private Socket sockClient;

    /// <summary>
    /// 接收数据缓冲区大小64K
    /// </summary>
    public const int DefaultBufferSize = 64 * 1024;
    /// <summary>
    /// 接收数据缓冲区
    /// </summary>
    private byte[] _recvDataBuffer = new byte[DefaultBufferSize];


    public bool isConnected;

    /// <summary>
    /// 
    /// </summary>
    public TcpSocketAny()
    {
    }

    /// <summary>
    /// 发送数据报文
    /// </summary>
    /// <param name="datagram"></param>
    public void Send(byte[] datagram,int nSize)
    {
        try
        {
            if (nSize == 0)
            {
                return;
            }
            if (!isConnected)
            {
                throw (new ApplicationException("没有连接服务器，不能发送数据"));
            }
            //获得报文的编码字节

            sockClient.BeginSend( datagram, 0, nSize, SocketFlags.None,
                new AsyncCallback(SendCallBack), sockClient);
        }
        catch (System.Exception )
        {
        	
        }
        
    }
    /// <summary>
    /// 关闭连接
    /// </summary>
    public void Close()
    {
        if (!isConnected)
        {
            return;
        }
        isConnected = false;

        sockClient.Disconnect(false);
        //关闭数据的接受和发送
        sockClient.Shutdown(SocketShutdown.Both);
        //清理资源
        sockClient.Close();
        


    }


    /// <summary>
    /// 数据发送完成处理函数
    /// </summary>
    /// <param name="iar"></param>
    protected virtual void SendCallBack(IAsyncResult iar)
    {
        try
        {
            Socket remote = (Socket)iar.AsyncState;
            int sent = remote.EndSend(iar);
        }
        catch (System.Exception )
        {
        	
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="strIP"></param>
    /// <param name="nPort"></param>
    /// <returns></returns>
    public bool Connect(string strIP, int nPort)
    {
        bool bRes = false;
        try
        {
            sockClient = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(strIP), nPort);

            sockClient.BeginConnect(endPoint, new AsyncCallback(Connected), sockClient );


        }
        catch (Exception)
        {
            bRes = false;
        }

        return bRes;
    }

    protected virtual void Connected(IAsyncResult iar)
    {
        try
        {
            Socket socket = (Socket)iar.AsyncState;
            socket.EndConnect(iar);
            //建立连接后应该立即接收数据
            sockClient.BeginReceive(_recvDataBuffer, 0,
            DefaultBufferSize, SocketFlags.None,
            new AsyncCallback(RecvCallBack), socket);
            isConnected = true;
        }
        catch (System.Exception )
        {
        	
        }
       
    }


    /// <summary>
    /// 数据接收处理函数
    /// </summary>
    /// <param name="iar">异步Socket</param>
    protected void RecvCallBack(IAsyncResult iar)
    {
        Socket remote = (Socket)iar.AsyncState;
        try
        {
            int recv = remote.EndReceive(iar);
            //正常的退出
            if (recv == 0)
            {
                Close();
                return;
            }
            //检查数据
            byte[] arrRecv = CheckPacket.Check(_recvDataBuffer, recv);
            if (null != arrRecv)
            {
                //接受到的数据放入接受容器中
                GloabVariable.ProtoPacket.AddPk(arrRecv, arrRecv.Length);
            }

            //继续接受数据
            sockClient.BeginReceive(_recvDataBuffer, 0, DefaultBufferSize, SocketFlags.None,
            new AsyncCallback(RecvCallBack), sockClient);
        }
        catch (SocketException ex)
        {
            Close();
            //客户端退出
            if ( 10054 == ex.ErrorCode )
            {
                //Close();
            }
            else
            {
                //throw (ex);
            }
        }
        catch (ObjectDisposedException ex)
        {
            //这里的实现不够优雅
            //当调用CloseSession()时,会结束数据接收,但是数据接收
            //处理中会调用int recv = client.EndReceive(iar);
            //就访问了CloseSession()已经处置的对象
            //我想这样的实现方法也是无伤大雅的.
            if (ex != null)
            {
                ex = null;
                //DoNothing;
            }
        }
    }


    /// <summary>
    /// 通讯数据解码
    /// </summary>
    /// <param name="dataBytes">需要解码的数据</param>
    /// <returns>编码后的数据</returns>
    public string GetEncodingString(byte[] dataBytes, int size)
    {
        switch (encType)
        {
            case EncodingType.Default:
                {
                    return Encoding.Default.GetString(dataBytes, 0, size);
                }
            case EncodingType.Unicode:
                {
                    return Encoding.Unicode.GetString(dataBytes, 0, size);
                }
            case EncodingType.UTF8:
                {
                    return Encoding.UTF8.GetString(dataBytes, 0, size);
                }
            case EncodingType.ASCII:
                {
                    return Encoding.ASCII.GetString(dataBytes, 0, size);
                }
            default:
                {
                    throw (new Exception("未定义的编码格式"));
                }
        }
    }
    /// <summary>
    /// 数据编码
    /// </summary>
    /// <param name="datagram">需要编码的报文</param>
    /// <returns>编码后的数据</returns>
    public byte[] GetEncodingBytes(string datagram)
    {
        switch (encType)
        {
            case EncodingType.Default:
                {
                    return Encoding.Default.GetBytes(datagram);
                }
            case EncodingType.Unicode:
                {
                    return Encoding.Unicode.GetBytes(datagram);
                }
            case EncodingType.UTF8:
                {
                    return Encoding.UTF8.GetBytes(datagram);
                }
            case EncodingType.ASCII:
                {
                    return Encoding.ASCII.GetBytes(datagram);
                }
            default:
                {
                    throw (new Exception("未定义的编码格式"));
                }
        }
    }
}

