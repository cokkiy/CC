#pragma once
[["cpp:include:list"]]
module CC
{
	["clr:generic:List"]
    sequence<string> StringList;

	///网卡信息
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
		//IP 地址
		StringList IPAddresses;	
		// 组播地址
		StringList MulticastAddresses;

		//DHCP 服务器
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

	//网卡流量统计信息
	struct InterfaceStatistics
	{
	    //bool HasValue;
		//网卡名称
		string IfName;
		// 接收发送流量信息
		float BytesReceivedPerSec;
		float BytesSentedPerSec;
		float TotalBytesPerSec;

		long BytesReceived;
		long BytesSented;
		long BytesTotal;

		// 接收发送包信息
		long UnicastPacketReceived;
		long UnicastPacketSented;
		long MulticastPacketReceived;
		long MulticastPacketSented;
	};

	["clr:generic:List"]
	sequence<InterfaceStatistics> InterfacesStatistics;

	//统计信息
	struct Statistics
	{
		string StationId;
		// UDP信息
		long DatagramsReceived;
		long DatagramsSent;
		long DatagramsDiscarded;
		long DatagramsWithErrors;
		int UDPListeners;

		//TCP信息
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