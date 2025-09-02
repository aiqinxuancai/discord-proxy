# Discord Proxy

本工具旨在解决 Discord 桌面客户端必须使用全局代理的问题，通过 DLL 注入实现为 Discord 单独设置代理，而无需修改系统设置。

## 原理

通过 `version.dll` 劫持，在 Discord 启动时加载自定义 DLL。该 DLL 会：
1.  读取并存储通过命令行参数传入的代理地址。
2.  拦截 Discord `updater.node` 模块读取系统环境变量的操作。
3.  返回命令行中设置的代理地址，从而实现局部代理。

## 如何使用

### 方式一：快速开始 (推荐)

推荐使用配套的启动器项目 [DiscordProxyStart](https://github.com/aiqinxuancai/DiscordProxyStart)。

该工具会自动下载并放置 `version.dll`，并提供图形化界面来启动 Discord 和配置代理。支持带认证的代理地址，例如：

```text
http://127.0.0.1:1080
http://user:password@127.0.0.1:1080
socks://127.0.0.1:1080
socks5://127.0.0.1:1080
socks5://user:password@127.0.0.1:1080
```

**[➡️ 前往 DiscordProxyStart 发布页面](https://github.com/aiqinxuancai/DiscordProxyStart/releases)**

---

### 方式二：手动配置

1.  **下载 DLL 文件**
    从本项目的 [Releases 页面](https://github.com/aiqinxuancai/discord-proxy/releases) 下载最新的 `release.zip`，并解压出 `version.dll`。

2.  **放置 DLL 文件**
    将 `version.dll` 文件复制到 Discord 的安装目录下（通常是 `Discord.exe` 所在的文件夹）。如果 Discord 更新，可能需要重新执行此步骤。

    ![DLL copy](images/1.png)

3.  **修改快捷方式**
    找到 Discord 在桌面创建的快捷方式，右键点击“属性”，在“目标”字段的末尾添加代理参数。

    > **注意**：请确保修改的是 Discord 安装时自动创建的快捷方式，其目标通常指向 `Update.exe`，而不是您手动为 `Discord.exe` 创建的快捷方式。

    ![Shortcut properties](images/2.png)

    在“目标”字段的最后，先添加一个**空格**，然后加上如下参数（请根据您的代理地址修改）：
    ```text
     --a=--proxy-server=http://127.0.0.1:1081
    ```

    修改后的完整“目标”内容示例如下：
    ```text
    C:\Users\xxxxxx\AppData\Local\Discord\Update.exe --processStart Discord.exe --a=--proxy-server=http://127.0.0.1:1081
    ```

4.  **启动 Discord**
    双击修改后的桌面快捷方式启动 Discord 即可。

> **重要提示**：手动配置方式仅支持无需用户名和密码认证的 `http` 和 `socks5` 代理协议。

---

## 疑难解答

如果配置后 Discord 仍然无法连接，请尝试以下检查：

#### 1. 检查快捷方式是否正确
请确保您修改的是由 Discord 安装程序创建的快捷方式。
- **正确的目标应指向**: `Update.exe`
- **错误的目标指向**: `Discord.exe`

#### 2. 检查代理客户端的规则
如果您使用 Clash 等基于规则的代理客户端，请确保 `discord.com` 及相关域名没有被设置为“直连”或被其他规则拦截，需要走代理通道。