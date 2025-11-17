using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Drawing.Imaging;
using System.Net.NetworkInformation;
using System.ComponentModel;
using System;

namespace Data
{


    public class PDU : INotifyPropertyChanged
    {
        public delegate void SendInterface_T(byte[] message, IPAddress filter_ip);

        private DateTime LastRecieveTime;
        public bool Obselete
        {
            get => (DateTime.Now - LastRecieveTime).TotalSeconds > 2;
        }

        public Message.P_Enable[] Sw_State;
        public Message.P_Enable[] Sw_Cmd;
        public Trip_Stat_Item[] Trip_Stat;
        public Message.P_Battle[] Btl_Mode;
        public Message.N_Status[] Bit;
        public decimal[] Out_Volt;
        public decimal[] Out_Curr;
        public int[] Channel_Group;
        public decimal Input_Volt;
        public decimal Input_Curr;
        public decimal Temp;
        public Message.Error_Stat_Item Error_Stat;

        public string PartNumber = "NA";
        public string SerialNumber = "NA";
        public DateTime SW_Ver_Date = new DateTime();
        public Version SW_Ver = new Version();

        public IPAddress            Config_IP;
        public IPAddress            SubnetMask;
        public bool                 DynamicIP;
        public TimeSpan             CommlossTimeout;
        public Message.P_Enable[]   CommlossOperation;
        public decimal[]            CurrentLimitSetting;
        public decimal[]            OverloadSetting;
        public TimeSpan[]           ThermalConstSetting;
        public int[]                Conf_Channel_Group = new int[] { 255, 255, 255, 255, 255, 255, 255, 255 };
        public int[]                PorChannelOrder;
        public TimeSpan[]           PorChannelDelay;
        public int[]                DciChannelOrder;
        public Message.P_Enable[]   DciChannelAction;
        public TimeSpan[]           DciChannelDelay;


        public Dictionary<OpCodeTable, Valid_Type> Message_Validity = new Dictionary<OpCodeTable, Valid_Type>
        {
            {OpCodeTable.BATTLE_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.CONFIG,Valid_Type.UNKNOWN },
            {OpCodeTable.DCI_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.DEBUG_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.ERASE_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.GROUP_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.IP_MODE_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.KEEPALIVE,Valid_Type.UNKNOWN },
            {OpCodeTable.OUTPUT_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.PDU_INFO,Valid_Type.UNKNOWN },
            {OpCodeTable.POR_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.RESET_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.STATUS,Valid_Type.UNKNOWN },
            {OpCodeTable.SW_INFO,Valid_Type.UNKNOWN },
            {OpCodeTable.SW_UPDATE_ACK,Valid_Type.UNKNOWN },
            {OpCodeTable.TRIP_RESET_ACK,Valid_Type.UNKNOWN },
        };


        public IPAddress Update_Server;
        public string Update_FileName;
        public SW_Info.SW_Status_Item Update_Status;
        public uint Update_Progress;
        public string Active_File;
        public uint Active_Size;

        public int Update_File_Size;

        public decimal[] OutputTemp;


        public IPAddress IP;
        public string ID
        {
            get
            {
                if (this.Message_Validity[OpCodeTable.PDU_INFO] == Valid_Type.OK)
                    return "SN" + this.SerialNumber;
                else
                    return "IP: " + IP.ToString();
            }
        }
        public DateTime LastUpdated { get; private set; }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public PDU()
        {
            IP = new IPAddress(new byte[4] { 255, 255, 255, 255 });
        }
        public PDU(IPAddress ip) : this()
        {
            IP = ip;
        }
        public void Parse(Message massage)
        {
            LastUpdated = DateTime.Now;
            Message_Validity[massage.OpCode] = massage.Valid;
            LastRecieveTime = DateTime.Now;
            switch (massage.OpCode)
            {
                case OpCodeTable.KEEPALIVE:
                case OpCodeTable.STATUS:
                    {
                        Status nStat = new Status(massage);
                        Sw_State = nStat.Sw_State;
                        Sw_Cmd = nStat.Sw_Cmd;
                        Trip_Stat = nStat.Trip_Stat;
                        Btl_Mode = nStat.Btl_Mode;
                        Bit = nStat.Bit;
                        Out_Volt = nStat.Out_Volt;
                        Out_Curr = nStat.Out_Curr;
                        Channel_Group = nStat.Channel_Group;
                        Input_Volt = nStat.Input_Volt;
                        Input_Curr = nStat.Input_Curr;
                        Temp = nStat.Temp;
                        Error_Stat = nStat.Error_Stat;
                    }
                    NotifyPropertyChanged("Status");
                    break;
                case OpCodeTable.SW_INFO:
                    {
                        SW_Info nSw = new SW_Info(massage);
                        Update_Server = nSw.Server;
                        Update_FileName = nSw.FileName;
                        Update_Status = nSw.Status;
                         Update_Progress = nSw.Progress;
                         Active_File = nSw.ActiveFile;
                         Active_Size = nSw.Active_Size;
                    }
                    NotifyPropertyChanged("SW_Info");
                    break;
                case OpCodeTable.PDU_INFO:
                    {
                        PDU_Info nInfo = new PDU_Info(massage);
                        PartNumber = nInfo.PartNumber;
                        SerialNumber = nInfo.SerialNumber;
                        SW_Ver_Date = nInfo.SW_Ver_Date;
                        SW_Ver = nInfo.SW_Ver;
                    }
                    NotifyPropertyChanged("PDU_Info");
                    break;
                case OpCodeTable.CONFIG:
                    {
                        using (Config nConf = new Config(massage))
                        {
                            Config_IP = nConf.IP;
                            SubnetMask = nConf.SubnetMask;
                            DynamicIP = nConf.DynamicIP;
                            CurrentLimitSetting = nConf.CurrentLimitSetting;
                            OverloadSetting = nConf.OverloadSetting;
                            ThermalConstSetting = nConf.ThermalConstSetting;
                            Conf_Channel_Group = nConf.GroupNum;
                            PorChannelOrder = nConf.PorChannelOrder;
                            PorChannelDelay = nConf.PorChannelDelay;
                            DciChannelOrder = nConf.DciChannelOrder;
                            DciChannelAction = nConf.DciChannelAction;
                            DciChannelDelay = nConf.DciChannelDelay;
                        }
                        NotifyPropertyChanged("Config");
                    }
                    break;
                case OpCodeTable.DEBUG_ACK:
                    if (massage.Length == 4)
                    {
                        NotifyPropertyChanged("Ack");
                    }
                    else
                    {
                        Set_Debug nDbg = new Set_Debug(massage);
                        OutputTemp = nDbg.OutputTemp;
                        NotifyPropertyChanged("DebugTemp");
                    }
                    break;
                case OpCodeTable.SET_LIMIT_ACK:
                    Message_Validity[massage.OpCode] = (Valid_Type) (new General_Ack(massage).Ack) ;
                    NotifyPropertyChanged("LimitAck");
                    break;
                default:
                    Message_Validity[massage.OpCode] = (Valid_Type)(new General_Ack(massage).Ack);
                    NotifyPropertyChanged("Ack");
                    break;
            }
        }

    }

    
}
