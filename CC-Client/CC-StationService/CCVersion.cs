using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC
{
    partial class Version
    {
        public static implicit operator Version(System.Diagnostics.FileVersionInfo version)
        {
            Version v = new Version();
            v.Major = version.FileMajorPart;
            v.Minor = version.FileMinorPart;
            v.Build = version.FileBuildPart;
            v.Private = version.FilePrivatePart;
            v.ProductVersion = version.ProductVersion;
            return v;
        }
    }
}
