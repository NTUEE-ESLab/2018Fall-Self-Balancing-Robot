# 兩輪自走平衡車
<p align="center">
  <img src="https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_1481.jpg" width="70%" height="70%">
</p> 

## 動機

一般四輪車為車子的縮版，雖然具備馬力較強移動速度較快等優點，但實際需要靈活操作技巧的工作，其操作技巧難易度遠遠遜色於兩輪車，且兩輪車續航力較高，走路起來也較像人類兩條腿的走路方式，因此我們這一組想實踐兩輪車的實際製作，但這堂課的重點顯然不是在硬體製作，因此我們買來現有的平衡車，去對他做平衡性能的優化，以及加上我們的遇到障礙物被擋住後，相機開啟偵測方向的功能。

## 架構

裝有MPU6050，透過Kalman Filter的值使量出的加速度更精準，以及GB37帶測速馬達，和馬達驅動機TB6612FNG，使左右兩輪的值給的相同，如有速度偏差容易造成小車平衡不穩，容易翻倒，最重要的裝上藍芽接收器HC-06，實現和RPi溝通

### RPI

有USB port，使電池座能夠穩定地降壓穩流輸出5V3A的電流，SG90伺服馬達兩組，實現雲台雙軸鏡頭的轉動，以及HC-SR04超音波模組實現遇到障礙物停止移動告訴camera開始辨識的功能，PiCamera搭配opencv實現便是箭頭指示的功能。


### Arduino

### 平衡車零件

|            |Hardware    |                                                   |
|:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|
|RPI         |18650Bat x3 |USB port x1 |SG90 x2     |HC-SR04 x1  |Camera X1   |
|Arduino     |18650Bat x3 |MPU6050 x1  |GB37 x2     |TB66 x1     | HC-06 x1   |



## 成果

1. 介紹影片

[![Demo](https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_3312.png)](https://www.youtube.com/watch?v=ek1j272iAmc)

2. 閃避示範

[![Demo](https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_1313.png)](https://www.youtube.com/watch?v=SOX47pxRJZg&feature=youtu.be)

## 參考資料

1. [51單片機平衡小車](https://item.taobao.com/item.htm?spm=a211ha.10565794.0.0.3cd53ca9aoSh6g&id=42913336181)

2. [平衡車製作規格](http://wickedlabelectronics.com/self-balancing-robot-projects/)

3. [PID演算法](https://blog.csdn.net/jsgaobiao/article/details/50643037)

