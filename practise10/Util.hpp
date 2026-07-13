
#pragma once 
#include<iostream>
#include<string>
#include<fstream>



class Util
{
public:
    static bool ReadFileContent(const std::string& filename,std::string* out)
    {
        std::ifstream in(filename);
        if(!in.is_open())
        {
            return false;
        }
        std::string line;
        while(std::getline(in, line))
        { 
            *out += line;
        }
        in.close();
        return true;
    }


    static bool ReadOneLine(std::string& bigstr,std::string *out,const std::string &sep)
    {
        auto pos = bigstr.find(sep);
        if(pos==std::string::npos)
        {
            return false;
        }
        *out = bigstr.substr(0, pos);
        bigstr.erase(0,pos+sizeof(sep));
        return true;
    }

    static int FileSize(const std::string &filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if(!in.is_open())
            return -1;
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg);
        in.close();
        return filesize;
    }
    

};