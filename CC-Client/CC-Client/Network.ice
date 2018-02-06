#pragma once
[["cpp:include:list"]]
module CC
{
	["cpp:type:std::list<std::string>"]	
    sequence<string> StringList;

	///������Ϣ
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
		//IP ��ַ
		StringList IPAddresses;	
		// �鲥��ַ
		StringList MulticastAddresses;

		//DHCP ������
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

	//��������ͳ����Ϣ
	struct InterfaceStatistics
	{
		//bool HasValue;
		//��������
		string IfName;
		// ���շ���������Ϣ
		float BytesReceivedPerSec;
		float BytesSentedPerSec;
		float TotalBytesPerSec;

		long BytesReceived;
		long BytesSented;
		long BytesTotal;

		// ���շ��Ͱ���Ϣ
		long UnicastPacketReceived;
		long UnicastPacketSented;
		long MulticastPacketReceived;
		long MulticastPacketSented;
	};

	["cpp:type:std::list<CC::InterfaceStatistics>"]
	sequence<CC::InterfaceStatistics> InterfacesStatistics;

	//ͳ����Ϣ
	struct Statistics
	{
	    string StationId;
		// UDP��Ϣ
		long DatagramsReceived;
		long DatagramsSent;
		long DatagramsDiscarded;
		long DatagramsWithErrors;
		int UDPListeners;

		//TCP��Ϣ
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