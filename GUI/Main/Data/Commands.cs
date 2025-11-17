using System.Collections.Generic;
using System.Linq;
using System.Net;
using System;
using System.Net.NetworkInformation;
using System.Collections;
using System.Text;
using System.ComponentModel;

namespace Data
{
    public static class EnumExtensions
    {
        public static string ToName(this Enum value)
        {
            var type = value.GetType();
            var memberInfo = type.GetMember(value.ToString());
            var attributes = memberInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);
            if (attributes.Count() == 0)
                return value.ToString();
            var attr = ((DescriptionAttribute)attributes[0]);
            return attr == null ? value.ToString() : attr.Description;
        }
    }

    public enum OpCodeTable : byte
    {
        GET_STATUS = 0x74,
        STATUS = 0x75,
        GET_PDU_INFO = 0x76,
        PDU_INFO = 0x77,
        SET_OUTPUT = 0x80,
        OUTPUT_ACK = 0x81,
        TRIP_RESET = 0x82,
        TRIP_RESET_ACK =0x83,
        RESET = 0x84,
        RESET_ACK = 0x85,
        SET_BATTLE = 0x86,
        BATTLE_ACK = 0x87,

        SET_LIMIT = 0x90,
        SET_LIMIT_ACK = 0x91,
        SET_GROUP = 0x92,
        GROUP_ACK = 0x93,
        SET_POR_MACRO = 0x94,
        POR_ACK = 0x95,
        SET_DCI_MACRO = 0x96,
        DCI_ACK = 0x97,
        SET_IP_MODE = 0x98,
        IP_MODE_ACK = 0x99,
        GET_CONFIG = 0x9A,
        CONFIG = 0x9B,
        ERASE_DATA = 0x9C,
        ERASE_ACK = 0x9D,

        KEEPALIVE = 0x5A,

        SET_DEBUG = 0xB0,
        DEBUG_ACK = 0xB1,

        GET_SW_INFO = 0xCA,
        SW_INFO = 0xCB,
        SET_SW_UPDATE = 0xCC,
        SW_UPDATE_ACK = 0xCD,
        CONTROL_OPCODE_ERR = 0xFF,
    }

    public enum Trip_Stat_Item : byte
    {
        [DescriptionAttribute("OK")]
        NO_ERROR = 0x00,

        [DescriptionAttribute("Over Load")]
        TRIP_OL = 0x01,

        [DescriptionAttribute("Short Circuit")]
        TRIP_SC = 0x02,

        NA = 0xFF
    }

    public enum Valid_Type { OK = 0, Fail = 1, EEprom =2, UNKNOWN = 3};
    public class Message : IDisposable
    {
        protected readonly int NUM_OF_CB = 16;
        protected readonly int NUM_OF_GROUPS = 8;
        protected readonly int NUM_OF_CB_BYTE; 
        private List<byte> buffer;
        public Valid_Type Valid;
        
        protected ulong conv(byte[] arr, int index)
        {
            if (NUM_OF_CB <= 8)
                return arr[index];
            if (NUM_OF_CB <= 16)
                return BitConverter.ToUInt16(arr, index);
            if (NUM_OF_CB <= 32)
                return BitConverter.ToUInt32(arr, index);
            if (NUM_OF_CB <= 64)
                return BitConverter.ToUInt64(arr, index);
            throw new WrongParse("Wrong number of CBs", this);
        }

        public OpCodeTable OpCode
        {
            get
            {
                return (OpCodeTable)buffer[0];
            }
            set
            {
                buffer[0] = (byte)value;
            }
        }
        public int Length
        {
            get
            {
                return BitConverter.ToUInt16(buffer.Skip(1).Take(2).Reverse().ToArray(), 0);
            }
            set
            {
                buffer.RemoveRange(1, 2);
                buffer.InsertRange(1,BitConverter.GetBytes((short)(value)).Reverse());
            }
        }

        public byte[] Body
        {
            get
            {
                return buffer.Skip(3).Take(Length - 3).ToArray();
            }
            set
            {
                buffer.RemoveRange(3, buffer.Count - 3);
                buffer.AddRange(value);
                buffer.AddRange(new byte[buffer.Count % 4]);
            }
        }

        public byte[] ToByteArray()
        {
            return buffer.ToArray();
        }

        public delegate void SendInteface_Type(byte[] message,IPAddress IP);
        public static SendInteface_Type SendInterface = null;

        // Flag: Has Dispose already been called?
        bool disposed = false;
        // Public implementation of Dispose pattern callable by consumers.
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        // Protected implementation of Dispose pattern.
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
                return;

            if (disposing)
            {
               
            }

            disposed = true;
        }

        //public event PropertyChangedEventHandler PropertyChanged;

        public void Send(PDU dev)
        {
            SendInterface?.Invoke(this.ToByteArray(),dev.IP);
            Valid = Valid_Type.UNKNOWN;
        }

        public void Get(PDU dev)
        {
            SendInterface?.Invoke(new Message(this.OpCode -1).ToByteArray(),dev.IP);
            Valid = Valid_Type.UNKNOWN;
        }

        public Message()
        {
            NUM_OF_CB_BYTE = (int)Math.Ceiling( NUM_OF_CB / 8.0);
            buffer = new byte[3].ToList();
            Valid = Valid_Type.UNKNOWN ;
        }

        public Message(Message msg)
        {
            NUM_OF_CB_BYTE = (int)Math.Ceiling(NUM_OF_CB / 8.0);
            buffer = msg.buffer;
            Valid = Valid_Type.OK;
        }

        public Message(OpCodeTable CmdOp) : this()
        {
            OpCode = CmdOp;
            Length = 3;
            Valid = Valid_Type.UNKNOWN;
        }

        public Message(byte[] RawData) : this()
        {
            if (RawData.Length < 3)
            {

                return;
            }
            buffer = RawData.ToList();
            if (buffer.Count < 3 || !Enum.IsDefined(typeof(OpCodeTable), buffer[0]))
            {
                //OpCode = OpCodeTable.ERR;
                throw new ErrorMsg(this);
            }
            Valid = Valid_Type.OK;
        }
        protected void SetToBody(byte[] sourceArray, int sourceIndex, int BodyIndex, int length)
        {
            byte[] temp_body = Body;
            Array.Copy(sourceArray, sourceIndex, temp_body, BodyIndex, length);
            Body = temp_body;
        }
        protected void SetToBody(byte val, int BodyIndex)
        {
            byte[] temp_body = Body;
            temp_body[BodyIndex] = val;
            Body = temp_body;
        }



        public class ErrorMsg : Exception
        {
            public Message m_Msg;
            public ErrorMsg(Message Msg)
            {
                m_Msg = Msg;
            }
        }

        public class WrongParse : ErrorMsg
        {
            private readonly string m_text;
            public WrongParse(string _text, Message Msg) : base(Msg)
            {
                m_Msg = Msg;
                m_text = _text;
            }

            public override string ToString()
            {
                return m_text;
            }
        }

        public enum N_Status: byte
        {
            [DescriptionAttribute("OK")]
            OK = 0,
            [DescriptionAttribute("Error")]
            Error = 1,
            [DescriptionAttribute("NA")]
            NA = 2
        }

        public enum N_Enable: byte
        {
            Enable = 0,
            Disable = 1,
            NA = 2
        }
        public enum P_Enable : byte
        {
            Enable = 1,
            Disable = 0,
            NA = 2
        }
        public enum N_Set
        {
            Set = 0,
            UnSet = 1,
            NA = 2
        }
        public enum P_Set
        {
            Set = 1,
            UnSet = 0,
            NA = 2
        }
        public enum N_Level : byte
        {
            High = 0,
            Low = 1,
            NA = 2
        }
        public enum P_Battle : byte
        {
            Normal = 0,
            Battle = 1,
            NA = 2
        }
        public enum Error_Stat_Item : byte
        {
            [DescriptionAttribute("Normal")]
            NORMAL = 0,
            [DescriptionAttribute("Trip Fault")]
            TRIP_ERROR = 0x01,
            [DescriptionAttribute("BIT Error")]
            BIT_ERROR = 0x02,
            [DescriptionAttribute("Temp Warning")]
            TEMP_ERROR = 0x04,
            [DescriptionAttribute("NA")]
            NA = 2
        }
    }

    public class General_Ack : Message
    {
        static readonly OpCodeTable[] Avalible_OpCodes = {  OpCodeTable.OUTPUT_ACK,
                                                            OpCodeTable.TRIP_RESET_ACK,
                                                            OpCodeTable.RESET_ACK,
                                                            OpCodeTable.BATTLE_ACK,
                                                            OpCodeTable.SET_LIMIT_ACK,
                                                            OpCodeTable.POR_ACK,
                                                            OpCodeTable.DCI_ACK,
                                                            OpCodeTable.IP_MODE_ACK,
                                                            OpCodeTable.ERASE_ACK,
                                                            OpCodeTable.DEBUG_ACK,
                                                            OpCodeTable.SW_UPDATE_ACK,
                                                            };
        public General_Ack(OpCodeTable AckOp) : base()
        {
            if (Avalible_OpCodes.Contains(AckOp))
                OpCode = AckOp;
            else
                throw new ErrorMsg(this);
            Length = 4;
            Body = new byte[10];
            Ack = 3;
        }
        public General_Ack(Message msg) : base(msg)
        {
        }
        public General_Ack(byte[] RawData) : base(RawData)
        {
            if (!Avalible_OpCodes.Contains(OpCode))
                throw new WrongParse(String.Format("Wrong OpCode. Recieved : {0}, Expected Ack Opcode", OpCode), this);
            if (Length != 4)
                throw new WrongParse(String.Format("Wrong Length. Recieved : {0}, Expected : 4", Length), this);
            if (Body.Length != 1)
            {
                Body.Concat(new byte[1 - Body.Length]).Take(1);
            }
        }

        public int Ack
        {
            get
            {
                return Body[0];
            }
            set
            {
                    Body[0] = (byte)value;
                
            }
        }

    }

    public class Status : Message
    {
        public Status() : base(OpCodeTable.STATUS)
        {
            Length = (((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB) + 7 + 3));
            Body = new byte[Length];
        }

        public Status(Message msg) : base(msg)
        {
            if (Length != (((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB) + 7 + 3)))
                throw new WrongParse("Wrong Length", this);
        }
        public new void Get(PDU dev)
        {
            OpCode = OpCodeTable.STATUS;
            base.Get(dev);
        }
        public P_Enable[] Sw_State
        {
            get
            {
                P_Enable[] rc = new P_Enable[NUM_OF_CB];
                ulong state = conv(Body, 0);
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (P_Enable)((state >> i) & 0x01);
                return rc;
            }
        }
        public P_Enable[] Sw_Cmd
        {
            get
            {
                P_Enable[] rc = new P_Enable[NUM_OF_CB];
                ulong state = conv(Body, NUM_OF_CB_BYTE);
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (P_Enable)((state >> i) & 0x01);
                return rc;
            }
        }
        public Trip_Stat_Item[] Trip_Stat
        {
            get => Body.Skip(2* NUM_OF_CB_BYTE).Take(NUM_OF_CB).Select(x =>
                                   typeof(Trip_Stat_Item).IsEnumDefined(x) ?
                                            (Trip_Stat_Item)x :
                                            Trip_Stat_Item.NA
                                    ).ToArray();
        }
        public P_Battle[] Btl_Mode
        {
            get
            {
                P_Battle[] rc = new P_Battle[NUM_OF_CB];
                ulong state = conv(Body, 2 * NUM_OF_CB_BYTE + NUM_OF_CB);
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (P_Battle)((state >> i) & 0x01);
                return rc;
            }
        }
        public N_Status[] Bit
        {
            get
            {
                N_Status[] rc = new N_Status[NUM_OF_CB];
                ulong state = conv(Body, (3 * NUM_OF_CB_BYTE) + NUM_OF_CB);
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (N_Status)((state >> i) & 0x01);
                return rc;
            }
        }
        public decimal[] Out_Volt
        {
            get
            {
                decimal[] rc = new decimal[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (decimal)(BitConverter.ToInt16(Body.Skip(((4 * NUM_OF_CB_BYTE) + NUM_OF_CB) + 2 * i).Take(2).Reverse().ToArray(), 0)/16.0);
                return rc;
            }
        }
        public decimal[] Out_Curr
        {
            get
            {
                decimal[] rc = new decimal[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    rc[i] = (decimal)(BitConverter.ToInt16(Body.Skip(((4 * NUM_OF_CB_BYTE) + (NUM_OF_CB*3)) + (2 * i)).Take(2).Reverse().ToArray(), 0)/16.0);
                return rc;


            }
        }
        public int[] Channel_Group
        {
            get
            {
                return Body.Skip((4 * NUM_OF_CB_BYTE) + (5 * NUM_OF_CB) ).Take(NUM_OF_CB).Select(x=> (int)x).ToArray();
            }
        }
        public decimal Input_Volt
        {
            get => (decimal)(BitConverter.ToInt16(Body.Skip((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB)).Take(2).Reverse().ToArray(), 0) / 16.0);
        }
        public decimal Input_Curr
        {
            get => (decimal)(BitConverter.ToInt16(Body.Skip((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB)+2).Take(2).Reverse().ToArray(), 0) / 16.0);
        }
        public decimal Temp
        {
            get => (decimal)(BitConverter.ToInt16(Body.Skip((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB)+4).Take(2).Reverse().ToArray(), 0) / 16.0);

        }
        public Error_Stat_Item Error_Stat
        {
            get => Body.Skip((4 * NUM_OF_CB_BYTE) + (6 * NUM_OF_CB)+6).Take(1).Select(x =>
                                   typeof(Error_Stat_Item).IsEnumDefined(x) ?
                                            (Error_Stat_Item)x :
                                            Error_Stat_Item.NA
                                    ).First();

        }

    }

    public class PDU_Info : Message
    {

        public PDU_Info() : base(OpCodeTable.PDU_INFO)
        {
        }

        public PDU_Info(Message msg) : base(msg)
        {
            if (Length != 59)
                throw new WrongParse("Wrong Length", this);
        }
        public string PartNumber
        {
            get => Encoding.UTF8.GetString(Body.Skip(0).Take(16).ToArray());
        }
        public string SerialNumber
        {
            get => Encoding.UTF8.GetString(Body.Skip(16).Take(8).ToArray());
        }

        public DateTime SW_Ver_Date
        {
            get
            {
                try
                {
                    return new DateTime(int.Parse(Encoding.UTF8.GetString(Body.Skip(24).Take(4).ToArray())),
                                      int.Parse(Encoding.UTF8.GetString(Body.Skip(28).Take(2).ToArray())),
                                      int.Parse(Encoding.UTF8.GetString(Body.Skip(30).Take(2).ToArray())));
                }
                catch
                {
                    return new DateTime(1900, 1, 1);
                }
            }
        }
        public Version SW_Ver
        {
            get
            {
                try
                {
                    return new Version(int.Parse(Encoding.UTF8.GetString(Body.Skip(33).Take(3).ToArray())),
                                        int.Parse(Encoding.UTF8.GetString(Body.Skip(37).Take(3).ToArray())),
                                        int.Parse(Encoding.UTF8.GetString(Body.Skip(41).Take(3).ToArray())));
                }
                catch
                {
                    return new Version(0, 0, 0);
                }
            }

        }

    }

    public class Set_Output : Message
    {
        public Set_Output() : base(OpCodeTable.SET_OUTPUT)
        {
            Length = 5;
            Body = new byte[Length - 3];
        }
        public void Out(int output, P_Enable state)
        {
            SetToBody((byte)output, 0);
            SetToBody(state == P_Enable.Enable ? (byte)0x01 : (byte)0x00,1);


        }

        public void Group(int group, P_Enable state)
        {
            SetToBody((byte)(group + 128), 0);
            SetToBody(state == P_Enable.Enable ? (byte)0x01 : (byte)0x00, 1);

        }
    }

    public class Trip_Reset : Message
    {
        public Trip_Reset() : base(OpCodeTable.TRIP_RESET)
        {
            Length = 4;
            Body = new byte[Length - 3];
        }
        public void Out(int output)
        {
            SetToBody((byte)output, 0);
        }
        public void Group(int group)
        {
            SetToBody((byte)(group+ 128), 0);
        }
    }

    public class Reset : Message
    {
        public Reset() : base(OpCodeTable.RESET) { }
    }

    public class Set_Battle : Message
    {
        public Set_Battle() : base(OpCodeTable.SET_BATTLE)
        {
            Length = 5;
            Body = new byte[Length - 3];
        }
        public void Out(int output, P_Enable state)
        {
            SetToBody((byte)output, 0);
            SetToBody(state == P_Enable.Enable ? (byte)0x1 : (byte)0x0, 1);
        }
        public void Group(int group, P_Enable state)
        {
            SetToBody((byte)(group+ 128), 0);
            SetToBody(state == P_Enable.Enable ? (byte)0x1 : (byte)0x0, 1);
        }
    }

    public class Set_Limit : Message
    {
        public Set_Limit() : base(OpCodeTable.SET_LIMIT)
        {
            Length = 10;
            Body = new byte[Length - 3];
        }

        public Set_Limit Out(int output, decimal Overload, decimal CurrentLimit, TimeSpan ThermalConstant)
        {
            CurrentLimit = (CurrentLimit * 16 > 0xFFFF) ? 4095.9375M : CurrentLimit;
            Overload = (Overload * 16 > 0xFFFF) ? 4095.9375M : Overload;
            ThermalConstant = (ThermalConstant.TotalMilliseconds > 0xFFFF) ? TimeSpan.FromMilliseconds(0xFFFF) : ThermalConstant;
            SetToBody((byte)output, 0);
            SetToBody(BitConverter.GetBytes((ushort)(Overload * 16)).Reverse().ToArray(), 0, 1, 2);
            SetToBody(BitConverter.GetBytes((ushort)(CurrentLimit * 16)).Reverse().ToArray(), 0, 3, 2);
            SetToBody(BitConverter.GetBytes((ushort)(ThermalConstant.TotalMilliseconds)).Reverse().ToArray(), 0, 5, 2);
            return this;
        }

        public Set_Limit Group(int group, decimal Overload, decimal CurrentLimit, TimeSpan ThermalConstant)
        {
            return Out(group + 128, Overload, CurrentLimit, ThermalConstant);
        }
    }

    public class Set_Group : Message
    {
        public Set_Group() : base(OpCodeTable.SET_GROUP)
        {
            Length = 4 + NUM_OF_CB;
            Body = new byte[Length - 3];
        }

        public Set_Group Group(int group)
        {
            SetToBody((byte)(group), 0);
            return this;
        }
        
        public int[] Channels
        {
            get
            {
                return Body.Skip(NUM_OF_CB_BYTE).TakeWhile(x=> x!=0).Select(x => (int)x).ToArray();
            }
            set
            {
                byte[] nchan = new byte[NUM_OF_CB];
                value.Select(x => (byte)x).ToArray().CopyTo(nchan, 0);
                SetToBody(nchan, 0, 1, NUM_OF_CB);
            }
        }
    }

    public class Set_Por_Macro : Message
    {
        public Set_Por_Macro() : base(OpCodeTable.SET_POR_MACRO)
        {
            Length = 1 + 2 + NUM_OF_CB + NUM_OF_CB*2; // op-cado(1) + Length(2) + chan_on(UINT8[16]) + CHAN_DELAY(UINT16[16])
            Body = new byte[Length - 3];
        }

        public int[] ChannelOn
        {
            set
            {
                byte[] b = new byte[NUM_OF_CB];
                value?.Select(x => (byte)x).ToArray().CopyTo(b, 0);
                SetToBody(b, 0, 0,NUM_OF_CB);
            }  
        }

        public TimeSpan[] Delay
        {
            set
            {
                byte[] b = new byte[NUM_OF_CB *2];
                for (int i = 0; i < value.Count() && i < NUM_OF_CB; i++)
                {
                    Array.Copy(BitConverter.GetBytes((UInt16)value[i].TotalMilliseconds).Reverse().ToArray(), 0, b, i * 2, 2);
                }
                SetToBody(b, 0, NUM_OF_CB, NUM_OF_CB * 2);
            }
        }
    }

    public class Set_Dci_Macro : Message
    {
        public Set_Dci_Macro() : base(OpCodeTable.SET_DCI_MACRO)
        {
            Length = 3 + NUM_OF_CB*4; //  Op-Code(1) + Length(2) + CHANNEL(NUM_OF_CB) + CHANNEL_ STATE(NUM_OF_CB) + CHAN_DELAY(NUM_OF_CB*2)
            Body = new byte[Length - 3];
        }

        public int[] Channel
        {
            set
            {
                byte[] b = new byte[NUM_OF_CB];
                value.Select(x => (byte)x).ToArray().CopyTo(b, 0);
                SetToBody(b, 0, 0, NUM_OF_CB);
            }
        }

        public P_Enable[] State
        {
            set
            {
                byte[] b = new byte[NUM_OF_CB];
                value.Select(x => (byte)x).ToArray().CopyTo(b, 0);
                SetToBody(b, 0, NUM_OF_CB, NUM_OF_CB);
            }
        }

        public TimeSpan[] Delay
        {
            set
            {
                byte[] b = new byte[NUM_OF_CB * 2];
                for (int i = 0; i < value.Count() && i < NUM_OF_CB; i++)
                {
                    Array.Copy(BitConverter.GetBytes((UInt16)value[i].TotalMilliseconds).Reverse().ToArray(), 0, b, i * 2, 2);
                }
                SetToBody(b, 0, NUM_OF_CB*2, NUM_OF_CB * 2);
            }
        }
    }

    public class Set_Ip_Mode : Message
    {
        public Set_Ip_Mode() : base(OpCodeTable.SET_IP_MODE)
        {
            Length = 12;
            Body = new byte[Length - 3];
        }

        public bool DynamicIP
        {
            set => SetToBody(value ? (byte)0 : (byte)1, 0);
        }

        public IPAddress IP
        {
            set => SetToBody(value.GetAddressBytes(), 0, 1, 4);
        }

        public IPAddress IPMask
        {
            set => SetToBody(value.GetAddressBytes(), 0, 5, 4);
        }
    }

    public class Config : Message
    {
        public Config() : base(OpCodeTable.CONFIG)
        {
            Length =    1 + 2 + 1 +                                             //Opcode length Ack
                        4 + 4 + 1 +                                             // Ip configuration 
                        (NUM_OF_CB * 2) + (NUM_OF_CB * 2) + (NUM_OF_CB * 2) +   //Current settings (OL,Short,Trip time) 
                        NUM_OF_CB +                                             //Grouping
                        NUM_OF_CB + (NUM_OF_CB * 2) +                           // POR settings
                        NUM_OF_CB + NUM_OF_CB + (NUM_OF_CB * 2) ;               // DCI settings
            Body = new byte[Length];

        }

        public Config(Message msg) : base(msg)
        {
            Config temp = new Config();
            if (Length != temp.Length)
                throw new WrongParse("Wrong Length", this);
            Valid = Body[0] == 0 ? Valid_Type.OK : Valid_Type.Fail;
        }
        
        public IPAddress IP
        {
            get => new IPAddress(Body.Skip(1).Take(4).ToArray());
        }
        public IPAddress SubnetMask
        {
            get => new IPAddress(Body.Skip(5).Take(4).ToArray());
        }

        public bool DynamicIP
        {
            get => Body.Skip(9).First() == 0 ? true : false;
        }

        public decimal[] OverloadSetting
        {
            get
            {
                decimal[] val = new decimal[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = (decimal)(BitConverter.ToUInt16(Body.Skip(10 + i * 2).Take(2).Reverse().ToArray(), 0)/16.0);
                return val;
            }
        }

        public decimal[] CurrentLimitSetting
        {
            get
            {
                decimal[] val = new decimal[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = (decimal)(BitConverter.ToUInt16(Body.Skip(10+(NUM_OF_CB*2) + i * 2).Take(2).Reverse().ToArray(), 0) / 16.0);
                return val;
            }
        }

        public TimeSpan[] ThermalConstSetting
        {
            get
            {
                TimeSpan[] val = new TimeSpan[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = TimeSpan.FromMilliseconds(BitConverter.ToUInt16(Body.Skip(10 + (NUM_OF_CB * 4) + i * 2).Take(2).Reverse().ToArray(), 0) );
                return val;
            }
        }

        public int[] GroupNum
        {
            get
            {
                int[] val = new int[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = Body.Skip(10 + (NUM_OF_CB * 6) + i).First();
                return val;
            }
        }

        public int[] PorChannelOrder
        {
            get
            {
                int[] val = new int[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = Body.Skip(10 + (NUM_OF_CB * 7) + i).First();
                return val.TakeWhile(x=> x!=0).ToArray();
            }
        }

        public TimeSpan[] PorChannelDelay
        {
            get
            {
                TimeSpan[] val = new TimeSpan[PorChannelOrder.Length];
                for (int i = 0; i < val.Length; i++)
                    val[i] = TimeSpan.FromMilliseconds(BitConverter.ToUInt16(Body.Skip(10 + (NUM_OF_CB * 8) + 2*i).Take(2).Reverse().ToArray(), 0));
                return val;
            }
        }

        public int[] DciChannelOrder
        {
            get
            {
                int[] val = new int[NUM_OF_CB];
                for (int i = 0; i < NUM_OF_CB; i++)
                    val[i] = Body.Skip(10 + (NUM_OF_CB * 10) + i).First();
                return val.TakeWhile(x => x != 0).ToArray();
            }
        }

        public P_Enable[] DciChannelAction
        {
            get
            {
                P_Enable[] val = new P_Enable[DciChannelOrder.Length];
                for (int i = 0; i < val.Length; i++)
                    val[i] = (P_Enable)Body.Skip(10 + (NUM_OF_CB * 11) + i).First();
                return val;
            }
        }

        public TimeSpan[] DciChannelDelay
        {
            get
            {
                TimeSpan[] val = new TimeSpan[DciChannelOrder.Length];
                for (int i = 0; i < val.Length; i++)
                    val[i] = TimeSpan.FromMilliseconds(BitConverter.ToUInt16(Body.Skip(10 + (NUM_OF_CB * 12) + 2 * i).Take(2).Reverse().ToArray(), 0));
                return val;
            }
        }



    }

    public class Erase_Data : Message
    {
        public Erase_Data() : base(OpCodeTable.ERASE_DATA)
        {
            Length = 35;
            Body = new byte[Length - 3];
        }

        public string Confirm
        {
            set => SetToBody(Encoding.UTF8.GetBytes(value), 0, 0, value.Count());
        }

    }

    public class Set_Debug : Message
    {
        public Set_Debug() : base(OpCodeTable.SET_DEBUG)
        {
            Length = 12;
            Body = new byte[Length - 3];

        }
        public Set_Debug(byte[] RawData) : base(RawData)
        {
            if (OpCode != OpCodeTable.SET_DEBUG)
                throw new WrongParse(String.Format("Wrong OpCode. Recieved : {0}, Expected : 0x{0:X}", OpCode, (byte)OpCodeTable.SET_DEBUG), this);
            if (Length != 12)
                throw new WrongParse(String.Format("Wrong Length. Recieved : {0}, Expected : 9", Length), this);
            if (Body.Length < Length - 3)
            {
                Body.Concat(new byte[Length - 3 - Body.Length]);
            }
        }

        public Set_Debug(Message msg) : base(msg)
        {

        }

        public byte Type //0- SN, 1 - Slave Calibration
        {
            set => SetToBody(value,0);
        }

        public string SN
        {
            
            set {
                OpCode = OpCodeTable.SET_DEBUG;
                byte[] fn = new byte[8];
                int serial;
                string[] words = value.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                switch (words.Length)
                {
                    case 1:
                        if (words[0].Length > 7 || words[0].Length == 0)
                            throw new Exception("Wrong Serial Number");
                        serial = int.Parse(value);
                        break;
                    case 2:
                        if (words[0] != "M9547")
                            throw new Exception("Wrong Part Number");
                        serial = int.Parse(words[1]);
                        break;
                    default:
                        throw new Exception("Wrong Format");
                }

                if (serial <= 0 || serial > 9999999)
                    throw new Exception("Wrong Serial Number");
                Array.Copy(Encoding.UTF8.GetBytes(serial.ToString("0000000")), fn, 7);
                SetToBody(fn, 0, 1, 7);
                
            }
        }
        
        public UInt16 CbNumMask
        {
            set => SetToBody(BitConverter.GetBytes(value).Reverse().ToArray(), 0, 1, 2);
        }

        public void GetOutputTemp(PDU dev)
        {
            SendInterface?.Invoke(new Message(new byte[4] { (byte)OpCodeTable.SET_DEBUG, 0, 4, 1 }).ToByteArray(),dev.IP);
            Valid = Valid_Type.UNKNOWN;
        }

        public decimal[] OutputTemp
        {
            get
            {
                decimal[] val = new decimal[NUM_OF_CB];
                if (Length != 4+NUM_OF_CB*2 || Body[0] != 1)
                    return val;
                for (int i =0; i< NUM_OF_CB; i++)
                {
                    val[i] = BitConverter.ToInt16(Body.Skip(1 + i * 2).Take(2).Reverse().ToArray(), 0)/((decimal)16.0);
                }
                return val;
            }
        }

    }
      
    public class SW_Info : Message
    {
        public SW_Info() : base(OpCodeTable.SW_INFO)
        {
            Length = 82;
            Body = new byte[Length - 3];
        }
        public SW_Info(Message msg) : base(msg)
        {
        }
        public SW_Info(byte[] RawData) : base(RawData)
        {
            if (OpCode != OpCodeTable.SW_INFO)
                throw new WrongParse(String.Format("Wrong OpCode. Recieved : {0}, Expected : 0x{0:X}", OpCode, (byte)OpCodeTable.SW_INFO), this);
            if (Length != 82)
                throw new WrongParse(String.Format("Wrong Length. Recieved : {0}, Expected : 82", Length), this);
            if (Body.Length < Length - 3)
            {
                Body.Concat(new byte[Length - 3 - Body.Length]);
            }
        }
        public IPAddress Server
        {
            get => new IPAddress(Body.Skip(0).Take(4).ToArray());
            set => SetToBody(value.GetAddressBytes(), 0, 0, 4);
        }
        public string FileName
        {
            get => Encoding.UTF8.GetString(Body.Skip(4).Take(33).ToArray());
            set => SetToBody(Encoding.UTF8.GetBytes(value), 0, 4, 33);
        }
        public enum SW_Status_Item : byte
        {
            [Description("Ready")]
            READY = 0x01,
            [Description("Busy")]
            BUSY = 0x15,
            [Description("Update Succeed")]
            UPDATE_SUCCESS = 0x1F,
            [Description("Update Failed")]
            UPDATE_FAIL = 0x29,
            [Description("Failed: Size too big")]
            UPDATE_FAIL_SIZE = 0x2A,
            [Description("Failed: Unknown reason")]
            UPDATE_FAIL_GENERAL = 0x2B,
            [Description("Update Verify Failed")]
            UPDATE_FAIL_VERIFY = 0x2C,
            [Description("Unknown Status")]
            NA = 0xFF
        }
        public SW_Status_Item Status
        {
            get => Body.Skip(37).Take(1).Select(x =>
                                   typeof(SW_Status_Item).IsEnumDefined(x) ?
                                            (SW_Status_Item)x :
                                            SW_Status_Item.NA
                                    ).First();
            set => SetToBody(new byte[] { (byte)value }, 0, 37, 1);
        }
        public uint Progress
        {
            get => BitConverter.ToUInt32(Body, 38);
            set => SetToBody(BitConverter.GetBytes(value), 0, 38, 4);
        }
        public string ActiveFile
        {
            get => Encoding.UTF8.GetString(Body.Skip(42).Take(33).ToArray());
            set => SetToBody(Encoding.UTF8.GetBytes(value), 0, 42, 33);
        }
        public uint Active_Size
        {
            get => BitConverter.ToUInt32(Body, 75);
            set => SetToBody(BitConverter.GetBytes(value), 0, 75, 4);
        }


    }

    public class Set_SW_Update : Message
    {
        public int File_Size;
        public Set_SW_Update() : base(OpCodeTable.SET_SW_UPDATE)
        {
            Length = 40;
            Body = new byte[Length - 3];
        }
        public Set_SW_Update(byte[] RawData) : base(RawData)
        {
            if (OpCode != OpCodeTable.SET_SW_UPDATE)
                throw new WrongParse(String.Format("Wrong OpCode. Recieved : {0}, Expected : 0x{0:X}", OpCode, (byte)OpCodeTable.SET_SW_UPDATE), this);
            if (Length != 40)
                throw new WrongParse(String.Format("Wrong Length. Recieved : {0}, Expected : 40", Length), this);
            if (Body.Length < Length - 3)
            {
                Body.Concat(new byte[Length - 3 - Body.Length]);
            }
        }
        public IPAddress Server
        {
            get => new IPAddress(Body.Skip(0).Take(4).ToArray());
            set => SetToBody(value.GetAddressBytes(), 0, 0, 4);
        }
        public string FileName
        {
            get => Encoding.UTF8.GetString(Body.Skip(4).Take(33).ToArray());
            set
            {
                byte[] fn = new byte[33];
                Array.Copy(Encoding.UTF8.GetBytes(value), fn, value.Length);
                SetToBody(fn, 0, 4, 33);
            }
        }

    }
}
