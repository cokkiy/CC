#pragma once

//Center Control
module CC
{
	// 版本信息
	struct Version
	{
		int Major;
		int Minor;
		int Build;
		int Private;
		//string表示的版本号，major number.minor number.build number.private part number
		string ProductVersion;
	};

	// 服务端版本信息
	struct ServerVersion
	{
		Version ServicesVersion;
		Version AppLuncherVersion;	
	};
};