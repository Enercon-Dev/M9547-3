using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Drawing.Imaging;
using System.Net.NetworkInformation;

namespace Data
{

    public class Topology
    {
        public IPAddress Filter_IP;
        public string ID;
        public PhysicalAddress Filter_MAC;
        public List<Topology> Right;
        public List<Topology> Left;
        private Control Parent;
        public Bitmap canvas;
        public System.EventHandler FilterButton_Handler = null;

        public Topology()
        {
            Right = new List<Topology>();
            Left = new List<Topology>();
            Filter_IP = new IPAddress(new byte[4] { 255, 255, 255, 255 });
            Filter_MAC = new PhysicalAddress(new byte[6] { 255, 255, 255, 255, 255, 255 });
        }

        public void CreateTopology(List<Filter> l_filters)
        {
            Right.Clear();
            Left.Clear();
            foreach (Filter filt in l_filters)
            {
                Topology t_filt = new Topology();
                t_filt.Filter_MAC = filt.Filter_MAC;
                t_filt.Filter_IP = filt.Filter_IP;
                t_filt.ID = filt.ID;
                if (filt.RSU_OnRight)
                    Left.Add(t_filt);
                else
                    Right.Add(t_filt);
            }
            List<Topology> toDel = new List<Topology>();
            foreach (Topology t_filt in Right)
            {
                Filter filt = l_filters.Find(x => x.Filter_IP == t_filt.Filter_IP);
                t_filt.Right = Right.FindAll(x => filt.Right.Contains(x.Filter_MAC));
                toDel.AddRange(t_filt.Right);

            }

            Right.RemoveAll(x => toDel.Contains(x));
            toDel.Clear();

            foreach (Topology t_filt in Left)
            {
                Filter filt = l_filters.Find(x => x.Filter_IP == t_filt.Filter_IP);
                t_filt.Left = Left.FindAll(x => filt.Left.Contains(x.Filter_MAC));
                toDel.AddRange(t_filt.Left);


            }
            Left.RemoveAll(x => toDel.Contains(x));

        }

        public void DrawGraphRight(RadioButton node, Topology tpl = null)
        {
            List<RadioButton> cmp_lst = new List<RadioButton>();
            if (tpl == null)
            {
                tpl = this;
                Parent = node.Parent;
                foreach (Control btn in Parent.Controls.Find("FilterIPNodes", true))
                {
                    Parent.Controls.Remove(btn);
                }
                Bitmap tmp = new Bitmap(Parent.Width, Parent.Height, PixelFormat.Format32bppRgb);
                tmp.MakeTransparent();
                using (Graphics g = Graphics.FromImage(tmp))
                {
                    g.Clear(Parent.BackColor);
                    if (canvas != null)
                    {
                        g.DrawImage(tmp, 0, 0);
                        canvas.Dispose();
                    }
                }
                canvas = tmp;
                DrawGraphRight(node, tpl);
                
                return;
            }

            Point pnt = node.Location;
            pnt.Offset(120, 0);
            foreach (Topology newnode in tpl.Right)
            {
                RadioButton newradio = new RadioButton();
                newradio.Appearance = System.Windows.Forms.Appearance.Button;
                newradio.Location = pnt;
                newradio.Name = "FilterIPNodes";
                newradio.Size = new System.Drawing.Size(100, 25);
                newradio.TabIndex = 0;
                newradio.Text = newnode.ID;
                newradio.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
                newradio.UseVisualStyleBackColor = false;
                newradio.CheckedChanged += FilterButton_Handler;
                pnt.Offset(0, 35);
                
                Parent.Controls.Add(newradio);
                   

                using (Graphics g = Graphics.FromImage(canvas))
                {
                    Point end = newradio.Location;
                    end.Offset(0, newradio.Height / 2);
                    Point begin = node.Location;
                    begin.Offset(node.Width, node.Height / 2);
                    g.DrawLine(new Pen(Color.Black, 1), begin, end);
                }


                DrawGraphRight(newradio, newnode);


                //newradio.Click += new System.EventHandler(this.radioButton4_Click);
            }
            return;
        }

        public void DrawGraphLeft(RadioButton node, Topology tpl = null)
        {
            List<RadioButton> cmp_lst = new List<RadioButton>();
            if (tpl == null)
            {
                tpl = this;
                Parent = node.Parent;
                DrawGraphLeft(node, tpl);
                //Parent.CreateGraphics().Clear(Parent.BackColor);
                Parent.CreateGraphics().DrawImage(canvas, 0, 0);
                Parent.Refresh();
                return;
            }
            Point pnt = node.Location;
            pnt.Offset(-120, 0);
            foreach (Topology newnode in tpl.Left)
            {
                RadioButton newradio = new RadioButton();
                newradio.Appearance = System.Windows.Forms.Appearance.Button;
                newradio.Location = pnt;
                newradio.Name = "FilterIPNodes";
                newradio.Size = new System.Drawing.Size(100, 25);
                newradio.TabIndex = 0;
                newradio.Text = newnode.Filter_IP.ToString();
                newradio.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
                newradio.UseVisualStyleBackColor = false;
                newradio.CheckedChanged += FilterButton_Handler;
                pnt.Offset(0, 35);

                Parent.Controls.Add(newradio);


                using (Graphics g = Graphics.FromImage(canvas))
                {
                    Point end = newradio.Location;
                    end.Offset(newradio.Width, newradio.Height / 2);
                    Point begin = node.Location;
                    begin.Offset(0, node.Height / 2);
                    g.DrawLine(new Pen(Color.Black, 1), begin, end);
                }


                DrawGraphLeft(newradio, newnode);


                //newradio.Click += new System.EventHandler(this.radioButton4_Click);
            }
            return;
        }

    }



}
