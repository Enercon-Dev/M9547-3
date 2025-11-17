using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Data
{
    public class LimitDataSource
    {
        public string strOutput { get => Output.ToString(); }
        public string strChannel { get => Channel.ToString(); }
        public string strOverload { get => Overload.ToString(); set => Overload = decimal.Parse(value); }
        public string strCurrentLimit { get => CurrentLimit.ToString(); set => CurrentLimit = decimal.Parse(value); }
        public string strThermal { get => ThermalConst.TotalMilliseconds.ToString(); set => ThermalConst = new TimeSpan(0,0,0,0,int.Parse(value)); }

        public int Output;
        public int Channel;
        public decimal Overload;
        public decimal CurrentLimit;
        public TimeSpan ThermalConst;


        public LimitDataSource(int nChannel)
        {
            Output = nChannel;
            Channel = nChannel;
            Overload = 0;
            CurrentLimit = 0;
            ThermalConst = TimeSpan.FromMilliseconds(0);

        }

    }
}
