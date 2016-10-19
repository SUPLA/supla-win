/*
============================================================================
Name        : SuplaClient.cs
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

using System;
using SuplaRuntimeComponent;
using Windows.UI.Xaml;
using Windows.Security.ExchangeActiveSyncProvisioning;

namespace SUPLA
{
    class SuplaClient : DependencyObject
    {
        private SuplaClient4Win_RTC sc_rtc = null;

        public SuplaClient()
        {

            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;

            byte[] GUID = (byte[])localSettings.Values["GUID"];

            if ( GUID == null )
            {
                byte[] _GUID = new byte[16];
                Random rnd = new Random();
                rnd.NextBytes(_GUID);

                localSettings.Values["GUID"] = _GUID;
                GUID = _GUID;
            }

            EasClientDeviceInformation i = new EasClientDeviceInformation();

            System.Diagnostics.Debug.WriteLine(i.FriendlyName+"\n");
            System.Diagnostics.Debug.WriteLine(i.SystemManufacturer + "\n");
            System.Diagnostics.Debug.WriteLine(i.SystemProductName + "\n");

            sc_rtc = new SuplaClient4Win_RTC();
            sc_rtc.setClientGUID(GUID);
            sc_rtc.setPort(2015);
            sc_rtc.setSSLEnabled(false);

            String Name = i.FriendlyName;
                   
            if (Name.Equals("Windows Phone") || Name.Equals("") || Name == null )
            {
                Name = i.SystemManufacturer + " " + i.SystemProductName;
            }

            sc_rtc.setName(Name.Trim());
            sc_rtc.setOnVersionErrorCallback(onVersionError);
            sc_rtc.setOnConnErrorCallback(onConnError);
            sc_rtc.setOnConnectingCallback(onConnecting);
            sc_rtc.setOnConnectedCallback(onConnected);
            sc_rtc.setOnDisconnectedCallback(onDisconnected);
            sc_rtc.setOnRegisteringCallback(onRegistering);
            sc_rtc.setOnRegisteredCallback(onRegistered);
            sc_rtc.setOnRegisterErrorCallback(onRegisterError);
            sc_rtc.setLocationUpdateCallback(locationUpdate);
            sc_rtc.setChannelUpdateCallback(channelUpdate);
            sc_rtc.setChannelUpdateValueCallback(channelValueUpdate);
            sc_rtc.setOnEventCallback(onEvent);
        }

        ~SuplaClient()
        {
            stop();
        }

        private void addLog(String log)
        {

#pragma warning disable CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
            Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                MainPage.getCurrentInstance().addLog(log);
            });
#pragma warning restore CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed

        }

        public void start()
        {
            sc_rtc.setHost(MainPage.getCurrentInstance().getServer());
            sc_rtc.setAccessID(MainPage.getCurrentInstance().getAccessID());
            sc_rtc.setAccessIDpwd(MainPage.getCurrentInstance().getPassword());

            sc_rtc.start();
        }

        public void stop()
        {
            sc_rtc.stop();
        }

        private void onVersionError( int version, int remote_version_min, int remote_version)
        {
            addLog("Version Error version:"+version.ToString()+", remote_version_min:"+ remote_version_min.ToString()+ ", remote_version:"+remote_version.ToString());
        }

        private void onConnError(int code)
        {
            addLog("ConnError code:" + code.ToString());
        }

        private void onConnecting()
        {
            addLog("Connecting...");
        }

        private void onConnected()
        {
            addLog("Connected");
        }

        private void onDisconnected()
        {
            addLog("Disconnected");
        }

        private void onRegistering()
        {
            addLog("Registering");
        }

        private void onRegistered(SuplaRegisterResult registerResult)
        {
            addLog("Registered ");
            addLog("        ResultCode: "+ registerResult.getResultCode().ToString());
            addLog("        ClientID: " + registerResult.getClientID());
            addLog("        LocationCount: " + registerResult.getLocationCount());
            addLog("        ChannelCount: " + registerResult.getChannelCount());
            addLog("        ActivityTimeout: " + registerResult.getActivityTimeout());
            addLog("        Version: " + registerResult.getVersion());
            addLog("        VersionMin: " + registerResult.getVersionMin());

        }

        private void onRegisterError(int code)
        {
            addLog("RegisterError code:" + code.ToString());
        }

        private void locationUpdate(SuplaLocation location)
        {
            addLog("locationUpdate ");
            addLog("       Caption: " + location.getCaption());
            addLog("       EOL: " + location.getEOL().ToString());
            addLog("       Id: " + location.getId().ToString());
         }

        private void channelUpdate(SuplaChannel channel)
        {
            addLog("channelUpdate ");
            addLog("       EOL: " + channel.getEOL().ToString());
            addLog("       Id: " + channel.getId().ToString());
            addLog("       LocationID: " + channel.getLocationID().ToString());
            addLog("       Func: " + channel.getFunc().ToString());
            addLog("       OnLine: " + channel.getOnLine().ToString());
            addLog("       Caption: " + channel.getCaption());

        }

        private void channelValueUpdate(SuplaChannelValueUpdate channel_value)
        {
            addLog("channelValueUpdate ");

            addLog("       EOL: " + channel_value.getEOL().ToString());
            addLog("       Id: " + channel_value.getId().ToString());
            addLog("       OnLine: " + channel_value.getOnLine().ToString());

        }

        private void onEvent(SuplaEvent ev)
        {
            addLog("event ");

            addLog("       Owner: " + ev.getOwner().ToString());
            addLog("       Event: " + ev.getEvent().ToString());
            addLog("       ChannelID: " + ev.getChannelID().ToString());
            addLog("       DurationMS: " + ev.getDurationMS().ToString());
            addLog("       SenderID: " + ev.getSenderID().ToString());
            addLog("       SenderName: " + ev.getSenderName());
  

        }

        public Boolean open(int channelId, byte _open)
        {
            return sc_rtc.open(channelId, _open);
        }
        public Boolean setRGBW(int channelId, int color, byte color_brightness, byte brightness)
        {
            return sc_rtc.setRGBW(channelId, color, color_brightness, brightness);
        }

        public Boolean setDimmer(int channelId, byte brightness)
        {
            return sc_rtc.setDimmer(channelId, brightness);
        }

    }
}
