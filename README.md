# discord-proxy

本项目不提供可供连接Discord的代理地址，仅解决Discord必须全局代理的问题，实现可以单独设置代理。

原理是version.dll会在Discord.exe启动时跟随加载，读取并存储命令行中的代理地址(http....)，并且拦截Discord的updater.node模块读取环境变量的操作，将命令行中的代理地址直接返回，这样就无需修改系统的环境变量。

## 如何使用

### 快速开始
[DiscordProxyStart](https://github.com/aiqinxuancai/DiscordProxyStart)  / [Release](https://github.com/aiqinxuancai/DiscordProxyStart/releases)

使用此项目自动复制version.dll并启动Discord，并可支持带有认证的代理地址
```
http://127.0.0.1:1080
http://user:password@127.0.0.1:1080
socks://127.0.0.1:1080
socks5://127.0.0.1:1080
socks5://user:password@127.0.0.1:1080
...
```

---

### 手动配置
手动下载release.zip并解压version.dll

https://github.com/aiqinxuancai/discord-proxy/releases

将version.dll放在Discord.exe所在目录（如Discord有更新，则可能需要重新放一下）
<p align="center" color="#6a737d">
<img src="./images/1.png" alt="discord-proxy">
</p>


然后在桌面的快捷方式按照上次添加的代理地址（Discord安装时自动创建的那个快捷方式，不要自行从Discord.exe创建，具体请见底部）
<p align="center" color="#6a737d">
<img src="./images/2.png" alt="discord-proxy">
</p>

注意最前面有个空格，地址请根据自身情况调整
```
 --a=--proxy-server=http://127.0.0.1:1081
```
如此就设置完毕了，使用桌面快捷方式运行即可。

**注意：手动配置仅可支持无需验证的http及socks5协议。**

---

### **如果你还是进不去，请看这里：**

#### **检查快捷方式对不对**
上面所说的桌面快捷方式为安装时自动创建的快捷方式，目标指向是**Update.exe**，而非自行从Discord.exe所创建的，修改完毕后的完整命令行样例：
```
C:\Users\xxxxxx\AppData\Local\Discord\Update.exe --processStart Discord.exe --a=--proxy-server=http://127.0.0.1:1081
```
#### **注意代理规则**
如果用的**clash代理**，请注意下代理规则，检查discord相关域名流量是否通过代理，v2、ss、ssr为指定端口全代所以不用另外做什么。

