/**
\defgroup BlackBerry

These are some brief notes to get you started. If anything is unclear, drop me a line at ag@em.net.

The BlackBerry code is all in the namespace ZBlackBerry.

Most entities in this API suite are ultimately derived from ZCounted, and thus
we use refcounted pointers of the form ZRef<XXX> rather than using unadorned XXX*.

There are three entities with which you'll be working: Manager, Device and Channel.

An instance of Manager provides access to a roster of BlackBerrys. In the common case that will
be one BlackBerry connected by USB. Registering with the Manager as an Observer will cause your
ManagerChanged method to be invoked whenever the roster has or may have changed.

Device represents a single connected BlackBerry. Register with the Device as an Observer to
be told when it has been disconnected. Device's other job is to open channels to software
running on the BlackBerry.

Channel represents the connection to a channel on a BlackBerry. It's a subclass of ZStreamerRWCon,
adding the GetIdealSize_Read and GetIdealSize_Write methods. For detailed information on
ZStreamerRWCon see ZStreamer.cpp.

You can see an example of real code in ZBlackBerryServer -- it's made more complex by the
details of safely handling multiple connections simultaneously. The sample application
zoolib_samples/BlackBerry/BBDaemon provides access to a ZBlackBerryServer over a TCP
connection, and thus allows multiple apps to talk to multiple BlackBerrys even on OSX.

The essentials of working with ZBlackBerry are demonstrated in this code:

\code
void Sample()
	{
	using namespace ZBlackBerry;

	// Instantiate the Manager. On Windows you would create a Manager_BBDevMgr.
	// If you're using BBDaemon you'd create a Manager_Client, passing a ZStreamerRWConFactory
	// that will open connections to the appropriate TCP or other address.
	ZRef<Manager> theManager = new Manager_OSXUSB;

	// Get the IDs of all BlackBerrys handled by this Manager.
	// The ID of a Device is non-zero, opaque and unique within a Manager.
	vector<uint64> theDeviceIDs;
	theManager->GetDeviceIDs(theDeviceIDs);

	if (theDeviceIDs.size())
		{
		// We found at least one. Open the first.
		const uint64 theDeviceID = theDeviceIDs[0];
		if (ZRef<Device> theDevice = theManager->Open(theDeviceID))
			{
			// The device opened okay. Just because you have the ID for a device
			// that is no guarantee that the device is still there -- it can
			// get unplugged at any time. When a device is unplugged its ID
			// is retired and will never again be used by the Manager that allocated it.

			// Now open a Channel to "MyAppChannel". We're passing nil for the
			// password hash parameter, indicating that we do not know the device's
			// password. We're passing nil for the oError output parameter, indicating
			// that we don't care to know the precise reason for a failure.
			if (ZRef<Channel> theChannel = theDevice->Open("MyAppChannel", nullptr, nullptr))
				{
				// We can now send and receive data over theChannel. As with any other
				// streamer we can take a local reference to the stream(s) it encapsulates.				
				const ZStreamWCon& w = theChannel->GetStreamWCon();
				const ZStreamRCon& r = theChannel->GetStreamRCon();

				// Send a string
				w.WriteString("Hello MyAppChannel");
				// And a terminating zero byte.
				w.WriteUInt8(0);
				
				// Just in case there's anything funky happening (buffers, filters etc)
				// it is advisable to flush a write stream before reading from an
				// associated read stream.
				w.Flush();

				// Read the response, whatever it might be.
				bool result = r.ReadBool();

				// Close our send direction.
				w.SendDisconnect();

				// Suck up and discard any outstanding data until the receive direction closes.
				r.ReceiveDisconnect(-1);
				}
			}
		}
	// And by the magic of refcounting, Channels, Devices and Managers will cleanly
	// disconnect and dispose when they are no longer in use.
	}
\endcode
*/
