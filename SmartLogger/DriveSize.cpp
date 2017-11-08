#include "StdAfx.h"
#include "DriveSize.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * @param driveLetter such as 'C'
 */
CDriveSize::CDriveSize(char driveLetter)
	: driveLetter(driveLetter)
{
	// do nothing
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Construct array of drive size.
 */
CDriveSizeArray::CDriveSizeArray()
{
	DWORD dwDrive = ::GetLogicalDrives();
	for (int nDrive = 0; nDrive < 26; nDrive++)
	{
		if (dwDrive & (1 << nDrive))
		{
			char pszDrive[16];

			::sprintf_s(pszDrive, sizeof(pszDrive), "%c:\\", nDrive + 'A');

			UINT driveType = ::GetDriveTypeA(pszDrive);
			if (driveType == DRIVE_FIXED)
			{
				CDriveSize * driveSize = new CDriveSize('A' + nDrive);

				BOOL fResult =
					::GetDiskFreeSpaceExA(
						pszDrive,
						(PULARGE_INTEGER)&driveSize->i64FreeBytesToCaller,
						(PULARGE_INTEGER)&driveSize->i64TotalBytes,
						(PULARGE_INTEGER)&driveSize->i64FreeBytes);
				if (fResult)
				{
					Add(driveSize);
				}
			}
		}
	}
}

/**
 * @brief Release all drive size.
 */
CDriveSizeArray::~CDriveSizeArray()
{
	for (int i=0 ; i<GetSize() ; i++)
	{
		delete GetAt(i);
	}
}

/**
 * @brief get drive size.
 * @return drive size
 */
CDriveSize * CDriveSizeArray::GetAt(int i)
{
	return (CDriveSize *)CObArray::GetAt(i);
}
