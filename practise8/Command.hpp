#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <set>
#include "Command.hpp"
#include "InetAddr.hpp"
#include "Log.hpp"

using namespace LogModule;

class Command
{
public:
    // ls -a && rm -rf
    // ls -a; rm -rf
    Command()
    {
        // 严格匹配
        _WhiteListCommands.insert("ls");
        _WhiteListCommands.insert("pwd");
        _WhiteListCommands.insert("ls -l");
        _WhiteListCommands.insert("touch haha.txt");
        _WhiteListCommands.insert("who");
        _WhiteListCommands.insert("whoami");
    }
    bool IsSafeCommand(const std::string &cmd)
    {
        auto iter = _WhiteListCommands.find(cmd);
        return iter != _WhiteListCommands.end();
    }
    std::string Execute(const std::string &cmd, InetAddr &addr)
    {
        // 1. 属于白名单命令
        if(!IsSafeCommand(cmd))
        {
            return std::string("坏人");
        }

        std::string who = addr.StringAddr();

        // 2. 执行命令
        FILE *fp = popen(cmd.c_str(), "r");
        if(nullptr == fp)
        {
            return std::string("你要执行的命令不存在: ") + cmd;
        }
        std::string res;
        char line[1024];
        while(fgets(line, sizeof(line), fp))
        {
            res += line;
        }
        pclose(fp);
        std::string result = who + "execute done, result is: \n" + res;
        LOG(LogLevel::DEBUG) << result;
        return result;
    }
    ~Command()
    {}
private:
    // 受限制的远程执行
    std::set<std::string> _WhiteListCommands;
};