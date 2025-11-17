
namespace M9547.Switch
{
    public partial class Switch
    {

        protected override void Dispose(bool disposing)
        {

            base.Dispose(disposing);
        }


        private void InitializeComponent()
        {
            this.BackColor = System.Drawing.Color.Transparent;
            this.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.ForeColor = System.Drawing.Color.White;
            this.Image = Switch_Res.sw_off.ToBitmap();
            //this.Image = Switch_Res.sw_on.ToBitmap();
            //this.Size = new System.Drawing.Size(75, 23);
            this.UseVisualStyleBackColor = false;
            base.Text = "";

        }


    }
}
