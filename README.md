# discord-proxy

上篇文章中的方法已经失效了很久，也时不时有人问是否有别的设置方法，看了一圈，发现并没有比较独立的设置代理的方案，最简单的是设置系统环境变量来实现updater.node的代理，但是这个设置多或少会影响一些其他程序。

所以我就自己实现了一个DLL劫持方案，将Discord读取环境变量的方法拦截并直接返回。

## 开始使用
方法非常简单，在此下载release.zip并解压version.dll
https://github.com/aiqinxuancai/discord-proxy/releases

将version.dll放在Discord.exe所在目录（如Discord有更新，则可能需要重新放一下）
<img src="./images/1.png" alt="ToukenBrowser">

然后在桌面的快捷方式按照上次添加的代理地址
<img src="./images/2.png" alt="ToukenBrowser">

注意最前面有个空格，地址请根据自身情况调整
```
 --a=--proxy-server=http://127.0.0.1:1081
```
如此就设置完毕了，使用桌面快捷方式运行即可。
