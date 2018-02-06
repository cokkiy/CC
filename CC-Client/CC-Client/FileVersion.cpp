#include "FileVersion.h"



FileVersion::FileVersion()
{
	Major = 0;
	Minor = 0;
	Build = 0;
	Private = 0;
}

FileVersion::FileVersion(const FileVersion & ref)
{
	Major = ref.Major;
	Minor = ref.Minor;
	Build = ref.Build;
	Private = ref.Private;
	ProductVersion = QStringLiteral("").arg(Major).arg(Minor).arg(Build).arg(Private);
}

FileVersion::FileVersion(const CC::Version & v)
{
	Major = v.Major;
	Minor = v.Minor;
	Build = v.Build;
	Private = v.Private;	
	ProductVersion = QStringLiteral("").arg(Major).arg(Minor).arg(Build).arg(Private);
}

FileVersion::FileVersion(int major, int minor, int build, int privatePart)
{
	Major = major;
	Minor = minor;
	Build = build;
	Private = privatePart;
	ProductVersion = QStringLiteral("").arg(Major).arg(Minor).arg(Build).arg(Private);
}


FileVersion::~FileVersion()
{
}
