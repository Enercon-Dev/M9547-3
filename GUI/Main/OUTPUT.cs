using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace M9547
{
    [Serializable()]
    public partial class OUTPUT : UserControl
    {
        public event EventHandler _VoutRequested;
        public event EventHandler _IoutRequested;
        public event EventHandler _SwitchClicked;
        public event EventHandler _BattleClicked;
        public event EventHandler _ResetTripsClicked;

        public class BattleEventArgs : EventArgs
        {
            public BattleEventArgs(bool val)
            {
                BattleMode = val;
            }
            public bool BattleMode { get; }
        }

        private int _out_id;
        [DefaultValue(null)]
        public int OUT_ID {
            get => _out_id;
            set
            {
                _out_id = value;
                groupBox1.Text = OUT_ID_Text;
                OUTList.Sort(delegate (OUTPUT x, OUTPUT y) { return x.OUT_ID.CompareTo(y.OUT_ID); });
            }
        }
        public string OUT_ID_Text { get=> "OUT " + ((int)OUT_ID).ToString(); }
        public static List<OUTPUT> OUTList = new List<OUTPUT>();
        public Switch.Switch Switch
        {
            get => btnswitch;
        }
        public Label Voltage
        {
            get => lblVout;
        }

        public Label Temp
        {
            get => lblTemp;
        }

        public Label Current
        {
            get => lblIout;
        }



        public Label ErrorLable
        {
            get => lblError;
        }

        public Label BattleLable
        {
            get => lblBtl;
        }

        public OUTPUT()
        {
            InitializeComponent();
            OUTList.Add(this);
            OUTList.Sort(delegate (OUTPUT x1, OUTPUT x2) { return x1.OUT_ID.CompareTo( x2.OUT_ID); });
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            if (lblBtl.Text == Data.Message.P_Battle.Battle.ToString())
            {
                toolStripMenuItem1.Enabled = false;
                toolStripMenuItem2.Enabled = true;
            }
            else
            {
                toolStripMenuItem1.Enabled = true;
                toolStripMenuItem2.Enabled = false;
            }
        }

        private void btnswitch_Click(object sender, EventArgs e)
        {
            _SwitchClicked?.Invoke(this, e);
        }

        private void lblVout_Click(object sender, EventArgs e)
        {
            _VoutRequested?.Invoke(this, e);
        }

        private void lblIout_Click(object sender, EventArgs e)
        {
            _IoutRequested?.Invoke(this, e);
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            BattleEventArgs ne = new BattleEventArgs(true);
            _BattleClicked?.Invoke(this, ne);
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            BattleEventArgs ne = new BattleEventArgs(false);
            _BattleClicked?.Invoke(this, ne);
        }

        private void OUT_Load(object sender, EventArgs e)
        {

        }

        private void toolStripMenuItem3_Click(object sender, EventArgs e)
        {
            _ResetTripsClicked?.Invoke(this, e);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
