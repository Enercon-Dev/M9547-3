using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Data
{
    public enum Led_Type : byte
    {
        OFF = 0xFE,
        GREEN = 0x00,
        BLINK_GREEN = 0x04,
        RED = 0x01,
        BLINK_RED = 0x02,
        RED_GREEN = 0x08,
        NA = 0xFF
    }
    public partial class Led : Label
    {
        public Led_Type Status;
        [Browsable(false)]
        public new Boolean AutoSize;
        [Browsable(false)]
        public new System.Drawing.Color BackColor;
        [Browsable(false)]
        public new System.Windows.Forms.BorderStyle BorderStyle;
        [Browsable(false)]
        public new string Text;

        public Led()
        {
            InitializeComponent();
        }

        public Led(IContainer container)
        {
            container.Add(this);

            InitializeComponent();
            timer1.Enabled = true;
        }

        private void Timer1_Tick(object sender, System.EventArgs e)
        {
            switch (Status)
            {
                case Led_Type.OFF:
                    base.BackColor = System.Drawing.Color.Silver;
                    break;
                case Led_Type.BLINK_GREEN:
                    if (base.BackColor == System.Drawing.Color.Silver)
                        base.BackColor = System.Drawing.Color.Green;
                    else
                        base.BackColor = System.Drawing.Color.Silver;
                    break;
                case Led_Type.GREEN:
                    base.BackColor = System.Drawing.Color.Green;
                    break;
                case Led_Type.RED:
                    base.BackColor = System.Drawing.Color.Red;
                    break;
                case Led_Type.BLINK_RED:
                    if (base.BackColor == System.Drawing.Color.Silver)
                        base.BackColor = System.Drawing.Color.Red;
                    else
                        base.BackColor = System.Drawing.Color.Silver;
                    break;
                default:
                    break;
            }
            
        }

    }
}
