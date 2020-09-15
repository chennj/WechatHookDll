<h2>Wechat Hook</h2>

这个项目的目的是hook微信pc一些常用的功能<br>
完全公开免费!!!<br>
感谢wsbblyy，Hezone<br>
目前项目代码和结构非常简陋, 等功能比较完善时再重构一下

我用的pc微信的版本是2.6.6.28, 如果版本不同的话需要自己找到call的基址

使用方法<br>
先使用visal studio 2015 生成WechatDllCpp.dll<br>
注入WechatDllCpp.dll到微信进程<br>
目前完成的有：发送文本，发送图片，接收消息，获取好友/群列表
