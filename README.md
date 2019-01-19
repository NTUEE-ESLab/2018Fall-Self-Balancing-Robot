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

裝有MPU6050，透過Kalman Filter的值使量出的加速度更精準，以及GB37帶測速馬達，和馬達驅動機TB6612FNG，使左右兩輪的值給的相同，如有速度偏差容易造成小車平衡不穩，容易翻倒，最重要的裝上藍芽接收器HC-06，實現和RPi溝通


### 平衡車零件

<table>
   <tr>
      <td></td>
      <td colspan="5">Hardware</td>
   </tr>
   <tr>
      <td>RPI</td>
      <td>18650 Bat x3</td>
      <td>USB port x1</td>
      <td>SG90 x2</td>
      <td>HC-SR04 x1</td>
      <td>Camera x1</td>
   </tr>
   <tr>
      <td>Arduino</td>
      <td>18650 Bat x3</td>
      <td>MPU6050 x1</td>
      <td>GB37 x2</td>
      <td>TB66 x1</td>
      <td>HC-06 x1</td>
   </tr>
</table>

### 平衡原理

<p align="center">
  <img src="https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_1481.jpg" width="70%" height="70%">
</p> 

原理就是應用負反饋控制，由測量到的角度和自身平衡時的自然角度的差作為誤差，通過一個叫做PID的控制算法來控制電機轉速和轉向，偏離目標角度時，往前倒就向前跑一點，往後倒就向後跑一點，只要這個過程做的足夠快，參數合適，就能穩穩地站在原地。通過MPU6050檢測小車的角度作為PID 函數的輸入，設定一個平衡角度作為PID函數的目標值，然後把PID函數的輸出作為PWM值驅動電機。然而一般市售的平衡車其實不不穩定，仍然會出現小幅度顯而易見的晃動，因此我們希望在不修改硬體的情況下，實現軟體演算法的平衡，我們在陀螺儀的偵測上加上Kalman Filter，它是一種自回歸濾波器，能夠從一系列的不完全及包含雜訊的測量中，估計動態系統的狀態。卡爾曼濾波會根據各測量量在不同時間下的值，考慮各時間下的聯合分布，再產生對未知變數的估計，因此會比只以單一測量為基礎的估計方式要準。加上這個濾波器後，再調上適合這個平地或斜坡的參數，我們可以實現看起來像不動般停在原地，不論在平地或斜坡上都可以。

### 超音波模組

這次我們是使用HC-SR04的超音波模組來測距，其精準範圍為1cm ~ 400cm左
右，其主要碰到的問題為他的廣度沒有辦法涵蓋小車，但我們嘗試在車子的左右各
安裝一個卻發現會互相干擾，只能以伺服馬達的左右掃瞄來確立車子能夠通過是比   
較麻煩的。再來由於rpi可能負荷太多外接模組導致電流有時候會不穩定，會出現異
常的數值，因此我們還是有對超音波模組回傳的值做一個篩選、去掉極值及標準差
比較大的值，此外，在電池電量較低的時候會直接失準，如果有偵測到直接失準程
式要自己結束運行這些都是要被特別處理的部份。

### 伺服馬達

伺服馬達是用SG90，但RPI在使用伺服馬達會碰上一些問題。首先是控制能力
，因為RPI的PWM並沒有特別的優化，因此效果是不甚滿意，在角度上面常常不 
是那麼精準，但這可以事先校正解決。再來是電壓，因為伺服馬達需要更高的電壓
5V，而RPI的電壓進來並沒有經過整流，所以穩定度很差，因此我們一開始直接用
RPI是跑不起來伺服馬達的，後面接了電池座再接一個降壓穩流才有辦法控制。

### 雙軸雲台

此外，我們用一個雙軸雲台接上兩個伺服馬達來達到可以讓超音波模組
和PiCamera來轉向，藉以判別左右的距離和物品，我們之後會參考助教的意見，在辨別圖片時用上下掃描來增加更多的判斷依據以達到更高的正確率。

### Picamera辨識箭頭

先用灰階的方式讀取，再用高斯分佈去模糊、canny演算法取邊緣去描繪輪廓。有了輪廓之後，用opencv的演算法去逼近成多邊形，取出箭頭的七個頂點座標，用我們的演算法去判定是不是箭頭。

<p align="center">
  <img src="https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_1481.jpg" width="70%" height="70%">
</p> 

## 成果

1. 介紹影片

[![Demo](https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_3312.png)](https://www.youtube.com/watch?v=ek1j272iAmc)

2. 閃避示範

[![Demo](https://raw.githubusercontent.com/NTUEE-ESLab/2018Fall-Self-Balancing-Robot/master/img/IMG_1313.png)](https://www.youtube.com/watch?v=SOX47pxRJZg&feature=youtu.be)

## 參考資料

1. [51單片機平衡小車](https://item.taobao.com/item.htm?spm=a211ha.10565794.0.0.3cd53ca9aoSh6g&id=42913336181)

2. [平衡車製作規格](http://wickedlabelectronics.com/self-balancing-robot-projects/)

3. [PID演算法](https://blog.csdn.net/jsgaobiao/article/details/50643037)

