using System;
using System.Runtime.InteropServices;

namespace libncmdump_demo_cli
{
    /// <summary>
    /// NeteaseCrypt C# Wrapper
    /// </summary>
    class NeteaseCrypt
    {
        const string DLL_PATH = "libncmdump.dll";

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateNeteaseCrypt(IntPtr path);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        private static extern int Dump(IntPtr NeteaseCrypt, IntPtr outputPath);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FixMetadata(IntPtr NeteaseCrypt);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyNeteaseCrypt(IntPtr NeteaseCrypt);

        private IntPtr NeteaseCryptClass = IntPtr.Zero;

        /// <summary>
        /// 创建 NeteaseCrypt 类的实例。
        /// </summary>
        /// <param name="FileName">网易云音乐 ncm 加密文件路径</param>
        public NeteaseCrypt(string FileName)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(FileName);

            IntPtr inputPtr = Marshal.AllocHGlobal(bytes.Length + 1);
            Marshal.Copy(bytes, 0, inputPtr, bytes.Length);
            Marshal.WriteByte(inputPtr, bytes.Length, 0);

            NeteaseCryptClass = CreateNeteaseCrypt(inputPtr);
        }

        /// <summary>
        /// 启动转换过程。
        /// </summary>
        /// <param name="OutputPath">指定一个路径输出，如果为空，则输出到原路径</param>
        /// <returns>返回一个整数，指示转储过程的结果。如果成功，返回0；如果失败，返回1。</returns>
        public int Dump(string OutputPath)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(OutputPath);

            IntPtr outputPtr = Marshal.AllocHGlobal(bytes.Length + 1);
            Marshal.Copy(bytes, 0, outputPtr, bytes.Length);
            Marshal.WriteByte(outputPtr, bytes.Length, 0);

            return Dump(NeteaseCryptClass, outputPtr);
        }

        /// <summary>
        /// 修复音乐文件元数据。
        /// </summary>
        public void FixMetadata()
        {
            FixMetadata(NeteaseCryptClass);
        }

        /// <summary>
        /// 销毁 NeteaseCrypt 类的实例。
        /// </summary>
        public void Destroy()
        {
            DestroyNeteaseCrypt(NeteaseCryptClass);
        }
    }
}
