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
    public partial class GROUP: UserControl
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

        private int _group_id;
        [DefaultValue(null)]
        public int GROUP_ID
        {
            get => _group_id;
            set
            {
                _group_id = value;
                groupBox1.Text = GROUP_ID_Text;
                GROUPList.Sort(delegate (GROUP x, GROUP y) { return x.GROUP_ID.CompareTo(y.GROUP_ID); });
            }
        }
        public string GROUP_ID_Text { get=> "GROUP " + ((int)GROUP_ID).ToString(); }
        public static List<GROUP> GROUPList = new List<GROUP>();
        public Switch.Switch Switch
        {
            get => btnswitch;
        }
        public Label Voltage
        {
            get => lblVout;
        }

        public Label Current
        {
            get => lblIout;
        }
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<OUTPUT> Outputs
        {
            get
            {
                List<OUTPUT> val = new List<OUTPUT>();
                foreach (string cb in checkedListBox1.CheckedItems)
                {

                    val.Add(OUTPUT.OUTList.Find(x => x.OUT_ID_Text == cb));
                }
                return val;
            }
            set
            {
                if (checkedListBox1.Focused)
                    return;
                
                for(int i=0; i<checkedListBox1.Items.Count; i++)
                {
                    if (value.Exists(x => x.OUT_ID_Text == (checkedListBox1.Items[i] as string)))
                        checkedListBox1.SetItemChecked(i, true);
                    else
                        checkedListBox1.SetItemChecked(i, false);
                }
            }
        }


        public Label ErrorLable
        {
            get => lblError;
        }

        public Label BattleLable
        {
            get => lblBtl;
        }

        public GROUP()
        {
            InitializeComponent();
            GROUPList.Add(this);
            GROUPList.Sort(delegate (GROUP x1, GROUP x2) { return x1.GROUP_ID.CompareTo( x2.GROUP_ID); });
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


        private void toolStripMenuItem3_Click(object sender, EventArgs e)
        {
            _ResetTripsClicked?.Invoke(this, e);
        }

        private void GROUP_Load(object sender, EventArgs e)
        {
            checkedListBox1.Items.Clear();
            foreach( OUTPUT o in OUTPUT.OUTList)
            {
                checkedListBox1.Items.Add(o.OUT_ID_Text);
            }
        }


    }
}
