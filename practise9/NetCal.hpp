#pragma once

#include "Protocol.hpp"
#include <iostream>

class Cal
{
public:
    Response Execute(Request &req)
    {
        Response resp(0, 0); // code: 0表示成功
        switch (req.Oper())
        {
        case '+':
            resp.SetResult(req.X() + req.Y());
            break;
        case '-':
            resp.SetResult(req.X() - req.Y());

            break;
        case '*':
            resp.SetResult(req.X() * req.Y());
            break;
        case '/':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(1); // 1除零错误
            }
            else
            {
                resp.SetResult(req.X() / req.Y());
            }
        }
        break;
        case '%':
        {
            if (req.Y() == 0)
            {
                resp.SetCode(2); // 2 mod 0 错误
            }
            else
            {
                resp.SetResult(req.X() % req.Y());
            }
        }
        break;
        default:
            resp.SetCode(3); // 非法操作
            break;
        }

        return resp;
    }
};