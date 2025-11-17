using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace M9547
{
    [RunInstaller(true)]
    public partial class Installer : System.Configuration.Install.Installer
    {
        public Installer()
        {
            InitializeComponent();
        }

        protected override void OnAfterInstall(IDictionary savedState)
        {
            base.OnAfterInstall(savedState);

            var path = Path.GetDirectoryName(Context.Parameters["assemblypath"]);
            OpenFirewallForProgram(Path.Combine(path, "M9547.exe"),
                                   "MPS-M9547-PDU");
        }


        protected override void OnAfterUninstall(IDictionary savedState)
        {
            base.OnAfterUninstall(savedState);
            CloseFirewallForProgram("MPS-M9547-PDU");

        }
        private static void OpenFirewallForProgram(string exeFileName, string displayName)
        {
            var proc = Process.Start(
                new ProcessStartInfo
                {
                    FileName = "netsh",
                    Arguments =
                            string.Format(
                                "firewall add allowedprogram program=\"{0}\" name=\"{1}\" profile=\"ALL\"",
                                exeFileName, displayName),
                    WindowStyle = ProcessWindowStyle.Hidden
                });
            proc.WaitForExit();
        }

        private static void CloseFirewallForProgram(string displayName)
        {
            var proc = Process.Start(
                new ProcessStartInfo
                {
                    FileName = "netsh",
                    Arguments =
                            string.Format(
                                "advfirewall firewall delete rule name=\"{0}\"",
                                displayName),
                    WindowStyle = ProcessWindowStyle.Hidden
                });
            proc.WaitForExit();
        }
    }
}
