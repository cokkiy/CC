#pragma once
[["cpp:include:list"]]
module CC
{
	["cpp:type:std::list<std::string>"]	
    sequence<string> StringList;

	///网卡信息
	struct NetworkInterfaceInfo
	{
		string Id;
		["cpp:type:wstring"]
		string Name;
		["cpp:type:wstring"]
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

	["cpp:type:std::list<CC::InterfaceStatistics>"]
	sequence<CC::InterfaceStatistics> InterfacesStatistics;

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
		int CurrentConnections;
		int ResetConnections;

		InterfacesStatistics IfStatistics;		
	};
	["cpp:type:std::list<NetworkInterfaceInfo>"]
	sequence<NetworkInterfaceInfo> NetworkInterfaceInfos;
	["cpp:type:std::list<ConnectionInformation>"]
	sequence<ConnectionInformation> ConnectionInformations;
	["cpp:type:std::list<ListenerInfo>"]
	sequence<ListenerInfo> ListenerInfos;
};