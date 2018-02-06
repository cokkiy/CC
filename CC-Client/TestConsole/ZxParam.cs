using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace TestConsole
{
    public enum TransType : byte
    {
        Char = 0x00,
        Byte,
        Short,
        Word,
        Long,
        DWord,
        Float,
        Double,
        PMTime,
        Date,
        String,
        Code,
        BCDTime,
        UTCTime,
        ULong8,
        Long8,
        Longe,
        IvalidType = 0xff
    }
    [XmlType(TypeName = "Parameter")]
    public class ZxParam
    {
        public ushort TableIndex { get; set; }
        public ushort ParamIndex { get; set; }
        public string ParamName { get; set; }
        [XmlElement("ParamCode")]
        public string ParamCode { get; set; }
        [XmlElement("TransType")]
        public string TransTypeNode { get; set; }

        [XmlElement("DataLength")]
        public ushort DataLength { get; set; }
        [XmlElement("ConvType")]
        public string ChangeTypeNode { get; set; }

        [XmlElement("LowerLimit")]
        public double LowLimit { get; set; }

        [XmlElement("UpperLimit")]
        public double UpLimit { get; set; }

        /// <summary>
        /// 比例系数
        /// </summary>
        [XmlElement("Dimension")]
        public double Factor { get; set; }

        [XmlElement("ParamUnit")]
        public string Unit { get; set; }
        [XmlElement("DisplayType")]
        public string DisplayType { get; set; }
        [XmlElement("TheoryValue")]
        public string TheoryValue { get; set; }
        [XmlElement("TheoryValueRange")]
        public string TheoryValueRange { get; set; }
        [XmlElement("GrpIndex")]
        public uint GroupIndex { get; set; }
        [XmlElement("Remark")]
        public string Remark { get; set; }

        public TransType TransType
        {
            get
            {
                TransType t = TransType.IvalidType;
                if (!Enum.TryParse(TransTypeNode, true, out t))
                {
                    t = TransType.IvalidType;
                }
                return t;
            }
        }


        public TransType ChangeType
        {
            get
            {
                TransType t = TransType.IvalidType;
                if (!Enum.TryParse(ChangeTypeNode, true, out t))
                {
                    t = TransType.IvalidType;
                }
                return t;                
            }
        }

        public byte[] ToBytes()
        {
            byte[] result = new byte[1821];
            int index = 0;
            byte[] bytes = BitConverter.GetBytes(TableIndex);
            Array.Copy(bytes, result, 2);
            index += 2;
            bytes = BitConverter.GetBytes(ParamIndex);
            Array.Copy(bytes, 0, result, index, 2);
            index += 2;

            int length = 0;
            if (!string.IsNullOrEmpty(ParamName))
            {
                bytes = Encoding.UTF8.GetBytes(ParamName);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;


            if (!string.IsNullOrEmpty(ParamCode))
            {
                bytes = Encoding.UTF8.GetBytes(ParamCode);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);                
            }
            index += 255;


            bytes = BitConverter.GetBytes((byte)TransType);
            Array.Copy(bytes, 0, result, index, 1);
            index += 1;

            bytes = BitConverter.GetBytes(DataLength);
            Array.Copy(bytes, 0, result, index, 2);
            index += 2;

            bytes = BitConverter.GetBytes((byte)ChangeType);
            Array.Copy(bytes, 0, result, index, 1);
            index += 1;
            

            bytes = BitConverter.GetBytes(LowLimit);
            Array.Copy(bytes, 0, result, index, 8);
            index += 8;

            bytes = BitConverter.GetBytes(UpLimit);
            Array.Copy(bytes, 0, result, index, 8);
            index += 8;

            bytes = BitConverter.GetBytes(Factor);
            Array.Copy(bytes, 0, result, index, 8);
            index += 8;

            if (!string.IsNullOrEmpty(Unit))
            {
                bytes = Encoding.UTF8.GetBytes(Unit);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;

            if (!string.IsNullOrEmpty(DisplayType))
            {
                bytes = Encoding.UTF8.GetBytes(DisplayType);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;

            if (!string.IsNullOrEmpty(TheoryValue))
            {
                bytes = Encoding.UTF8.GetBytes(TheoryValue);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;

            if (!string.IsNullOrEmpty(TheoryValueRange))
            {
                bytes = Encoding.UTF8.GetBytes(TheoryValueRange);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;


            bytes = BitConverter.GetBytes(GroupIndex);
            Array.Copy(bytes, 0, result, index, 4);
            index += 4;

            if (!string.IsNullOrEmpty(Remark))
            {
                bytes = Encoding.UTF8.GetBytes(Remark);
                length = bytes.Length < 255 ? bytes.Length : 255;
                Array.Copy(bytes, 0, result, index, length);
            }
            index += 255;

            return result;
        }
    }
    
    [XmlType(Namespace ="",TypeName = "ParamAppoInfo")]
    public class ZxParams
    {       
        public List<ZxParam> Params { get; set; }
    }
}
