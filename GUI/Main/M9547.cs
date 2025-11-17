using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Reflection;
using System.Net;
using WinDHCP.Library;
using System.Collections.Specialized;
using Tftp.Net;
using Data;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using SimpleServer;
using System.IO;
using Coding;
using System.Drawing.Imaging;
using static Data.Message;
using System.Text.RegularExpressions;
using IPAddressControlLib;

namespace M9547
{
    public partial class Form1 : Form
    {
        private TabPage hiddenPage = new TabPage();
        private List<PDU> PDUs = new List<PDU>();
        private DhcpServer dhcp_server = new DhcpServer();
        private RadioButton[] TreeRootsRight = new RadioButton[3];
        private RadioButton[] TreeRootsLeft = new RadioButton[3];
        private TftpServer m_TftpServer;
        public PDU ActivePDU;
        private bool UpdateData = false;
        private IPAddress MyIP = null;
        private IPAddress BrodcastIP;
        private NetworkInterface m_adapter = null;
        private TSimpleServer FilterServer = null;
        private List<string> m_TFTP_Path = new List<string>();
        private Dictionary<string, Component>[] LVPS_Outputs = new Dictionary<string, Component>[7];
        public BindingList<LimitDataSource> LimitData = new BindingList<LimitDataSource>();
        //StreamWriter file = null;



        public Form1()
        {
            InitializeComponent();
            if (System.Deployment.Application.ApplicationDeployment.IsNetworkDeployed)
                this.Text = this.Text + " " + System.Deployment.Application.ApplicationDeployment.CurrentDeployment.CurrentVersion;
            else
                this.Text = this.Text + " " + Assembly.GetExecutingAssembly().GetName().Version.ToString();
        }

        private void ActiveFilter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {

            PDU dev = (sender as PDU);

            foreach (DataGridViewRow row in dgPDUs.Rows)
            {
                if ((row.Cells[2] as DataGridViewTextBoxCell).Value.ToString() == dev.IP.ToString())
                {
                    (row.Cells[1] as DataGridViewTextBoxCell).Value = dev.SerialNumber.ToString();
                    (row.Cells[3] as DataGridViewTextBoxCell).Value = dev.Error_Stat.ToString();
                }
            }

            if (ActivePDU == null) return;
            if (ActivePDU != dev) return;

            if (e.PropertyName == "SW_Info" || e.PropertyName == "")
            {
                progressBar1.Value = (dev.Update_File_Size == 0) ? 0 : (int)((dev.Update_Progress * 100) / dev.Update_File_Size);
                label2.Text = dev.Update_Status.ToName();
                label12.Text = dev.Update_Server.ToString();
                label14.Text = dev.Update_FileName;
                label4.Text = dev.Active_File;
                label15.Text = dev.Active_Size.ToString();
            }
            if (e.PropertyName == "Status" || e.PropertyName == "")
            {
                foreach (OUTPUT sw in OUTPUT.OUTList)
                {
                    sw.Switch.Sent_Value = dev.Sw_Cmd[sw.OUT_ID - 1] == P_Enable.Enable ? true : false;
                    sw.Switch.Recieved_Value = dev.Sw_State[sw.OUT_ID - 1] == P_Enable.Enable ? true : false;
                    sw.ErrorLable.Text = dev.Trip_Stat[sw.OUT_ID - 1].ToName();
                    sw.ErrorLable.Text += dev.Bit[sw.OUT_ID - 1] == N_Status.Error ? " & BIT Error" : "";
                    sw.BattleLable.Text = dev.Btl_Mode[sw.OUT_ID - 1].ToName();
                    sw.Voltage.Text = dev.Out_Volt[sw.OUT_ID - 1].ToString("0.00") + " V";
                    sw.Current.Text = dev.Out_Curr[sw.OUT_ID - 1].ToString("0.00") + " A";
                    GROUP.GROUPList.ForEach(x =>
                    {
                        List<OUTPUT> chns = x.Outputs;
                        chns.Remove(sw);
                        x.Outputs = chns;
                    }
                    );
                    if (dev.Channel_Group[sw.OUT_ID - 1] <= 8 && dev.Channel_Group[sw.OUT_ID - 1] > 0)
                    {
                        int group = dev.Channel_Group[sw.OUT_ID - 1];
                        List<OUTPUT> chns = GROUP.GROUPList.Find(x => x.GROUP_ID == group)?.Outputs;
                        chns.Add(sw);
                        GROUP.GROUPList.Find(x => x.GROUP_ID == group).Outputs = chns;
                    }
                }
                foreach (GROUP gr in GROUP.GROUPList)
                {
                    if (gr.Outputs.Count == 0)
                        continue;
                    gr.Current.Text = (gr.Outputs.Sum(x => dev.Out_Curr[x.OUT_ID - 1])).ToString("0.00") + " A";
                    gr.Voltage.Text = (gr.Outputs.Sum(x => dev.Out_Volt[x.OUT_ID - 1]) / gr.Outputs.Count).ToString("0.00") + " V";

                    P_Enable cmd = dev.Sw_Cmd[gr.Outputs[0].OUT_ID - 1];
                    P_Enable state = dev.Sw_State[gr.Outputs[0].OUT_ID - 1];
                    gr.Outputs.ForEach(x => cmd = (dev.Sw_Cmd[x.OUT_ID - 1] == cmd) ? cmd : P_Enable.NA);
                    gr.Outputs.ForEach(x => state = (dev.Sw_State[x.OUT_ID - 1] == state) ? state : P_Enable.NA);
                    gr.Switch.Sent_Value = cmd == P_Enable.Enable ? true : false;
                    gr.Switch.Recieved_Value = state == P_Enable.Enable ? true : false;
                    //TODO:ADD Group command status
                }
                lblInVolt.Text = dev.Input_Volt.ToString("0.00") + " V";
                lblInCurr.Text = dev.Input_Curr.ToString("0.00") + " A";
                lblTemp.Text = dev.Temp.ToString("0.00") + " °C";
                led1.Status = (Led_Type)dev.Error_Stat;
            }
            if (e.PropertyName == "PDU_Info" || e.PropertyName == "")
            {
                label8.Text = dev.SW_Ver.ToString();
                label79.Text = dev.SW_Ver_Date.ToShortDateString();
                if (dev.SerialNumber == Encoding.UTF8.GetString(new byte[] { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }))
                {
                    tbSerial.ReadOnly = false;
                    btnSetSN.Visible = true;
                    //tbSerial.Text = "0000000";
                }
                else
                {
                    tbSerial.ReadOnly = true;
                    btnSetSN.Visible = false;
                    tbSerial.Text = dev.SerialNumber;
                }
                label83.Text = dev.PartNumber;
            }
            if (e.PropertyName == "Config")
            {
                foreach (LimitDataSource o in LimitData)
                {
                    o.CurrentLimit = ActivePDU.CurrentLimitSetting[o.Channel - 1];
                    o.Overload = ActivePDU.OverloadSetting[o.Channel - 1];
                    o.ThermalConst = ActivePDU.ThermalConstSetting[o.Channel - 1];
                }
                dgvLimits.Refresh();
                tbPorChans.Text = string.Join(",", ActivePDU.PorChannelOrder.Select(x => x.ToString()));
                tbPorDelay.Text = string.Join(",", ActivePDU.PorChannelDelay.Select(x => x.TotalMilliseconds.ToString()));
                tbDciChan.Text = string.Join(",", ActivePDU.DciChannelOrder.Select(x => x.ToString()));
                tbDciDelay.Text = string.Join(",", ActivePDU.DciChannelDelay.Select(x => x.TotalMilliseconds.ToString()));
                tbDciOper.Text = string.Join(",", ActivePDU.DciChannelAction.Select(x => ((int)x).ToString()));
                rbConfigDynamic.Checked = ActivePDU.DynamicIP;
                rbConfigStatic.Checked = !ActivePDU.DynamicIP;
                iacSelectedIp.IPAddress = ActivePDU.Config_IP;
                iacSelectedMac.IPAddress = ActivePDU.SubnetMask;

            }
            if (e.PropertyName == "DebugTemp")
            {
                foreach (OUTPUT o in OUTPUT.OUTList)
                {
                    o.Temp.Text = ActivePDU.OutputTemp[o.OUT_ID - 1].ToString() + "°C";
                }
            }
            if (e.PropertyName == "")
            {

            }
            if (e.PropertyName == "LimitAck")
            {
                tmrGetStatus.Start();
                if (btnGroupLimits.Focused)
                    ValidToImage(btnGroupLimits, dev.Message_Validity[OpCodeTable.SET_LIMIT_ACK]);
                else
                {
                    (dgvLimits.CurrentRow.Cells[0] as DataGridViewButtonCell).FlatStyle = FlatStyle.Flat;
                    switch (dev.Message_Validity[OpCodeTable.SET_LIMIT_ACK])
                    {
                        case Valid_Type.OK:
                            (dgvLimits.CurrentRow.Cells[0] as DataGridViewButtonCell).Style.BackColor = Color.Green;
                            break;
                        case Valid_Type.Fail:
                            (dgvLimits.CurrentRow.Cells[0] as DataGridViewButtonCell).Style.BackColor = Color.PaleVioletRed;
                            break;
                        case Valid_Type.EEprom:
                            (dgvLimits.CurrentRow.Cells[0] as DataGridViewButtonCell).Style.BackColor = Color.Yellow;
                            break;
                        default:
                            (dgvLimits.CurrentRow.Cells[0] as DataGridViewButtonCell).Style.BackColor = Color.Transparent;
                            break;

                    }
                    dgvLimits.ClearSelection();
                }
            }
            if (e.PropertyName == "Ack")
            {
                tmrGetStatus.Start();
                ValidToImage(btnSetGroup, dev.Message_Validity[OpCodeTable.GROUP_ACK]);
                ValidToImage(button1, dev.Message_Validity[OpCodeTable.IP_MODE_ACK]);
                ValidToImage(btnReset, dev.Message_Validity[OpCodeTable.RESET_ACK]);
                ValidToImage(btnPor, dev.Message_Validity[OpCodeTable.POR_ACK]);
                ValidToImage(btnDci, dev.Message_Validity[OpCodeTable.DCI_ACK]);
                ValidToImage(button4, dev.Message_Validity[OpCodeTable.ERASE_ACK]);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            while (MyIP == null)
            {
                NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
                foreach (NetworkInterface adapter in nics)
                {
                    if (adapter.NetworkInterfaceType == NetworkInterfaceType.Loopback)
                        continue;
                    if (adapter.OperationalStatus == OperationalStatus.Down)
                        continue;
                    // Only display informatin for interfaces that support IPv4.
                    if (adapter.Supports(NetworkInterfaceComponent.IPv4) == false)
                    {
                        continue;
                    }

                    IPInterfaceProperties adapterProperties = adapter.GetIPProperties();
                    // Try to get the IPv4 interface properties.
                    IPv4InterfaceProperties p = adapterProperties.GetIPv4Properties();

                    if (p == null)
                        continue;
                    if (p.IsDhcpEnabled == true)
                        continue;
                    foreach (IPAddress ip in adapterProperties.UnicastAddresses.Select(x => x.Address))
                    {
                        if (ip.AddressFamily == AddressFamily.InterNetwork)
                        {
                            this.MyIP = ip;
#pragma warning disable CS0618 // Type or member is obsolete
                            BrodcastIP = new IPAddress(ip.Address | 0xFF000000);
#pragma warning restore CS0618 // Type or member is obsolete
                            m_adapter = adapter;
                            break;
                        }
                    }
                    if (MyIP != null)
                        break;

                }
                if (MyIP == null)
                {
                    DialogResult rs = MessageBox.Show("No Static Network Interface Found", "Error", MessageBoxButtons.RetryCancel);
                    if (rs == DialogResult.Cancel)
                        Close();
                }
            }
            this.FilterServer = new TSimpleServer(MyIP);
            this.m_TftpServer = new TftpServer(MyIP);
            //
            // TFTP
            this.m_TftpServer.OnReadRequest += new TftpServerEventHandler(this.server_OnReadRequest);
            this.m_TftpServer.OnError += new TftpServerErrorHandler(this.server_OnError);
            this.m_TftpServer.OnWriteRequest += new TftpServerEventHandler(this.server_OnWriteRequest);
            //
            ActivePDU = new PDU();
            Data.Message.SendInterface = FilterServer.Send;

            //string path = "C:\\Logs\\" + DateTime.Now.ToString("yyyyMMdd_HHmmss") + ".csv";
            //file = new StreamWriter(path, false, Encoding.ASCII);
            //textBox1.Text = path;
            hiddenPage = tabControl.TabPages["tpDHCP"];
            lbDHCPTrace.DisplayMember = "Msg";
            lbDHCPTrace.ValueMember = "Msg";
            lbDHCPTrace.DataSource = DhcpServer.Log;
            dgvDHCPLease.DataSource = dhcp_server.SessionExport;
            dgvLimits.DataSource = LimitData;

            cbGroup.Items.Clear();
            cbGroup.Items.AddRange(GROUP.GROUPList.Select(x => x.GROUP_ID_Text).ToArray());

            clbOutputs.Items.Clear();
            clbOutputs.Items.AddRange(OUTPUT.OUTList.Select(x => x.OUT_ID_Text).ToArray());

            DhcpServer.Log.RaiseListChangedEvents = true;
            DhcpServer.Log.ListChanged += lbDHCPTrace_Update;



            FilterServer.OnResponse += Recieve_To_Filter;
            FilterServer.OnErupt += Update_filters;
            FilterServer.Start();

            tabControl.TabPages.Remove(hiddenPage);
            UpdateTopologyDiagram();

            foreach (OUTPUT o in OUTPUT.OUTList)
            {
                LimitDataSource l = new LimitDataSource(o.OUT_ID);
                LimitData.Add(l);
            }

            led1.Status = Led_Type.OFF;
            m_TftpServer.Start();
        }

        private void Recieve_To_Filter(byte[] Message, EndPoint client)
        {
            if (((IPEndPoint)client).Address.Equals(MyIP)) return;
            if (!PDUs.Any(x => x.IP.Equals(((IPEndPoint)client).Address))) return;
            PDUs.Find(x => x.IP.Equals(((IPEndPoint)client).Address)).Parse(new Data.Message(Message));
            UpdateData = true;
        }

        private void Update_filters(byte[] Message, EndPoint client)
        {
            //file.WriteLine(string.Join("", Message.Select(x => x.ToString("X")).ToArray()));
            if (!PDUs.Any(x => x.IP.Equals(((IPEndPoint)client).Address))) {
                PDUs.Add(new PDU(((IPEndPoint)client).Address));
                PDUs.Last().PropertyChanged += ActiveFilter_PropertyChanged;
                UpdateTopologyDiagram();
            }

            PDUs.Find(x => x.IP.Equals(((IPEndPoint)client).Address)).Parse(new Data.Message(Message));
            PDUs.RemoveAll(x => x.Obselete);
            if (PDUs.Any(x => x.IP == ActivePDU.IP))
            {
                ActivePDU = PDUs.Find(x => x.IP == ActivePDU.IP);

            }
            UpdateData = true;
        }

        private void lbDHCPTrace_Update(object sender, ListChangedEventArgs e)
        {

            Invoke((MethodInvoker)(() => {

                /*DhcpServer.Log.RaiseListChangedEvents = false;

                string temp = DhcpServer.Log[e.NewIndex];
                DhcpServer.Log.RemoveAt(e.NewIndex);
                DhcpServer.Log.Insert(e.NewIndex, temp);
                lbDHCPTrace.Update();
                DhcpServer.Log.RaiseListChangedEvents = true;*/
                lbDHCPTrace.DataSource = DhcpServer.Log;
                lbDHCPTrace.Update();
                if (lbDHCPTrace.SelectedIndex == 1)
                    lbDHCPTrace.SelectedIndex = 0;

            }));
        }

        private void tsmiStart_Click(object sender, EventArgs e)
        {
            MenuItem Item = sender as MenuItem;
            if (Item.Text == "Start")
            {
                Item.Text = "Stop";
                tabControl.TabPages.Add(hiddenPage);
                tabControl.SelectTab("tpDHCP");
                dhcp_server.Start(m_adapter);
            }
            else
            {
                dhcp_server.Stop();
                Item.Text = "Start";
                hiddenPage = tabControl.TabPages["tpDHCP"];
                tabControl.TabPages.Remove(hiddenPage);
            }
        }

        private void radioButton4_Click(object sender, EventArgs e)
        {
        }

        private void UpdateTopologyDiagram()
        {
            dgPDUs.Rows.Clear();
            foreach (PDU dev in PDUs)
            {
                DataGridViewRow row = dgPDUs.Rows[dgPDUs.Rows.Add()];
                (row.Cells[0] as DataGridViewCheckBoxCell).Value = false;
                (row.Cells[1] as DataGridViewTextBoxCell).Value = dev.SerialNumber.ToString();
                (row.Cells[2] as DataGridViewTextBoxCell).Value = dev.IP.ToString();
                (row.Cells[3] as DataGridViewTextBoxCell).Value = dev.Error_Stat.ToString();
            }

            if (ActivePDU != null)
            {
                foreach (DataGridViewRow row in dgPDUs.Rows)
                {
                    if ((row.Cells[2] as DataGridViewTextBoxCell).Value.ToString() == ActivePDU.IP.ToString())
                    {
                        (row.Cells[0] as DataGridViewCheckBoxCell).Value = true;
                    }
                }
                Config conf = new Config();
                conf.Get(ActivePDU);
            }
        }

        private void tabControl_Selected(object sender, TabControlEventArgs e)
        {
            if (e.TabPage != tpDHCP)
            {
                UpdateTopologyDiagram();
                e.TabPage.Refresh();
            }
            if (e.TabPage == tpUpdate)
                tmrFirmware.Start();
            else
                tmrFirmware.Stop();

        }

        private void menuItem3_Click(object sender, EventArgs e)
        {

            List<PDU> pdus_to_update;
            DialogResult rslt = openFileDialog1.ShowDialog();
            if (rslt != DialogResult.OK)
                return;
            string tftp_file = openFileDialog1.FileName;

            FileStream fl = new FileStream(tftp_file, FileMode.Open, FileAccess.Read);
            byte[] magic_check = new byte[20];
            fl.Seek(0x150, SeekOrigin.Begin);
            fl.Read(magic_check, 0, 20);
            if (string.Compare(System.Text.Encoding.Default.GetString(magic_check.TakeWhile(x => x != '\0').ToArray()), "M9547 SW a") != 0)
            {
                MessageBox.Show("Flie is not legal for the project", "Wrong File", MessageBoxButtons.OK, MessageBoxIcon.Error);
                fl.Close();
                return;
            }
            fl.Position = 0;
            CRC16 coding = new CRC16();
            coding.Restart();
            coding.CalcCRC16(fl, fl.Length - 2);
            byte[] crcread = new byte[2];
            fl.Read(crcread, 0, 2);
            if (BitConverter.ToUInt16(crcread, 0) != coding.GetCRC)
            {
                MessageBox.Show("Flie CRC is not correct. Please rebuild file.", "File Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                fl.Close();
                return;
            }

            m_TFTP_Path.Add(tftp_file);
            using (ChoosePDUs form = new ChoosePDUs(PDUs))
            {
                if (form.ShowDialog() != DialogResult.OK)
                    return;
                pdus_to_update = form.Selected_PDUs;
            }
            Set_SW_Update msg = new Set_SW_Update
            {
                Server = MyIP,
                FileName = tftp_file.Substring(tftp_file.LastIndexOf('\\') + 1),
                File_Size = (int)fl.Length
            };
            pdus_to_update.ForEach(x => {
                x.Update_File_Size = (int)fl.Length;
                x.Update_Progress = 0;
                msg.Send(ActivePDU);
            });
        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void server_OnReadRequest(ITftpTransfer transfer, EndPoint client)
        {
            string path = m_TFTP_Path.Find(x => x.Contains(transfer.Filename));
            if (path == null)
                transfer.Cancel(TftpErrorPacket.FileNotFound);
            else
                //copy path to local and add CRC and then set to the filestream
                transfer.Start(new FileStream(path, FileMode.Open, FileAccess.Read));
        }

        private void server_OnWriteRequest(ITftpTransfer transfer, EndPoint client)
        {
            transfer.Cancel(TftpErrorPacket.AccessViolation);
        }

        private void server_OnError(TftpTransferError error)
        {
            throw new Exception(error.ToString());

        }

        private void menuItem4_Click(object sender, EventArgs e)
        {
            DialogResult rslt = openFileDialog1.ShowDialog();
            if (rslt != DialogResult.OK)
                return;
            string input_file = openFileDialog1.FileName;
            FileStream fl = new FileStream(input_file, FileMode.Open, FileAccess.Read);
            byte[] magic_check = new byte[20];
            fl.Seek(0x150, SeekOrigin.Begin);
            fl.Read(magic_check, 0, 20);
            if (string.Compare(System.Text.Encoding.Default.GetString(magic_check.TakeWhile(x => x != '\0').ToArray()), "M9547 SW a") != 0)
            {
                MessageBox.Show("Flie is not legal for the project", "Wrong File", MessageBoxButtons.OK, MessageBoxIcon.Error);
                fl.Close();
                return;
            }
            fl.Position = 0;
            CRC16 coding = new CRC16();
            coding.Restart();
            coding.CalcCRC16(fl);

            saveFileDialog1.FileName = openFileDialog1.FileName.Remove(openFileDialog1.FileName.Length - 4) + "_CRC";
            rslt = saveFileDialog1.ShowDialog();
            if (rslt != DialogResult.OK)
                return;
            string output_file = saveFileDialog1.FileName;
            FileStream fl_out = new FileStream(output_file, FileMode.Create, FileAccess.Write);
            fl.Position = 0;
            fl.CopyTo(fl_out);
            fl_out.Write(BitConverter.GetBytes(coding.GetCRC), 0, 2);
            fl_out.Close();
            fl.Close();
        }

        private void tmrFirmware_Tick(object sender, EventArgs e)
        {
            if (ActivePDU == null) return;
            if (!ActivePDU.IP.Equals(new IPAddress(new byte[] { 255, 255, 255, 255 })))
            {
                new SW_Info().Get(ActivePDU);
                new PDU_Info().Get(ActivePDU);
            }
        }

        private void Updater_Tick(object sender, EventArgs e)
        {

            if (PDUs.RemoveAll(x => (DateTime.Now - x.LastUpdated).TotalSeconds > 6) > 0)
            {
                UpdateData = true;
                if (!PDUs.Contains(ActivePDU))
                    led1.Status = Led_Type.OFF;
                UpdateTopologyDiagram();
            }
            if (UpdateData == false) return;
            UpdateData = false;

            if (ActivePDU == null) return;
            new Set_Debug().GetOutputTemp(ActivePDU);
        }

        private void label16_Click(object sender, EventArgs e)
        {

        }

        void ValidToImage(ButtonBase btn, Valid_Type valid)
        {
            if (!btn.Focused)
                return;
            switch (valid)
            {
                case Valid_Type.UNKNOWN:
                    btn.Image = null;
                    break;
                case Valid_Type.OK:
                    btn.Image = Properties.Resources.OK;
                    break;
                case Valid_Type.Fail:
                    btn.Image = Properties.Resources.Fail;
                    break;
                case Valid_Type.EEprom:
                    btn.Image = Properties.Resources.EEprom;
                    break;
                default:
                    btn.Image = null;
                    break;
            }
            btn.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
            btn.Refresh();
        }

        private void btnReset_Click(object sender, EventArgs e)

        {
            if (ActivePDU == null) return;
            ValidToImage(btnReset, Valid_Type.UNKNOWN);
            new Reset().Send(ActivePDU);
        }

        private void btnSetSN_Click(object sender, EventArgs e)
        {
            Set_Debug Debug = new Set_Debug();
            try
            {
                Debug.SN = tbSerial.Text;
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
            Debug.Send(ActivePDU);
        }

        private void tbSerial_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && !tbSerial.ReadOnly)
            {
                btnSetSN_Click(this, new EventArgs());
            }
        }

        private void tableLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void led1_Click(object sender, EventArgs e)
        {

        }

        private void dataGridView1_CellContentClick_1(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 0)
                for (int i = 0; i < (sender as DataGridView).RowCount; i++)
                {
                    if (i == e.RowIndex)
                        ActivePDU = PDUs.Where(x => x.IP.ToString() == dgPDUs.Rows[i].Cells[2].Value.ToString()).First();
                    (sender as DataGridView).Rows[i].Cells[i].Value = (i == e.RowIndex) ? true : false;

                }
            tmrGetStatus.Start();
        }

        private void sspc1__SwitchClicked(object sender, EventArgs e)
        {
            Set_Output set_output = new Set_Output();
            OUTPUT o = (sender as OUTPUT);
            if (o.Switch.State)
                set_output.Out(o.OUT_ID, P_Enable.Disable);
            else
                set_output.Out(o.OUT_ID, P_Enable.Enable);
            set_output.Send(ActivePDU);

        }

        private void sspc1__ResetTripsClicked(object sender, EventArgs e)
        {
            Trip_Reset trip_reset = new Trip_Reset();
            OUTPUT o = (sender as OUTPUT);
            if (o.Switch.State)
                trip_reset.Out(o.OUT_ID);
            else
                trip_reset.Out(o.OUT_ID);
            trip_reset.Send(ActivePDU);
        }

        private void tmrGetStatus_Tick(object sender, EventArgs e)
        {
            tmrGetStatus.Stop();
            new Status().Get(ActivePDU);
            new PDU_Info().Get(ActivePDU);
            new Config().Get(ActivePDU);
        }

        private void rbConfigDynamic_Click(object sender, EventArgs e)
        {
            rbConfigStatic.Checked = false;
        }

        private void rbConfigStatic_Click(object sender, EventArgs e)
        {
            rbConfigDynamic.Checked = false;
        }

        private void rbConfigStatic_CheckedChanged(object sender, EventArgs e)
        {
            if (rbConfigStatic.Checked)
            {
                gbConfigStatic.Enabled = true;
            }
            else
                gbConfigStatic.Enabled = false;
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            Set_Ip_Mode IpMode = new Set_Ip_Mode
            {
                DynamicIP = rbConfigDynamic.Checked
            };
            IPAddress addr = new IPAddress(0);
            if (!iacSelectedIp.AnyBlank)
                IpMode.IP = new IPAddress(iacSelectedIp.GetAddressBytes());
            else if (rbConfigStatic.Checked)
            {
                MessageBox.Show("Wrong IP Adress");
            }

            if (!iacSelectedMac.AnyBlank)
                IpMode.IPMask = new IPAddress(iacSelectedMac.GetAddressBytes());
            else if (rbConfigStatic.Checked)
            {
                MessageBox.Show("Wrong IP Adress");
            }
            else
                IpMode.IPMask = new IPAddress(new byte[] { 255, 255, 255, 0 });
            IpMode.Send(ActivePDU);
        }

        private void iacSelectedIp_KeyDown(object sender, KeyEventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            int[] channels = null;
            TimeSpan[] delays = null;
            try
            {
                channels = tbPorChans.Text.Split(',').Select(x => int.Parse(x)).ToArray();
                delays = tbPorDelay.Text.Split(',').Select(x => new TimeSpan(0, 0, 0, 0, int.Parse(x))).ToArray();
            }
            catch (System.FormatException)
            {
                channels = new int[0];
                delays = new TimeSpan[0];
            }

            using (Set_Por_Macro POR = new Set_Por_Macro())
            {
                POR.ChannelOn = channels;
                POR.Delay = delays;
                POR.Send(ActivePDU);
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            DialogResult dr = MessageBox.Show("All data will be erased. Continue?", "Erase Command", MessageBoxButtons.OKCancel);
            if (dr == DialogResult.OK)
            {
                using (Erase_Data Erase = new Erase_Data())
                {
                    Erase.Confirm = "ERASE EEPROM M9547";
                    Erase.Send(ActivePDU);
                }
            }
        }

        private void menuItem11_Click(object sender, EventArgs e)
        {

        }

        private void menuItem9_Click(object sender, EventArgs e)
        {

        }

        private void menuItem9_Click_1(object sender, EventArgs e)
        {
            CalibrateForm f = new CalibrateForm(this);
            f.Show();
        }

        private void BattleClicked(object sender, EventArgs e)
        {
            Set_Battle set_battle = new Set_Battle();
            if ((sender as OUTPUT).BattleLable.Text == P_Battle.Battle.ToName())
            {
                set_battle.Out((sender as OUTPUT).OUT_ID, P_Enable.Disable);
            }
            else
                set_battle.Out((sender as OUTPUT).OUT_ID, P_Enable.Enable);
            set_battle.Send(ActivePDU);
        }

        private void dgvLimits_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            // Ignore clicks that are not on button cells. 
            if (e.RowIndex < 0 || e.ColumnIndex !=
                (sender as DataGridView).Columns["clUpdate"].Index) return;

            LimitDataSource r = LimitData[e.RowIndex];
            using (Set_Limit SetLim = new Set_Limit())
            {
                SetLim.Out(r.Output, r.Overload, r.CurrentLimit, r.ThermalConst);
                SetLim.Send(ActivePDU);
            }
        }

        private void cbGroup_SelectedValueChanged(object sender, EventArgs e)
        {
            int group = GROUP.GROUPList.Find(x => x.GROUP_ID_Text.Equals(cbGroup.SelectedItem)).GROUP_ID;
            for (int i = 0; i < ActivePDU.Conf_Channel_Group.Length; i++)
                clbOutputs.SetItemChecked(i, ActivePDU.Conf_Channel_Group[i] == group);
        }

        private void btnSetGroup_Click(object sender, EventArgs e)
        {
            using (Set_Group msg = new Set_Group())
            {
                msg.Group(GROUP.GROUPList.Find(x => x.GROUP_ID_Text.Equals(cbGroup.SelectedItem)).GROUP_ID).Channels = clbOutputs.CheckedIndices.OfType<int>().Select(x => x + 1).ToArray();
                msg.Send(ActivePDU);
            }
        }

        private void btnDci_Click(object sender, EventArgs e)
        {
            int[] channels = null;
            TimeSpan[] delays = null;
            P_Enable[] operation = null;
            try
            {
                channels = tbDciChan.Text.Split(',').Select(x => int.Parse(x)).ToArray();
                delays = tbDciDelay.Text.Split(',').Select(x => new TimeSpan(0, 0, 0, 0, int.Parse(x))).ToArray();
                operation = tbDciOper.Text.Split(',').Select(x => (P_Enable)(int.Parse(x))).ToArray();
            }

            catch (System.FormatException)
            {
                channels = new int[0];
                delays = new TimeSpan[0];
                operation = new P_Enable[0];
            }

            using (Set_Dci_Macro DCI = new Set_Dci_Macro())
            {
                DCI.Channel = channels;
                DCI.Delay = delays;
                DCI.State = operation;
                DCI.Send(ActivePDU);
            }

        }

        private void dgvLimits_CellEnter(object sender, DataGridViewCellEventArgs e)
        {
            (dgvLimits.Rows[e.RowIndex].Cells[0] as DataGridViewButtonCell).Style.BackColor = Color.LightGray;
        }

        private void group1__SwitchClicked(object sender, EventArgs e)
        {
            using (Set_Output set_output = new Set_Output())
            {
                GROUP g = (sender as GROUP);
                if (g.Switch.State)
                    set_output.Group(g.GROUP_ID, P_Enable.Disable);
                else
                    set_output.Group(g.GROUP_ID, P_Enable.Enable);
                set_output.Send(ActivePDU);
            }
        }


        private void group1__ResetTripsClicked(object sender, EventArgs e)
        {
            using (Trip_Reset trip_reset = new Trip_Reset())
            {
                GROUP g = (sender as GROUP);
                trip_reset.Group(g.GROUP_ID);
                trip_reset.Send(ActivePDU);
            }
                
        }

        private void group1__BattleClicked(object sender, EventArgs e)
        {
            using (Set_Battle set_battle = new Set_Battle())
            {
                if ((sender as GROUP).BattleLable.Text == P_Battle.Battle.ToName())
                {
                    set_battle.Group((sender as GROUP).GROUP_ID, P_Enable.Disable);
                }
                else
                    set_battle.Group((sender as GROUP).GROUP_ID, P_Enable.Enable);
                set_battle.Send(ActivePDU);
            }

        }

        private void btnGroupLimits_Click(object sender, EventArgs e)
        {
            int Group = 0;
            decimal Overload = 0;
            decimal CurrentLimit = 0;
            TimeSpan ThermalConstant; 

            try
            {
                Group = GROUP.GROUPList.Find(x => x.GROUP_ID_Text.Equals(cbGroup.SelectedItem)).GROUP_ID;
                Overload = decimal.Parse(tbGroupOL.Text);
                CurrentLimit = decimal.Parse(tbGroupSC.Text);
                ThermalConstant = TimeSpan.FromMilliseconds(double.Parse(tbGroupTherm.Text));
            }
            
            catch (System.FormatException)
            {
                Group = GROUP.GROUPList.Find(x => x.GROUP_ID_Text.Equals(cbGroup.SelectedItem)).GROUP_ID;
                CurrentLimit = 4095.9375M;
                Overload = 4095.9375M;
                ThermalConstant = TimeSpan.FromMilliseconds((0xFFFF));
            }

            using (Set_Limit SetLim = new Set_Limit())
            {
                SetLim.Group(Group, Overload, CurrentLimit, ThermalConstant); 
                SetLim.Send(ActivePDU);
            }
        }
    }
}
