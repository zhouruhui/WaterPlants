一、目标
设计一种基于阿里云IOT的自动灌溉系统，该系统可以通过手机APP远程设置灌溉相关的配置参数，参数通过云端下发控制设备端行为，设备端也可以实时上传传感器数据用于监测系统状态。
二、端侧
1.端侧系统组成
<img width="1120" alt="image" src="https://github.com/user-attachments/assets/51a5099a-0d05-4093-8c7e-fdf8855a2aca">
2.物料BOM
3.组装调试

三、平台侧
1.物模型
<img width="1228" alt="image" src="https://github.com/user-attachments/assets/fd6bd7c5-5a11-4b54-a932-b3f972ae7c23">

四、项目坑点
1.Windows平台，Arduino的库是放在C:\Users\xxx\Documents\Arduino\libraries内，下载解压到这个目录就可以。
2.void DFRobot_Aliyun :: setConfig()函数中，ALIYUN_SERVER地址前面不需要加ProductKey，否则阿里云平台连不上。
3.设备端订阅消息，不需要set_reply。
4.在loop函数中使用millis()函数来实现非阻塞延时，这样可以在等待期间继续处理MQTT消息。
5.阿里云平台在解析上传的布尔类型数据的时候，只认识0和1，不认识true和false，所以端侧需要将数据定义为int型。
6.阿里云平台分物联网平台和家庭物联网平台，两者在灵活性，价格和功能上有很大区别，且不能是同一账号，个人开发者一般选用家庭物联网平台即可。

五、ToDo
1.租用一个云主机，基于MQTT代理重新实现云端应用。
