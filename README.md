①编译

![1](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/1.png)

②运行（服务端）

![2](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/2.png)

（客户端）

![3](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/3.png)

其中客户端连接的IP地址是服务器的IP地址，由服务器端ifconfig命令查得
![4](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/4.png)

客户端连接后，服务器会显示连接成功的信息
![5](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/5.png)
（说明：因为图片中是由本机充当客户端，所以显示的客户端IP地址与服务器IP地址相同）

③注册
客户端输入sign up进行注册，填写用户名、密码、邮箱。
![6](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/6.png)

用户登录输入的邮箱查看并输入验证码
![7](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/7.png)
![8](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/8.png)

④登录
显示用户登录成功，可选择输入log in登录或sign up继续注册新用户（这里选择log in登录）
![9](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/9.png)

登录成功，可以进行聊天。
同样地完成其他用户的注册与登录
![10](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/10.png)
![11](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/11.png)

⑤群聊
三人依次发送消息：
ain: hi, I am ain
wzj: hello, I am wzj
xy2: hey, I am xy2
界面左侧显示收到的消息，右侧显示自己发送的消息。

![12](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/12.png)
![13](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/13.png)
![14](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/14.png)

⑥私聊
ain与wzj私聊：
ain发送私聊请求(sendTo)，之后填写要发送的对象wzj以及要发送的内容”How are you?”
![15](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/15.png)

wzj收到私聊消息

![16](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/16.png)

xy2未收到私聊消息

![17](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/17.png)

⑦在线状态查询
输入(online_list)即可发送查询在线状态的请求，显示在线的用户及其IP地址
![18](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/18.png)

⑧禁言
只有第一个登录的人拥有管理员权限可以对成员实施禁言（本例中ain为第一个登录的用户，所以他拥有管理员权限）。ain输入(banned)发起禁言请求，填写用户名xy2对xy2实施禁言操作。
![19](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/19.png)

xy2被禁言，30秒内无法发送消息（输入无效）
![20](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/20.png)

（说明：没有管理员权限的用户使用(banned),会被提示没有管理员权限；同样若管理员禁言对象不存在也会被提示对象不存在）

⑨踢出群聊
同样只有管理员权限的用户可以发起踢出群成员的请求。ain输入(out)发起踢出成员请求，填写用户名wzj踢出该用户。

![21](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/21.png)

此时wzj已被踢出，有任何动作都会直接登出。
![22](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/22.png)

此时xy2进行(online_list)查询，用户wzj已不显示在在线列表。
![23](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/23.png)

⑩登出
ain和xy2输入(end)发出登出请求，退出程序。
![24](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/24.png)
![25](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/25.png)

三个客户端的整体界面情况：

![26](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/26.png)
![27](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/27.png)
![28](https://github.com/Ain-Crad/LAN-chat-under-linux/raw/master/imgs/28.png)