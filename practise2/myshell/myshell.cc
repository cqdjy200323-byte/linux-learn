#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <unordered_map>

#define COMMAND_SIZE 1024
#define FORMAT "[%s@%s %s]# "

// 下面是shell定义的全局数据

// 1. 命令行参数表
#define MAXARGC 128
char *g_argv[MAXARGC];
int g_argc = 0; 

// 2. 环境变量表
#define MAX_ENVS 100
char *g_env[MAX_ENVS];
int g_envs = 0;

// 3. 别名映射表
std::unordered_map<std::string, std::string> alias_list;

// for test
char cwd[1024];
char cwdenv[1024];

// last exit code
int lastcode = 0;

const char *GetUserName()
{
    const char *name = getenv("USER");
    return name == NULL ? "None" : name;
}

const char *GetHostName()
{
    const char *hostname = getenv("HOSTNAME");
    return hostname == NULL ? "None" : hostname;
}


const char *GetPwd()
{
    //const char *pwd = getenv("PWD");
    const char *pwd = getcwd(cwd, sizeof(cwd));
    if(pwd != NULL)
    {
        snprintf(cwdenv, sizeof(cwdenv), "PWD=%s", cwd);
        putenv(cwdenv);
    }
    return pwd == NULL ? "None" : pwd;
}

const char *GetHome()
{
    const char *home = getenv("HOME");
    return home == NULL ? "" : home;
}

std::string DirName(const char* pwd)
{
#define SLASH "/"
  std::string dir =pwd;
  if(dir==SLASH) return SLASH;
  auto pos = dir.rfind(SLASH);
  if(pos == std::string::npos) return "BUG";
  return dir.substr(pos+1);

}

void MakeCommandLine(char cmd_prompt[],int size)
{
  snprintf(cmd_prompt,size,FORMAT,GetUserName(), GetHostName(), DirName(GetPwd()).c_str());

}


void PrintCommandPrompt()
{
  char prompt[COMMAND_SIZE];
  MakeCommandLine(prompt, sizeof(prompt));
  printf("%s",prompt);
  fflush(stdout);
}

void InitEnv()
{
  extern char** environ;
  memset(g_env,0,sizeof(g_env));
  g_envs=0;
  for(int i = 0; environ[i]; i++)
    {
        // 1.1 申请空间
        g_env[i] = (char*)malloc(strlen(environ[i])+1);
        strcpy(g_env[i], environ[i]);
        g_envs++;
    }

    g_env[g_envs] = NULL;

    //2. 导成环境变量
    for(int i = 0; g_env[i]; i++)
    {
        putenv(g_env[i]);
    }
    environ = g_env;
}

bool Cd()
{
    // cd argc = 1
    if(g_argc == 1)
    {
        std::string home = GetHome();
        if(home.empty()) return true;
        chdir(home.c_str());
    }
    else
    {
        std::string where = g_argv[1];
        // cd - / cd ~
        if(where == "-")
        {
            // Todu
        }
        else if(where == "~")
        {
            // Todu
        }
        else
        {
            chdir(where.c_str());
        }
    }
    return true;
}

void Echo()
{
    if(g_argc == 2)
    {
        // echo "hello world"
        // echo $?
        // echo $PATH
        std::string opt = g_argv[1];
        if(opt == "$?")
        {
            std::cout << lastcode << std::endl;
            lastcode = 0;
        }
        else if(opt[0] == '$')
        {
            std::string env_name = opt.substr(1);
            const char *env_value = getenv(env_name.c_str());
            if(env_value)
                std::cout << env_value << std::endl;
        }
        else
        {
            std::cout << opt << std::endl;
        }
    }
}
bool GetCommandLine(char* out ,int size)
{
  char *c = fgets(out,size,stdin);
  if(c==NULL) return false;
  out[strlen(out)-1]=0;
  if(strlen(out)==0) return false;
  return true;
}

bool CommandParse(char* commandline)
{
#define SEP " "
  g_argc = 0;
    // 命令行分析 "ls -a -l" -> "ls" "-a" "-l"
    g_argv[g_argc++] = strtok(commandline, SEP);
    while((bool)(g_argv[g_argc++] = strtok(nullptr, SEP)));
    g_argc--;
    return g_argc > 0 ? true:false;
}

bool CheckAndExecBuiltin()
{
    std::string cmd = g_argv[0];
    if(cmd == "cd")
    {
        Cd();
        return true;
    }
    else if(cmd == "echo")
    {
        Echo();
        return true;
    }
    else if(cmd == "export")
    {
    }
    else if(cmd == "alias")
    {
       // std::string nickname = g_argv[1];
       // alias_list.insert(k, v);
    }

    return false;
}

int Execute()
{
  pid_t id =fork();
  if(id==0)
  {
    execvp(g_argv[0],g_argv);
    exit(1);
  }
  int status=0;
  pid_t rid=waitpid(id,&status,0);
  if(rid>0)
  {
    lastcode=WEXITSTATUS(status);
  }
  return 0;
}

int main()
{
    // shell 启动的时候，从系统中获取环境变量
    InitEnv();

    while(true)
    {
        // 1. 输出命令行提示符
        PrintCommandPrompt();

        // 2. 获取用户输入的命令
        char commandline[COMMAND_SIZE];
        if(!GetCommandLine(commandline, sizeof(commandline)))
            continue;

        // 3. 命令行分析 "ls -a -l" -> "ls" "-a" "-l"
        if(!CommandParse(commandline))
            continue;
        //PrintArgv();

        // 检测别名
        // 4. 检测并处理内键命令
        if(CheckAndExecBuiltin())
            continue;

        // 5. 执行命令
        Execute();
    }
    //cleanup();
    return 0;
}
