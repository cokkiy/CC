using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LogViewerWeb.Services
{
    public class ImageService
    {
        /// <summary>
        /// 图片格式类型
        /// </summary>
        public enum ImageFormat
        {
            bmp,
            jpeg,
            gif,
            tiff,
            png,
            unknown
        }

        /// <summary>
        /// 获取指定文件的图片类型
        /// </summary>
        /// <param name="bytes">图片文件内容</param>
        /// <returns>文件类型，<see cref="ImageFormat"/></returns>
        public static ImageFormat GetImageFormat(byte[] bytes)
        {
            if (bytes == null)
                return ImageFormat.unknown;

            // see http://www.mikekunz.com/image_file_header.html  
            var bmp = Encoding.ASCII.GetBytes("BM");     // BMP
            var gif = Encoding.ASCII.GetBytes("GIF");    // GIF
            var png = new byte[] { 137, 80, 78, 71 };    // PNG
            var tiff = new byte[] { 73, 73, 42 };         // TIFF
            var tiff2 = new byte[] { 77, 77, 42 };         // TIFF
            var jpeg = new byte[] { 255, 216, 255, 224 }; // jpeg
            var jpeg2 = new byte[] { 255, 216, 255, 225 }; // jpeg canon
            if (bmp.SequenceEqual(bytes.Take(bmp.Length)))
                return ImageFormat.bmp;
            if (gif.SequenceEqual(bytes.Take(gif.Length)))
                return ImageFormat.gif;
            if (png.SequenceEqual(bytes.Take(png.Length)))
                return ImageFormat.png;
            if (tiff.SequenceEqual(bytes.Take(tiff.Length)))
                return ImageFormat.tiff;
            if (tiff2.SequenceEqual(bytes.Take(tiff2.Length)))
                return ImageFormat.tiff;
            if (jpeg.SequenceEqual(bytes.Take(jpeg.Length)))
                return ImageFormat.jpeg;
            if (jpeg2.SequenceEqual(bytes.Take(jpeg2.Length)))
                return ImageFormat.jpeg;
            return ImageFormat.unknown;
        }


        /// <summary>
        /// 从byte[]类型的图片文件中获取base64编码的
        /// 内嵌式图片字符串
        /// </summary>
        /// <param name="imgContents">原始图片文件内容</param>
        /// <returns>符合html标准的内嵌式image字符串</returns>
        public static string GetEmbedImageSrcString(byte[] imgContents)
        {
            if (imgContents == null)
                return "#";

            ImageFormat format = GetImageFormat(imgContents);
            string prefix = "data:image/{0};base64,{1}";
            string src = Convert.ToBase64String(imgContents);
            string result = null;
            switch (format)
            {
                case ImageFormat.bmp:
                    result = string.Format(prefix, "bmp", src);
                    break;
                case ImageFormat.gif:
                    result = string.Format(prefix, "gif", src);
                    break;
                case ImageFormat.jpeg:
                    result = string.Format(prefix, "jpg", src);
                    break;
                case ImageFormat.png:
                    result = string.Format(prefix, "png", src);
                    break;
                case ImageFormat.tiff:
                    result = string.Format(prefix, "tiff", src);
                    break;
                case ImageFormat.unknown:
                    result = string.Format(prefix, "jpg", src);
                    break;
            }

            return result;
        } 
    }
}
