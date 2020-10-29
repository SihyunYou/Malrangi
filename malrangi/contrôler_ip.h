#pragma once
#include "malrangi.h"

class IpManage
{

public:
    unsigned int AdresseActuel;
    IpManage() : AdresseActuel(0) {}

    enum class ETAT_DE_RENOUVELLEMENT
    {

    };
	
    std::string ssystem(const char* command) {
        char tmpname[L_tmpnam];
        std::tmpnam(tmpname);
        std::string scommand = command;
        std::string cmd = scommand + " >> " + tmpname;
        std::system(cmd.c_str());
        std::ifstream file(tmpname, std::ios::in | std::ios::binary);
        std::string result;
        if (file) {
            while (!file.eof()) result.push_back(file.get())
                ;
            file.close();
        }
        remove(tmpname);
        return result;
    }
    bool Renouveler(const unsigned int Adresse)
    {
        if (0 == AdresseActuel)
        {
            AdresseActuel = Adresse;
        }
        else if (Adresse != AdresseActuel)
        {
            AdresseActuel = Adresse;
        }

        system("taskkill /im chrome.exe >nul 2>nul");
        string s;
        char a[100];
        GetCurrentDirectoryA(100, a);
        s = ssystem((string(IPMANAGER_PATH) + " " + to_string(Adresse)).c_str());
        Sleep(1000);
        if (s.find("vrai") != string::npos)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    bool IsNetworkConnected()
    {
        string s;
        s = ssystem("netsh wlan show interfaces");

        if (s.find("iptime5G") != string::npos)
        {
            return true;
        }

        return false;
    }
    void ConnectNetwork()
    {
        system("netsh wlan connect name = \"iptime5G\" >nul 2>nul");
        Sleep(2500);
    }


};