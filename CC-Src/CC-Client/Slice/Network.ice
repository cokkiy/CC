#pragma once
[["cpp:include:list"]]
module CC
{
	["clr:generic:List"]
    sequence<string> StringList;

	struct NetworkInterfaceInfo
	{
		string Id;
		["cpp:type:wstring"]
		string Name;
		string Description;
		bool IsUp;
		string DNSSuffix;
		bool DNSEnbled;
		bool DynamicDNS;
		StringList IPAddresses;	
		StringList MulticastAddresses;
		StringList DhcpServers;
	};

	struct ConnectionInformation
	{
		string LocalAddress;
		int LocalPort;
		string Remote;
		int RemotePort;
		string State;
	};

	struct ListenerInfo
	{
		string Address;
		int Port;
	};

	struct InterfaceStatistics
	{
	    //bool HasValue;
		string IfName;
		float BytesReceivedPerSec;
		float BytesSentedPerSec;
		float TotalBytesPerSec;

		long BytesReceived;
		long BytesSented;
		long BytesTotal;

		long UnicastPacketReceived;
		long UnicastPacketSented;
		long MulticastPacketReceived;
		long MulticastPacketSented;
	};

	["clr:generic:List"]
	sequence<InterfaceStatistics> InterfacesStatistics;

	struct Statistics
	{
		string StationId;
		long DatagramsReceived;
		long DatagramsSent;
		long DatagramsDiscarded;
		long DatagramsWithErrors;
		int UDPListeners;

		long SegmentsReceived;
		long SegmentsSent;
		long ErrorsReceived;
		long CurrentConnections;
		long ResetConnections;	
		
		InterfacesStatistics IfStatistics;		
	};

	["clr:generic:List"]
	sequence<NetworkInterfaceInfo> NetworkInterfaceInfos;
	["clr:generic:List"]
	sequence<ConnectionInformation> ConnectionInformations;
	["clr:generic:List"]
	sequence<ListenerInfo> ListenerInfos;
};