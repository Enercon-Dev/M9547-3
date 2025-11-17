
using System.Collections.Generic;
using System.Windows.Forms;

namespace M9547.Switch
{
    public partial class Switch : Button
    {
        

        public Switch():base()
        {
            InitializeComponent();
            sent_value = On_State.UNKNOWN;
            sent_ack = Valid_Type.UNKNOWN;
            recieved_value = false;
            state = false;
        }

        public enum Valid_Type { OK, FAIL, UNKNOWN };

        public enum On_State { On, Off, UNKNOWN };

        public bool Sent_Value
        {
            set
            {
                sent_value = value ? On_State.On : On_State.Off;
                sent_ack = Valid_Type.UNKNOWN;
            }
        }

        public Valid_Type Sent_Ack
        {
            set
            {
                sent_ack = value;
                switch (sent_ack)
                {
                    case Valid_Type.OK:
                        this.BackColor = System.Drawing.Color.Yellow;
                        /*if (sent_value == On_State.On)
                            this.Image = Switch_Res.sw_on.ToBitmap();
                        else
                            this.Image = Switch_Res.sw_off.ToBitmap();
                        state = (sent_value == On_State.On);*/
                        break;
                    default:
                        this.BackColor = System.Drawing.Color.PaleVioletRed;
                        break;
                }
            }
        }

        public bool Recieved_Value
        {
            set
            {
                recieved_value = value;
                if (recieved_value)
                    this.Image = Switch_Res.sw_on.ToBitmap();
                else
                    this.Image = Switch_Res.sw_off.ToBitmap();
                state = recieved_value;
                if (sent_value == On_State.UNKNOWN)
                {
                    this.BackColor = System.Drawing.Color.Transparent;
                    return;
                }
                if (recieved_value == (sent_value == On_State.On))
                    this.BackColor = System.Drawing.Color.Transparent;
                else
                    this.BackColor = System.Drawing.Color.Red;
                //sent_value = recieved_value;
            }
        }
        public bool State
        {
            get => state;
        }

        public void ClearColor()
        {
            sent_value = recieved_value ? On_State.On : On_State.Off;
            this.BackColor = System.Drawing.Color.Transparent;
        }
        private On_State        sent_value;
        private Valid_Type  sent_ack;
        private bool        recieved_value;
        private bool        state;
        new public string Text
        {
            set { }
            get => "";
        }

    }
}
