#pragma once
//Center Control
module CC
{
	///�汾��Ϣ
	struct Version
	{
		int Major;
		int Minor;
		int Build;
		int Private;
		//string��ʾ�İ汾�ţ�major number.minor number.build number.private part number
		string ProductVersion;
	};

	///����˰汾��Ϣ
	struct ServerVersion
	{
		Version ServicesVersion;
		Version AppLuncherVersion;	
	};
};