using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Data;

namespace M9547
{
    public partial class ChoosePDUs : Form
    {
        public List<PDU> Selected_PDUs
        {
            get
            {
                List<PDU> lf = new List<PDU>();
                foreach (ListViewItem lvi in lvFilters.CheckedItems)
                {
                    lf.Add(m_filter_list.Find(x => x.IP.ToString() == lvi.Text));
                }
                return lf;
            }
        }
        private List<PDU> m_filter_list;
        public ChoosePDUs(List<PDU> filter_list)
        {
            InitializeComponent();
            m_filter_list = filter_list;
            lvFilters.Items.AddRange(filter_list.Select(x => new ListViewItem(x.IP.ToString())).ToArray());
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnConfirm_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            
        }
    }
}
