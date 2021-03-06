// Copyright (C) 2008, 2009 by Sirrix AG security technologies
// Copyright (C) 2010 by Philipp Deppenwiese
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//


#include <TpmCryptKeyproviderMount.hxx>

using namespace tpmcrypt;

TpmCryptKeyproviderMount::TpmCryptKeyproviderMount(TpmCryptStorage &myKeyproviderStorage, UInt8 cryptoSystem, string device, string destination, string key, TpmCryptConfigfile &aTpmCryptConfig, string user, string uuid )
{
    debug << "Converting probably invalid UUID: " << uuid << endl;
    replaceCharInString(&uuid, TpmCryptPathSeparatorChar, TpmCryptFileSeparatorChar);
    debug << "UUID after conversion: " << uuid << endl;

    switch(cryptoSystem)
    {
        case VOLUMETYPE_LUKS:
            mountLUKS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_TRUECRYPT:
            mountTRUECRYPT(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_DMCRYPT:
            mountDMCRYPT(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_ECRYPTFS:
            mountECRYPTFS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_ENCFS:
            mountENCFS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        default:
            cout << "The desired cryptosystem has not been implemented!" << endl;
    }
};

TpmCryptKeyproviderMount::TpmCryptKeyproviderMount(TpmCryptStorage &myKeyproviderStorage, UInt8 cryptoSystem, string device, string destination, string key, TpmCryptConfigfile &aTpmCryptConfig, string user, string uuid )
{
    debug << "Converting probably invalid UUID: " << uuid << endl;
    replaceCharInString(&uuid, TpmCryptPathSeparatorChar, TpmCryptFileSeparatorChar);
    debug << "UUID after conversion: " << uuid << endl;

    switch(cryptoSystem)
    {
        case VOLUMETYPE_LUKS:
            umountLUKS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_TRUECRYPT:
            umountTRUECRYPT(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_DMCRYPT:
            umountDMCRYPT(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_ECRYPTFS:
            umountECRYPTFS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        case VOLUMETYPE_ENCFS:
            umountENCFS(device, destination, key, myKeyproviderStorage, user, uuid);
            break;
        default:
            cout << "The desired cryptosystem has not been implemented!" << endl;
    }
};

void TpmCryptKeyproviderMount::mounted(string destination)
{
	FILE *command;
	
	command = popen("mountpoint " + destination ,"r");
	
	if(!command)
		throw InvalidMountPoint("TpmCryptKeyproviderMount: Path already mounted by another device!");
}

void TpmCryptKeyproviderMount::mountLUKS(string device, string destination, string key, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
	mounted(destination);
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "mount_cryptsetup_luks.sh " + device + " \"tpmcrypt-" + uuid + "\"" + " " + key + " " + destination + " " + logfile);
    int ret = system(myCommand.c_str());
    if (ret)
		throw MountError("An error occured while trying to mount device '" + device + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
}

void TpmCryptKeyproviderMount::unmountLUKS(string destination, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "unmount_cryptsetup_luks.sh " + destination + " \"tpmcrypt-" + uuid + "\"" + " " + logfile);
    int ret = system(myCommand.c_str());
    if (ret)
        throw UnmountError("An error occured while trying to unmount  '" + destination + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
    myKeyproviderStorage.deleteSubSection(destination);
}

void TpmCryptKeyproviderMount::mountDMCRYPT(string device, string destination, string key, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
    mounted(destination);
    myKeyproviderStorage.selectSection(user);
    if ( !myKeyproviderStorage.findSubSection(destination))
    {// there is no other device mounted under destination
        debug << "device: " << device << "\t destination: " << destination << endl;
        string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "mount_dmcrypt.sh " + device + " \"tpmcrypt-" + uuid + "\"" + " " + key + " " + destination + " " + logfile );
        int ret = system(myCommand.c_str());
        if (ret)
            throw MountError("An error occured while trying to mount device '" + device + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");

        myKeyproviderStorage.selectSubSection(destination);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderDevice,uuid);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderCrypt_Entry,convertUInt32toString(VOLUMETYPE_LUKS));
    }
    else
    {
        string myMessage = "Device '" + device + "' already mounted on destination '" + destination + "'";
        debug << myMessage << endl;
        throw AlreadyMounted(myMessage);
    }
}

void TpmCryptKeyproviderMount::unmountDMCRYPT(string destination, TpmCryptStorage &myKeyproviderStorage, string user)
{
    string uuid = myKeyproviderStorage.getEntry(TpmCryptKeyproviderDevice);
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "unmount_dmcrypt.sh " + destination + " \"tpmcrypt-" + uuid + "\"" + " " + logfile);
    int ret = system(myCommand.c_str());
    if (ret)
        throw UnmountError("An error occured while trying to unmount '" + destination + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
    myKeyproviderStorage.deleteSubSection(destination);
}

void TpmCryptKeyproviderMount::mountTRUECRYPT(string device, string destination, string key, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
	mounted(destination);
    
    myKeyproviderStorage.selectSection(user);
    if ( !myKeyproviderStorage.findSubSection(destination))
    {// there is no other device mounted under destination
        debug << "device: " << device << "\t destination: " << destination << endl;
        string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "mount_truecrypt.sh " + device + " " + destination + " " + key + " " + logfile );
        int ret = system(myCommand.c_str());
        if (ret)
            throw MountError("An error occured while trying to mount device '" + device + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");

        myKeyproviderStorage.selectSubSection(destination);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderDevice,uuid);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderCrypt_Entry,convertUInt32toString(VOLUMETYPE_TRUECRYPT));
    }
    else
    {
        string myMessage = "Device '" + device + "' already mounted on destination '" + destination + "'";
        debug << myMessage << endl;
        throw AlreadyMounted(myMessage);
    }
}

void TpmCryptKeyproviderMount::unmountTRUECRYPT(string destination, TpmCryptStorage &myKeyproviderStorage, string user)
{
    string uuid = myKeyproviderStorage.getEntry(TpmCryptKeyproviderDevice);
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "unmount_truecrypt.sh " + destination + " " + logfile );
    int ret = system(myCommand.c_str());
    if (ret)
        throw UnmountError("An error occured while trying to unmount '" + destination + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
    myKeyproviderStorage.deleteSubSection(destination);
}

void TpmCryptKeyproviderMount::mountECRYPTFS(string device, string destination, string key, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
	mounted(destination);
    myKeyproviderStorage.selectSection(user);
    if ( !myKeyproviderStorage.findSubSection(destination))
    {// there is no other device mounted under destination
        debug << "device: " << device << "\t destination: " << destination << endl;
        string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "mount_ecryptfs.sh " + device + " " + destination + " " + key + " " + logfile );
        int ret = system(myCommand.c_str());
        if (ret)
            throw MountError("An error occured while trying to mount device '" + device + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");

        myKeyproviderStorage.selectSubSection(destination);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderDevice,uuid);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderCrypt_Entry,convertUInt32toString(VOLUMETYPE_ECRYPTFS));
    }
    else
    {
        string myMessage = "Device '" + device + "' already mounted on destination '" + destination + "'";
        debug << myMessage << endl;
        throw AlreadyMounted(myMessage);
    }
}

void TpmCryptKeyproviderMount::unmountECRYPTFS(string destination, TpmCryptStorage &myKeyproviderStorage, string user)
{
    string uuid = myKeyproviderStorage.getEntry(TpmCryptKeyproviderDevice);
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "unmount_ecryptfs.sh " + destination + " " + logfile );
    int ret = system(myCommand.c_str());
    if (ret)
        throw UnmountError("An error occured while trying to unmount device '" + destination + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
    myKeyproviderStorage.deleteSubSection(destination);
}

void TpmCryptKeyproviderMount::mountENCFS(string device, string destination, string key, TpmCryptStorage &myKeyproviderStorage, string user, string uuid)
{
	mounted(destination);
    myKeyproviderStorage.selectSection(user);
    if ( !myKeyproviderStorage.findSubSection(destination))
    {// there is no other device mounted under destination
        debug << "device: " << device << "\t destination: " << destination << endl;
        string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "mount_encfs.sh " + device + " " + destination + " " + key + " " + logfile );
        int ret = system(myCommand.c_str());
        if (ret)
            throw MountError("An error occured while trying to mount device '" + device + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");

        myKeyproviderStorage.selectSubSection(destination);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderDevice,uuid);
        myKeyproviderStorage.setEntry(TpmCryptKeyproviderCrypt_Entry,convertUInt32toString(VOLUMETYPE_ENCFS));
    }
    else
    {
        string myMessage = "Device '" + device + "' already mounted on destination '" + destination + "'";
        debug << myMessage << endl;
        throw AlreadyMounted(myMessage);
    }
}

void TpmCryptKeyproviderMount::unmountENCFS(string destination, TpmCryptStorage &myKeyproviderStorage, string user)
{
    string uuid = myKeyproviderStorage.getEntry(TpmCryptKeyproviderDevice);
    string myCommand = (myScriptDirectory + TpmCryptPathSeparator + "unmount_encfs.sh " + destination + " " + logfile );
    int ret = system(myCommand.c_str());
    if (ret)
        throw UnmountError("An error occured while trying to unmount '" + destination + "'. Please look into TpmCrypt's logfile: '" + logfile + "'");
    myKeyproviderStorage.deleteSubSection(destination);
}
