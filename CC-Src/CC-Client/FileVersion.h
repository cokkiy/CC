#pragma once
#include <QString>
#include "Version.h"
class FileVersion
{
public:
	FileVersion();
	FileVersion(const FileVersion& ref);
	FileVersion(const CC::Version& v);
	FileVersion(int major, int minor, int build, int privatePart);
	~FileVersion();

	int Major;
	int Minor;
	int Build;
	int Private;
	QString ProductVersion;

	bool operator==(const FileVersion& __rhs) const
	{
		if (this == &__rhs)
		{
			return true;
		}
		if (Major != __rhs.Major)
		{
			return false;
		}
		if (Minor != __rhs.Minor)
		{
			return false;
		}
		if (Build != __rhs.Build)
		{
			return false;
		}
		if (Private != __rhs.Private)
		{
			return false;
		}
		return true;
	}

	bool operator<(const FileVersion& __rhs) const
	{
		if (this == &__rhs)
		{
			return false;
		}
		if (Major < __rhs.Major)
		{
			return true;
		}
		else if (__rhs.Major < Major)
		{
			return false;
		}
		if (Minor < __rhs.Minor)
		{
			return true;
		}
		else if (__rhs.Minor < Minor)
		{
			return false;
		}
		if (Build < __rhs.Build)
		{
			return true;
		}
		else if (__rhs.Build < Build)
		{
			return false;
		}
		if (Private < __rhs.Private)
		{
			return true;
		}
		else if (__rhs.Private < Private)
		{
			return false;
		}

		return false;
	}

	bool operator!=(const FileVersion& __rhs) const
	{
		return !operator==(__rhs);
	}
	bool operator<=(const FileVersion& __rhs) const
	{
		return operator<(__rhs) || operator==(__rhs);
	}
	bool operator>(const FileVersion& __rhs) const
	{
		return !operator<(__rhs) && !operator==(__rhs);
	}
	bool operator>=(const FileVersion& __rhs) const
	{
		return !operator<(__rhs);
	}
};

