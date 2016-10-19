using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SUPLA
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private SuplaClient suplaClient = new SuplaClient();
        private static MainPage currentInstance;

        public MainPage()
        {
            this.InitializeComponent();
            currentInstance = this;
        }

        public static MainPage getCurrentInstance()
        {
            return currentInstance;
        }

        public void addLog(String log)
        {
            
            listBox.Items.Add(log);
            var lastItem = listBox.Items[listBox.Items.Count - 1];

            listBox.SelectedItem = lastItem;
            listBox.ScrollIntoView(lastItem);
            
        }

        public String getServer()
        {
            return tbServer.Text;
        }

        public int getAccessID()
        {
            return int.Parse(tbAccessID.Text);
        }

        public String getPassword()
        {
            return tbPassword.Text;
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            suplaClient.start();
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            suplaClient.stop();
        }

        private void button2_Click(object sender, RoutedEventArgs e)
        {
            suplaClient.open(int.Parse(tbChannelId.Text), 1);
        }

        private void button3_Click(object sender, RoutedEventArgs e)
        {
            suplaClient.open(int.Parse(tbChannelId.Text), 0);
        }
    }
}
