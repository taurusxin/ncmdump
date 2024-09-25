using System;
using System.Threading.Tasks;

namespace libncmdump_demo_cli
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // 文件名
            string filePath = "test.ncm";

            // 创建 NeteaseCrypt 类的实例
            NeteaseCrypt neteaseCrypt = new NeteaseCrypt(filePath);

            // 启动转换过程
            int result = neteaseCrypt.Dump("");  // 为空则输出到源

            // 修复元数据
            neteaseCrypt.FixMetadata();

            // [务必]销毁 NeteaseCrypt 类的实例
            neteaseCrypt.Destroy();
        }
    }
}
