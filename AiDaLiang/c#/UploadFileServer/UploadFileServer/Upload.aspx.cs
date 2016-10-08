using System;
using System.Collections.Generic;
using System.Collections;
//using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.IO;

namespace UploadFileServer
{
    public partial class Upload : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            string type = "";
            string Re = "";
            Re += "数据传送方式：";
            if (Request.RequestType.ToUpper() == "POST")
            {
                type = "POST";
                Re += type + "<br/>参数分别是：<br/>";
                SortedList table = Param();
                if (table != null)
                {
                    foreach (DictionaryEntry De in table) { 
                        Re += "参数[：" + De.Key + " ：" + De.Value + "]<br/>";

                        DoSaveFile(De.Key.ToString(), De.Value.ToString());
                    }

                }
                else
                { Re = "你没有传递任何参数过来！"; }
            }
            else if (Request.RequestType.ToUpper() == "GET")
            {
                type = "GET";
                Re += type + "<br/>参数分别是：<br/>";
                System.Collections.Specialized.NameValueCollection nvc = GETInput();
                if (nvc.Count != 0)
                {
                    for (int i = 0; i < nvc.Count; i++) { 
                        Re += "参数：[" + nvc.GetKey(i) + " ：" + nvc.GetValues(i)[0] + "]<br/>";

                        DoSaveFile(nvc.GetKey(i), nvc.GetValues(i)[0]);
                    }

                   
                }
                else
                { Re = "你没有传递任何参数过来！"; }
            }

            Response.Write(Re);
        }

        //获取GET返回来的数据
        private System.Collections.Specialized.NameValueCollection GETInput()
        { return Request.QueryString; }
        // 获取POST返回来的数据
        private string PostInput()
        {
            try
            {
                System.IO.Stream s = Request.InputStream;
                int count = 0;
                byte[] buffer = new byte[1024];
                System.Text.StringBuilder builder = new System.Text.StringBuilder();
                while ((count = s.Read(buffer, 0, 1024)) > 0)
                {
                    builder.Append(System.Text.Encoding.UTF8.GetString(buffer, 0, count));
                }
                s.Flush();
                s.Close();
                s.Dispose();
                return builder.ToString();
            }
            catch (Exception ex)
            { throw ex; }
        }

        private SortedList Param()
        {
            string POSTStr = PostInput();
            SortedList SortList = new SortedList();
            int index = POSTStr.IndexOf("&");
            string[] Arr = { };
            if (index != -1) //参数传递不只一项
            {
                Arr = POSTStr.Split('&');
                for (int i = 0; i < Arr.Length; i++)
                {
                    int equalindex = Arr[i].IndexOf('=');
                    string paramN = Arr[i].Substring(0, equalindex);
                    string paramV = Arr[i].Substring(equalindex + 1);
                    if (!SortList.ContainsKey(paramN)) //避免用户传递相同参数
                    { SortList.Add(paramN, paramV); }
                    else //如果有相同的，一直删除取最后一个值为准
                    { SortList.Remove(paramN); SortList.Add(paramN, paramV); }
                }
            }
            else //参数少于或等于1项
            {
                int equalindex = POSTStr.IndexOf('=');
                if (equalindex != -1)
                { //参数是1项
                    string paramN = POSTStr.Substring(0, equalindex);
                    string paramV = POSTStr.Substring(equalindex + 1);
                    SortList.Add(paramN, paramV);

                }
                else //没有传递参数过来
                { SortList = null; }
            }
            return SortList;
        }


        //图片转为base64编码的字符串
        protected string ImgToBase64String(string Imagefilename)
        {
            try
            {
                System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(Imagefilename);

                System.IO.MemoryStream ms = new System.IO.MemoryStream();
                bmp.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                byte[] arr = new byte[ms.Length];
                ms.Position = 0;
                ms.Read(arr, 0, (int)ms.Length);
                ms.Close();
                return Convert.ToBase64String(arr);
            }
            catch (Exception ex)
            {
                return null;
            }
        }

        //base64编码的字符串转为图片
        protected byte[] Base64StringToImage(string strbase64)
        {
            try
            {
                byte[] arr = Convert.FromBase64String(strbase64);
                return arr;
                //System.IO.MemoryStream ms = new System.IO.MemoryStream(arr);
               // System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(ms);

                //bmp.Save("test.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
                //bmp.Save("test.bmp", ImageFormat.Bmp);
                //bmp.Save("test.gif", ImageFormat.Gif);
                //bmp.Save("test.png", ImageFormat.Png);
                //ms.Close();
                //return bmp;
            }
            catch (Exception ex)
            {
                return null;
            }
        }

        protected void DoSaveFile(string key,string base64Files)
        {
            System.DateTime now = System.DateTime.Now;
            string str = Server.MapPath("");//System.Environment.CurrentDirectory;
            str += "\\"+now.Year + "-" + now.Month + "-" + now.Day;
            Response.Write(str);

            if (System.IO.Directory.Exists(str) == false)
            {
                System.IO.Directory.CreateDirectory(str);

                
            }

            string sFilePath = str + "\\" + key + ".jpg";
            //using ()
            {
                try
                {
                    byte[] bmp = Base64StringToImage(base64Files);
                    using (MemoryStream ms = new MemoryStream(bmp))
                    {
                        using (FileStream fs = new FileStream(sFilePath, FileMode.Create, FileAccess.Write))
                        {
                            ms.WriteTo(fs);
                        }
                    }
                }
                catch (Exception ex)
                {
                    Response.Write(ex.Message);
                }
                
//                 if (bmp != null)
//                 {
//                     try
//                     {
//                         bmp.Save(sFilePath, System.Drawing.Imaging.ImageFormat.Jpeg);
//                     }
//                     catch (Exception ex)
//                     {
//                         Response.Write(ex.Message);
//                     }
// 
//                 }
            }
            
            //System.IO.Directory.CreateDirectory()
        }
    }

   
}