一、目标

设计一种基于阿里云IOT的自动灌溉系统，该系统可以通过手机APP远程设置灌溉相关的配置参数，通过云端下发参数，控制设备端行为，设备端也可以实时上传传感器数据用于监测系统状态。

二、端侧

1.端侧系统组成
<img width="1120" alt="image" src="https://github.com/user-attachments/assets/51a5099a-0d05-4093-8c7e-fdf8855a2aca">

2.关键物料

1)主板：https://wiki.dfrobot.com.cn/_SKU_DFR0478_FireBeetle_Board_ESP32%E4%B8%BB%E6%9D%BF%E6%8E%A7%E5%88%B6%E5%99%A8V4_0

2)动态功率点跟踪电源板：https://wiki.dfrobot.com.cn/_SKU_DFR0580_Solar_Power_Manager_For_12V_Lead-Acid_Battery_%E5%A4%AA%E9%98%B3%E8%83%BD%E7%94%B5%E6%BA%90%E7%AE%A1%E7%90%86%E6%A8%A1%E5%9D%97_12V%E8%93%84%E7%94%B5%E6%B1%A0%E7%89%88_

3)土壤湿度传感器：https://item.taobao.com/item.htm?spm=a1z09.2.0.0.37292e8dRwYtQC&id=646485871489&_u=d2jpn9i955b&pisk=foojmZfCesfjU_k7srpzPqjOFpq_hjteXOwtKAIVBoEY5GHqORSa7E21f7MpnS8MofgS6b44M5zafRGUZxSwnxJ6fAHLMlu4gRhr1bbqgcPZahMnOrSqBcWmrXkd0m80bGZ0jldeTH-E3xqgX5bl_Nfcy-2HbiUY6ZtS3LReTH-6kTE6GBuNJ9L7NRN8HRFTXLZ8BR_YXPFYwLwTQsQODfHJFRV8HReTM3Q8K-NtMJWQCENmhLKfeXt56ODzNGICK-avtxbNYMr_Hrits7azz0wblWUr-BwtDbkt0m4klZEiU2GsWXdPuS3QdbaElBsjOj2tCRhXx9N-cAi7o45AkJZbGynYJhSshP3SVoovswDrFSUQ448kgPrjG2V3kUAo1YNq6maRGQPZ8YoYPXdPVf4sWmr-DQKC4PIUO-Ff5Tahf8NeFL_G7Otx4kSTYsg7k82bzL95MV4YE8NeFL_GSreuh7JWFs3G.

4)非接触液位开关：https://detail.tmall.com/item.htm?id=635040367802&spm=a1z09.2.0.0.37292e8dRwYtQC&_u=d2jpn9i206e&pisk=f3ZZm9_OQGIaIazmTXiqLRH_mYmtq0C5ioGjn-2mCfcGkAsD0-yohOCAByk0GWBABcwmgSPI9nMfWsK4T5wu5VNb5jDmtW26fjstuSy7wStfXPMD0WwxoSZ4Hnk0nSBtGOQQBRnxm_1S0gwTBrLPcKEZneD3pxMmn9UGmzmxm_1VQrdrBmFWEwfl_2XEhxtmiS0itDDStxDisScn-YDWsmm0iJXEhxTDjFDmKMDxTmygnHkSSTk4SC4HBRuZZJ7JmnywVVcufXxcZoHN2b2iTnxm6oeh3Jy1_3eIXoVrF5sHb7y0er0U0BfrMky3jr2p_9mzLyaS7y5M0XZs34mm8Kx0Lm4TwkmlqBkL-Ra0X5J2mAE_Vql-8txxlDqSrzVwHTenjxVxyo1JXf20euUS4GR-Qy2rYgoXMvXieoUwiEuipvl5L9xsS53LmIX90ELx-0HENOM6kE3ipvl5L98vk2D-Lb6sC

5)环境温度传感器：https://detail.tmall.com/item.htm?id=43339849601&spm=a1z09.2.0.0.37292e8dRwYtQC&_u=d2jpn9iaeb1&pisk=fZbjm9MIwxDbYf1WInFyVi0tNWY6ltaUWfOOt13q6ELvfVCVdCuwboAs5L1LiKrgmOs5BTx2Hd-w5C6eEsuaisP_51CpHFS23CBPCTmV3NJNUP1hdnuV6Nlcq95KuZrD7VTDSFeULyzFgsYMWdmoQcDm2BOY7Vd9Mma5g7yULyz_DbL_hJSZvbhvFC9JMCp9W7TJ6CivWhKte7O9_xntkOC8NCvJMCd9HznJtI9tM_hB1o9cG7Ujw9a7jx1DFV3ItItx3siZSVYHMn_OHLty4aOXcpKPxJOOkT5OuZxucmLGaM61X9er0KIBOTtFcJgfdtAO1CBb-X9Rl1_WmgkxDBTXhHQvvPu1GhI5PE7YIDfPNKKBzgru3h8fhMvHDu2lC_9VBZt-h8JNY_7vV9erPOx1XZ8Rk8UC4P3edIpjfbtn5Q9UN7iibfaAzeu98xsWDQAX47NSHGxvZQ9UN7iijndkGLP7NxIG.

6)光伏板：https://detail.tmall.com/item.htm?id=616148416897&spm=a1z09.2.0.0.37292e8dRwYtQC&_u=d2jpn9id888&pisk=fRQKmC0xfR2nujZlR6ZMrP5DdqNMJwCF_95jrLvnVOBOeshhx3vkVUBVH3guYexJyO9G-pfHRQt5EtsHdMvoyaBNp20kRT4JFsSaapAh-bd5f93lx6vhXbLFSM0kKJ8RNtY8n-40o65e4Uw0nURaNlYXgLGIEL161xvKtoa0o65ttBNc1rVzcitBUQTWA3O11dR-RBOCFh96ZdgBN2OSC5dyCLTWA3i6CBAmPXTSMPyJCX9oygTf5QESN2gxkNd_UK1Lx4AORBpHH6_Kmmqy9HpfOdemfpw6f_-RzbuJc6sVUI6Id8pPW1_CctwixpsvwsjROyg6-w5Cc3_bU06vJTKfRhhSJF5HBFd1l7m2INphFwK_iqKk5ZxXRGqaK3YpMT_Fpb35h1fc8h78HA9PxI86waPj2LK54XQckiDonKdoA5Ft40-BsmZ-SAjMphbv6KVLy0oyb5R9n5Ft40-B_Cp097nr4hPN.

7)铅酸电池：https://item.jd.com/100078712533.html

8)水泵：https://item.taobao.com/item.htm?spm=a1z09.2.0.0.37292e8dRwYtQC&id=773642924892&_u=d2jpn9i94b0&pisk=fCSompMLba8SIaHLZix7Jg0RTCzAVbtCY6np9HdUuIRbd438FeJFK6jdy9Rpx6fhtQLUN3IhiO6LFMeW4kvF6sxdeQddiJfRdDF7R3nEt_6aNTdLFMAei_5HR8OpTBfdLaeYXlB5FHtet5ETXtnOhaCo8B8ULKJpdZBryAW5FHOEHXzOuTg3E-Ll4e5e0EJ6LHJyaUo4npvKUHRy8ju2QIReYH5e0EJMdYueT4WqMonesMSf077F-wzlY8jkEUAnvImhjii9oCSvgDPNrLm6_TRmYDR5AtyhnTFoftK5Y1vOwools9W18Ejng5ACmtSP8tGu3hXckNLlo7oHMgpH7H8mLDWDYa110hrZiK_VewS5ZvmeFgI9-CT0LDTp0GLN7_DQCt-yL6T1v5iWa9W1fNKgqf9luOxc4y0q7rAxAKyd02gBzK9D1NlCbv1iEL_4n-0_PUJXe5e0n2gBzK9D6-2m5bTyhLFO.&sku_properties=716:4165777

9)继电器（可选）：https://detail.tmall.com/item.htm?id=15909056050&spm=a1z09.2.0.0.37292e8dRwYtQC&_u=d2jpn9i42f5&pisk=fUNxmlApFgjm0tBgGr6os_zncB_uHSU4zozBscmDCuE8XyL0nAmiCfEUvAOG3jDt6uiuim4mGVM_jkwmc-mM65EzDIAiGlftfyyP7m00iF3_Fodgnrm0eFh4Z-AimilT5kcOxMfhtrzqQfshxfuP5TcI-dO6jm15NHmvo91htrzJoq_3P6bNdWtKbVGshAg7P0u9GqgbfYMS-0Oj5Ig6Va3ZVcGshA9SVq0H1EGsMgjtVEiM6RGQNVB6bHdMs735bDaADC3uaqFxvrNvZQXZkxnQl0IHFmsSFPkT_FRtdrwUb2ZXcGnawzNbdksknmwKWy2TlsOSiSzbdANCbdZKMlHQGYL6Mbzm2b37ONvEq7n0fSH5-BHiNWDIG8XPmAcxJlN4DFd_Az43gYPOv3ian2lSW57BBcH14kF39JxMxD3MhaQJQdkjrp69Z32oDYVKyDbA6dJZUau-xaQJQdkjz4nhkNpwQY7P.

10)摄像头（可选）：https://item.jd.com/100117938010.html



3.编译环境

1)采用Arduino IDE2.3.2及以上版本，打开Arduino IDE，按照https://wiki.dfrobot.com.cn/_SKU_DFR0478_FireBeetle_Board_ESP32%E4%B8%BB%E6%9D%BF%E6%8E%A7%E5%88%B6%E5%99%A8V4_0，步骤设置IDE，并安装板卡对应的支持库。

2)安装OneWire、Seeed_Arduino_mbedtls、DallasTemprature库

3)下载Lib.zip，解压到C:\Users\xxx\Documents\Arduino\libraries内

4.结构设计

<img width="1120" alt="image" src="https://github.com/user-attachments/assets/3f880034-f595-41cb-bfbf-3c1c0adea0ba">


5.组装调试

待更新。。。

三、平台侧

1.物模型
<img width="1120" alt="image" src="https://github.com/user-attachments/assets/fd6bd7c5-5a11-4b54-a932-b3f972ae7c23">

四、坑点

1.Windows平台，Arduino的库是放在C:\Users\xxx\Documents\Arduino\libraries内，下载解压到这个目录就可以。

2.void DFRobot_Aliyun :: setConfig()函数中，ALIYUN_SERVER地址前面不需要加ProductKey，否则阿里云平台连不上。

3.设备端订阅消息，不需要set_reply。

4.在loop函数中使用millis()函数来实现非阻塞延时，这样可以在等待期间继续处理MQTT消息。

5.阿里云平台在解析上传的布尔类型数据的时候，只认识0和1，不认识true和false，所以端侧需要将数据定义为int型。

6.阿里云平台分为“物联网平台”和“家庭物联网平台”，两者在灵活性、价格和功能上有很大区别，且不能是同一账号，个人开发者一般选用家庭物联网平台即可。

五、ToDo

1.租用一个云主机，基于MQTT代理重新实现云端应用，降低对特定云平台架构的依赖。

2.设计一个转接板，方便电源板、主控板、传感器、电池等部件相互连接，减少内部飞线。
