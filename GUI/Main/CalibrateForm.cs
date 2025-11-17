using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace M9547
{
    public partial class CalibrateForm : Form
    {
        Form1 form_ref;
        public CalibrateForm(Form1 f)
        {
            InitializeComponent();
            comboBox1.Items.Clear();
            comboBox1.Items.Add("Input");
            comboBox1.Items.AddRange(OUTPUT.OUTList.Select(x => x.OUT_ID_Text).ToArray());
            form_ref = f;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        { 
                groupBox1.Enabled = rbCalibrateSlope.Checked;
        }


        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
