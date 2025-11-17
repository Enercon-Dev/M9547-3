namespace Data
{
    partial class Led
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            // 
            // label1
            // 
            base.AutoSize = false;
            base.BackColor = System.Drawing.Color.Silver;
            base.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            base.Size = new System.Drawing.Size(23, 23);
            base.Text = "";
            // 
            // timer1
            // 
            this.timer1.Interval = 500;
            this.timer1.Tick += Timer1_Tick;

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
    }
}
