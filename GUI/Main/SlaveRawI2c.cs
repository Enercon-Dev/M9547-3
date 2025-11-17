using Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace M9312_1
{
    class SlaveRawI2c
    {
        public System.Threading.AutoResetEvent RecievedI2C;
        public Slave_RW_Raw RecievedMsg = null;
        System.Threading.SynchronizationContext SC;
        public delegate void PrintFunc(object str);
        public PrintFunc PrintToBox;

        public SlaveRawI2c()
        {
            RecievedI2C = new System.Threading.AutoResetEvent(false);
            SC = System.Threading.SynchronizationContext.Current;
        }

        private void Write(int CbNum, byte Reg_Add, byte[] Data)
        {
            //SC.Send(status => PrintToBox(Data),null);
            foreach (byte b in Data)
            {
                Slave_Write_Raw msg = new Slave_Write_Raw();
                msg.CBnum = CbNum;
                msg.InternalAddress = Reg_Add;
                if (Reg_Add < 50)
                    Reg_Add++;
                msg.Data = b;
                SC.Send(status => msg.Send(), null);
                RecievedI2C.WaitOne(1000);
            }
        }

        private byte[] Read(int CbNum, byte Reg_Add, uint length)
        {
            List<byte> res = new List<byte>();
            
            for (int i=0; i<length; i++)
            {
                Slave_RW_Raw msg = new Slave_RW_Raw();
                msg.CBnum = CbNum;
                msg.InternalAddress = Reg_Add;
                if (Reg_Add < 50)
                    Reg_Add++;
                SC.Send(status => msg.Send(),null);
                RecievedI2C.WaitOne(1000);
                if (RecievedMsg == null)
                    throw new Exception("Not Recieved Back I2C, Read Fuction SlaveRawI2C");
                if (RecievedMsg.CBnum != CbNum)
                    throw new Exception("Wrong SSPC Num, Read Fuction SlaveRawI2C");
                res.Add(RecievedMsg.Data);
                RecievedMsg = null;
            }
            //SC.Send(status => PrintToBox(res.ToArray()),null);
            return res.ToArray();
        }

        private uint get_current_raw(int CbNum)
        {
            return BitConverter.ToUInt16(Read(CbNum, 0x06, 2), 0);
        }

        /*public void Calibrate()
        {
            foreach (SSPC cb in SSPC.SSPCList)
            {
                try
            {
               
                    uint current = get_current_raw(cb.SSPC_ID);
                    if (current == 0xFFFF)
                        continue;
                    if (current > 1)
                        Calibrate_CB(cb.SSPC_ID);
               
            }
            catch
            {

            }
            }


        }*/

        private void Calibrate_CB(int CbNum)
        {
            SC.Send(status => PrintToBox("Calibrating Ch " + CbNum.ToString()),null);
            set_offset_current_raw(CbNum, 0);
            uint minoffset = 200;
            for (int i=0; i<10; i++)
            {
                uint offset = get_current_raw(CbNum);
                minoffset = (offset < minoffset) ? offset : minoffset;
            }
            SC.Send(status => PrintToBox("Offset is: " +minoffset.ToString()),null);
            set_offset_current_raw(CbNum, minoffset);
            save_offset_to_mem(CbNum);
        }

        private void save_offset_to_mem(int sSPCnum)
        {
            byte offset = Read(sSPCnum, 0x12, 1).First();
            Write(sSPCnum, 0x13, new byte[1] { 1 });
            System.Threading.Thread.Sleep(1000);
            byte[] data = new byte[16];
            data[6] = offset;
            data[7] = 0xF0;
            WriteUFM0(sSPCnum,data);
            Write(sSPCnum, 0x13, new byte[1] { 0 });
        }

        private int WriteUFM0(int sSPCnum, byte[] data)
        {
            if (data.Length != 16)
                return -1;
            Write_WB(sSPCnum, 0x74, new byte[] { 0x08, 0, 0 });
            int retryCounter = 0;
            while (true)
            {
                byte[] stat = Read_WB(sSPCnum, 0x3C, new byte[] { 0, 0, 0 }, 4);
                if ((stat[2] & 0x10) == 0)
                {
                    SC.Send(status => PrintToBox(stat),null);
                    break;
                }
                retryCounter++;
                if (retryCounter > 100)
                    throw new Exception("Current Breaker Not Answerring, SlaveRawI2c.cs Write UFM0, 1");
            }

            Write_WB(sSPCnum, 0xCB, new byte[] { 0, 0, 0 });
            while (true)
            {
                byte[] stat = Read_WB(sSPCnum, 0x3C, new byte[] { 0, 0, 0 }, 4);
                if ((stat[2] & 0x10) == 0)
                {
                    SC.Send(status => PrintToBox(stat), null);
                    break;
                }
                retryCounter++;
                if (retryCounter > 100)
                    throw new Exception("Current Breaker Not Answerring, SlaveRawI2c.cs Write UFM0, 2");
            }

            Write_WB(sSPCnum, 0x47, new byte[] { 0, 0, 0 });
            Write_WB(sSPCnum, 0xC9, new byte[] { 0, 0, 0x01 },data);
            while (true)
            {
                byte[] stat = Read_WB(sSPCnum, 0x3C, new byte[] { 0, 0, 0 }, 4);
                if ((stat[2] & 0x10) == 0)
                {
                    SC.Send(status => PrintToBox(stat), null);
                    break;
                }
                retryCounter++;
                if (retryCounter > 100)
                    throw new Exception("Current Breaker Not Answerring, SlaveRawI2c.cs Write UFM0, 3");
            }
            Write_WB(sSPCnum, 0xC9, new byte[] { 0, 0, 0x01 }, data);
            while (true)
            {
                byte[] stat = Read_WB(sSPCnum, 0x3C, new byte[] { 0, 0, 0 }, 4);
                if ((stat[2] & 0x10) == 0)
                {
                    SC.Send(status => PrintToBox(stat), null);
                    break;
                }
                retryCounter++;
                if (retryCounter > 100)
                    throw new Exception("Current Breaker Not Answerring, SlaveRawI2c.cs Write UFM0, 4");
            }

            Write_WB(sSPCnum, 0x26, new byte[] { 0, 0 });
            Write_WB(sSPCnum, 0xFF);
            return 0;
        }


        private byte[] Read_WB(int sSPCnum, int Cmd, byte[] Operand, uint dataLength)
        {
            Open_Frame(sSPCnum);
            Write(sSPCnum, 0x71, new byte[] { (byte)Cmd });
            Write(sSPCnum, 0x71, Operand);
            if (dataLength == 0)
            {
                Close_Frame(sSPCnum);
                return null;
            }
            byte[] data = Read(sSPCnum, 0x73, dataLength);
            Close_Frame(sSPCnum);
            return data;
        }

        private void Close_Frame(int sSPCnum)
        {
            Write(sSPCnum, 0x70, new byte[] { 0x00 });
        }

        private void Open_Frame(int sSPCnum)
        {
            Write(sSPCnum, 0x70, new byte[] { 0x80 });
        }

        private void Write_WB(int sSPCnum, int Cmd, byte[] Operand = null, byte[] data = null)
        {
            Open_Frame(sSPCnum);
            Write(sSPCnum, 0x71, new byte[] { (byte)Cmd });
            if (Operand == null)
            {
                Close_Frame(sSPCnum);
                return;
            }
            Write(sSPCnum, 0x71, Operand);
            if (data == null)
            {
                Close_Frame(sSPCnum);
                return;
            }
            Write(sSPCnum, 0x71, data);
            Close_Frame(sSPCnum);
            return;
        }

        private void set_offset_current_raw(int sSPCnum, uint offset)
        {
            Write(sSPCnum, 0x12, new byte[]{ (byte)offset});
        }
    }
}
